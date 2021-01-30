#include "Booster.h"

// it's an autorelease object
Booster* Booster::createBooster(const char * spriteFileName, float x, float y, int type) 
{
	Booster *booster = new Booster();  
	if (booster && booster->initWithFile(spriteFileName))  
	{  
		booster->init(x,y,type);
		booster->autorelease();  
		return booster;  
	}  
	CC_SAFE_DELETE(booster);  
	return NULL; 
}

bool Booster::init(float x, float y, int type) 
{
	_x = x;  
	_y = y;
	_type = type;
	_width = getTextureRect().size.width;  
	_height = getTextureRect().size.height;
	return true;
}

Booster* Booster::addNewBoosterAtPosition(CCPoint pos, cpSpace* space, int type)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION   

	// create booster sprites according to the type
	const char* boosterName;
	switch(type){
	case Bullet:
		boosterName = "powerups/bullet.png";
		break;
	case Wider:
		boosterName = "powerups/wider.png";
		break;
	case Life:
		boosterName = "powerups/1up.png";
		break;
	case Double:
		boosterName = "powerups/extra_ball.png";
		break;
	default:
		boosterName = "powerups/1up.png";
	}

	Booster* _booster = Booster::createBooster(boosterName,pos.x,pos.y, type);

    int num = 4;

	// we set the body and shape vertexes
	// according to imagen pixels (points) (mid,mid) : eg 129x33 size o pad
	cpVect verts[] = {

		cpv(-(_booster->_width/2),-(_booster->_height/2)),

		cpv(-(_booster->_width/2), (_booster->_height/2)),

		cpv( (_booster->_width/2), (_booster->_height/2)),

		cpv( (_booster->_width/2),-(_booster->_height/2)),
	};

	// we create a chipmunk body and shape and assign it to out sprite (doesnt matter much
	// the mass and moment of inertia here, as it will only collide with pad and be destroyed after)
	cpBody *body = cpBodyNew(1.0f, cpMomentForPoly(INFINITY, num, verts, cpvzero));

    body->p = cpv(pos.x, pos.y);
    cpSpaceAddBody(space, body);

	// create shape and set its parameters (elasticity and friction)
	_booster->_shape = cpPolyShapeNew(body, num, verts, cpvzero);
    _booster->_shape->e = 1.0f; 
	_booster->_shape->u = 0.5f;

	// set collision type, user data linked to the shape (the actual booster object) and add it to the space
	cpShapeSetCollisionType(_booster->_shape,CollisionManager::BoosterCollision);
	cpShapeSetUserData(_booster->_shape,_booster);					
    cpSpaceAddShape(space, _booster->_shape);

	// set the body, the initial position and space
    _booster->setCPBody(body);
    _booster->setPosition(pos);
	_booster->setTag(3);

	_booster->_space = space;

	// impulse downwards when its released (from block), as it will be 0 gravity in game
	cpBodyApplyImpulse(_booster->getCPBody(), cpv(0,-250), cpv(0,0));

	return _booster;
#endif
}

// callback to remove Shapes from the Body
void removeBoosterShape( cpBody *body, cpShape *shape, void *data )
{
	cpShapeFree( shape );
}

Booster::~Booster()
{
	// dealloc the body's shapes
	cpBodyEachShape(getCPBody(), removeBoosterShape, NULL);
	// dealloc the body
	cpBodyFree( getCPBody() );
}

void Booster::banishBooster()
{
		cpSpaceRemoveBody(_space, _shape->body);
		// cpBodyFree(shape->body);

		cpSpaceRemoveShape(_space, _shape);
		// cpShapeFree(shape);
		this->removeFromParentAndCleanup(true);
}