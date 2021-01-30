#include "Pad.h"

// it's another autorelease object
Pad* Pad::createPad(const char * spriteFileName, float x, float y) 
{
	Pad *pad = new Pad();  
	if (pad && pad->initWithFile(spriteFileName))  
	{  
		pad->init(x,y);
		pad->autorelease();  
		return pad;  
	}  
	CC_SAFE_DELETE(pad);  
	return NULL; 
}

// init its various fields
bool Pad::init(float x, float y) 
{
	_x = x;  
	_y = y;
	_shootingPad = false;
	_widerPad = false;
	_lifeBoost = false;
	_doubleBoost = false;
	_shootingTime = 0;
	_widerTime = 0;
	_width = getTextureRect().size.width;  
	_height = getTextureRect().size.height;
	return true;
}

// callback to remove Shapes from the Space
void removeShape( cpBody *body, cpShape *shape, void *data )
{
	cpShapeFree( shape );
}

Pad::~Pad()
{
	// dealloc the body
	cpBodyEachShape(getCPBody(), removeShape, NULL);
	// cpBodyFree( body_ );
	cpBodyFree( getCPBody() );
	// how to dealloc the shape attached to that body?
}


Pad* Pad::addNewPadAtPosition(CCPoint pos, cpSpace* space)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION   

	// create pad sprite
	Pad* _pad = Pad::createPad("paddle/paddle_complete.png",pos.x,pos.y);

	int num = 4;

	// set the shape and body vertexes according to the sprite texture dimensions (enclosing rectangle)
	cpVect verts[] = {

		cpv(-(_pad->_width/2),-(_pad->_height/2)),

		cpv(-(_pad->_width/2), (_pad->_height/2)),

		cpv( (_pad->_width/2), (_pad->_height/2)),

		cpv( (_pad->_width/2),-(_pad->_height/2)),
	};

	// we create a chipmunk body and shape and assign it to out sprite
	// we make its mass and moment of inertia infinite to make it unmovable and unturnable against ball collisions
	cpBody *body = cpBodyNew(INFINITY, cpMomentForPoly(INFINITY, num, verts, cpvzero));

	body->p = cpv(pos.x, pos.y);
	cpSpaceAddBody(space, body);

	// we set the shape and its parameters
	cpShape* shape = cpPolyShapeNew(body, num, verts, cpvzero);
	shape->e = 1.0f; 
	shape->u = 0.5f;

	// we set collision type, user data and add it to space
	cpShapeSetCollisionType(shape,CollisionManager::PadCollision);
	cpShapeSetUserData(shape,_pad);					
	cpSpaceAddShape(space, shape);

	// we set pad's body, initial position and chipmunk space
	_pad->setCPBody(body);
	_pad->setPosition(pos);
	_pad->setTag(1);
	_pad->_space = space;

	return _pad;
#endif
}

// we boost the pad according to the type of booster that collided with it
void Pad::boost(int type)
{
	// we check the booster type and act accordingly
	switch(type){
	case Booster::Bullet:
		// check if pad isnt yet boosted (to make booster effect idempotent)
		if(!_shootingPad) {
			// set flag to true
			_shootingPad = true;
			// change texture to laser_paddle
			this->setTexture(CCTextureCache::sharedTextureCache()->addImage("paddle/laser_paddle.png"));
			// on entering shooting state we schedule the powerUp timer (called every sec)
			schedule(schedule_selector(Pad::powerUpCounter),1.0f);
		}
		break;
	case Booster::Wider:
		// check if pad isnt yet boosted (to make booster effect idempotent)
		if(!_widerPad) {
			// set flag to true
			_widerPad = true;
			// scale the pad by 150% in x axis and same (100%) in y axis
			this->runAction(CCScaleTo::create(0.1f,1.5f,1.0f));
			// on entering wider state we schedule the powerUp timer (called every sec)
			schedule(schedule_selector(Pad::powerUpCounter),1.0f);
		}
		break;
	case Booster::Life:
		// set flag and make GameScene poll them in update fc. (see if its preferrable an event driven approach)
		_lifeBoost = true;
		break;
	case Booster::Double:
		// set flag and make GameScene poll them in update fc.
		_doubleBoost = true;
		break;
	default:
		// ignore (maybe CCLOG)
		break;
	}
}

// shoot action, enabled on clicking inside pad, only if it's powered with bullet booster
void Pad::shoot()
{
	if(_shootingPad) 
	{
		CCLayer* gameScene = (CCLayer*)cpSpaceGetUserData(_space);

		// we instantiate one projectile on each pad side
		Bullet* bullet1 = Bullet::addNewBulletAtPosition(ccp(getPosition().x - _width / 4, getPosition().y),_space,Booster::Bullet);
		gameScene->addChild(bullet1);

		Bullet* bullet2 = Bullet::addNewBulletAtPosition(ccp(getPosition().x + _width / 4, getPosition().y),_space,Booster::Bullet);
		gameScene->addChild(bullet2);
	}
}

void Pad::powerUpCounter(float delta)
{
	// if pad is shooting powered
	if(_shootingPad)  {
		// every second (this selector callback is called evety 1 sec) we increment 1 sec of shooting time counter
		_shootingTime++;
		// check if powerup time passed
		if(_shootingTime >= 3) 
		{
			// restore timer, flag and sprite
			_shootingTime = 0;
			_shootingPad = false;
			this->setTexture(CCTextureCache::sharedTextureCache()->addImage("paddle/paddle_complete.png"));
			// if pad is not powered by wider booster we unschedule powerUp time
			// otherwise we delegate this to the "_widerPad" part
			if(!_widerPad){
				unschedule(schedule_selector(Pad::powerUpCounter));
			}
		}
	} 
	// if pad is wider powered
	if (_widerPad) {
		// every second (this selector callback is called evety 1 sec) we increment 1 sec of widerpad time counter
		_widerTime++;
		// check if we reach timer limits (7 for blink and 10 for restore pad)
		if(_widerTime >= 7) 
		{
			// if pad isnt tinted we change its color
			if(getColor().r == 255 && getColor().g == 255 && getColor().b == 255) {
				this->runAction(CCTintTo::create(0.1f,0,255,0));
			// otherwise we restore its original color
			} else {
				this->runAction(CCTintTo::create(0.1f,255,255,255));
			}
		}
		// if we reached last limit (10 secs)
		if(_widerTime >= 10) {
			// restore timer, flag, scale back to normal, restore color, and end schedule selector callback
			_widerTime = 0;
			_widerPad = false;
			this->runAction(CCScaleTo::create(0.1f,1.0f,1.0f));
			this->setColor(ccWHITE);
			unschedule(schedule_selector(Pad::powerUpCounter));	
		}
	}
}


