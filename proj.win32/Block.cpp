#include "Block.h"

// it's an autorelease object
Block* Block::createBlock(const char * spriteFileName, float x, float y, int type) 
{
	Block *block = new Block();  
	if (block && block->initWithFile(spriteFileName))  
	{  
		block->init(x,y, type);
		block->autorelease();  
		return block;  
	}  
	CC_SAFE_DELETE(block);  
	return NULL; 
}

bool Block::init(float x, float y, int type) 
{
	_x = x;  
	_y = y;
	_dropChance = 25; // per 100
	// init HP according to param
	if(type >= 0 && type < 3) {
		_hp = type + 1;		// starts at 0 the enum
	} else {
		_hp = 1;	// 1 by default
	}
	_width = getTextureRect().size.width;  
	_height = getTextureRect().size.height;
	CCLOG("Block initial position (%.2f,%.2f)", _x,_y);
	CCLOG("Block size: height (%.2f) width (%.2f)",_height, _width);
	return true;
}


Block* Block::addNewBlockAtPosition(CCPoint pos, cpSpace* space, int type)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION   

	// create custom sprites according to type parameter (.png files)
	const char* brickName;
	if(type == Red) {
		brickName = "bricks/brick_red.png";
	} else if (type == Yellow) {
		brickName = "bricks/brick_yellow.png";
	} else if (type == Blue) {
		brickName = "bricks/brick_blue.png";
	} else {
		// default color - 1 hit red block
		brickName = "bricks/brick_red.png";
	}

	Block* _block = Block::createBlock(brickName,pos.x,pos.y, type);

    int num = 4;
	// we set the body and shape vertexes
	// according to imagen pixels (points) (mid,mid) : eg 129x33 size o pad
	cpVect verts[] = {

		cpv(-(_block->_width/2),-(_block->_height/2)),

		cpv(-(_block->_width/2), (_block->_height/2)),

		cpv( (_block->_width/2), (_block->_height/2)),

		cpv( (_block->_width/2),-(_block->_height/2)),
	};

	// we create a chipmunk body and shape and assign it to out sprite (unmovable (infinite mass) and unturnable (infinite moment of inertia) 
	cpBody *body = cpBodyNew(INFINITY, cpMomentForPoly(INFINITY, num, verts, cpvzero));
	// cpBody *body = cpBodyNewStatic();		// maybe another approach

    body->p = cpv(pos.x, pos.y);
    cpSpaceAddBody(space, body);

	// set the shape, elasticity (e) and friction coefficient (u)
	_block->_shape = cpPolyShapeNew(body, num, verts, cpvzero);
    _block->_shape->e = 1.0f; 
	_block->_shape->u = 0.5f;

	// set the collision type, user data (the actual ball object) and add to physical chipmunk space
	cpShapeSetCollisionType(_block->_shape,CollisionManager::BlockCollision);
	cpShapeSetUserData(_block->_shape,_block);					
    cpSpaceAddShape(space, _block->_shape);

	// set body and initial position
    _block->setCPBody(body);
    _block->setPosition(pos);
	_block->setTag(5);

	_block->_space = space;

	return _block;
#endif
}

void Block::changeColor(int type)
{
	switch(type){
	case Red:
		//change color to red
		this->setTexture(CCTextureCache::sharedTextureCache()->addImage("bricks/brick_red.png"));
		break;
	case Yellow:
		//change color to yellow
		this->setTexture(CCTextureCache::sharedTextureCache()->addImage("bricks/brick_yellow.png"));
		break;
	case Blue:
		//change color to blue
		this->setTexture(CCTextureCache::sharedTextureCache()->addImage("bricks/brick_blue.png"));
		break;
	default:
		this->setTexture(CCTextureCache::sharedTextureCache()->addImage("bricks/brick_red.png"));
	}
}

void Block::takeDamage()
{
	// subtract 1 to the block hp
	_hp--;
	// change the color
	changeColor(_hp-1);
	// if hp equal or below 0
	if(_hp <= 0) 
	{
		releaseBooster();
		// remove body and shape from chipmunk space (cpSpace)
		cpSpaceRemoveBody(_space, _shape->body);
		cpSpaceRemoveShape(_space, _shape);
		// dealloc body, shape and actual block (detach it from parent, autorelease object)
		this->removeFromParentAndCleanup(true);
	}
}

void Block::releaseBooster()
{
	// we roll the dice
	int die = rand() % 100;
	// and drop booster by chance
	if(die < _dropChance )
	{
		// we drop one of the 4 boosters randomly
		int boosterType = rand() % 4;				// [0-3]
		_booster = Booster::addNewBoosterAtPosition(this->getPosition(),_space,boosterType);
		// add the booster to the actual gameScene (user data set before in cpSpace)
		CCLayer* gameScene = (CCLayer*)cpSpaceGetUserData(_space);	
		gameScene->addChild(_booster);
	}
}

// callback to remove Shapes from the Body
void removeBlockShape( cpBody *body, cpShape *shape, void *data )
{
	cpShapeFree( shape );
}

Block::~Block()
{
	// dealloc the body's shapes
	cpBodyEachShape(getCPBody(), removeBlockShape, NULL);
	// dealloc the body
	cpBodyFree( getCPBody() );
}