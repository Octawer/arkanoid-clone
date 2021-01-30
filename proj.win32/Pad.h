#ifndef __ARKA_PAD_H__
#define __ARKA_PAD_H__

#include "cocos2d.h"
#include "chipmunk.h" 
#include "cocos-ext.h"	
#include "CollisionManager.h"
#include "Booster.h"
#include "Bullet.h"

//
using namespace cocos2d;
USING_NS_CC_EXT;

class Pad  : public CCPhysicsSprite		// extending CCPhysicsSprite, provided that we use Chipmunk
{

private:
	cpShape* _shape;
	cpSpace* _space;

	bool init(float x, float y);
	static Pad* createPad(const char * spriteFileName, float x, float y);

public:
	
	// pad properties and its getters and setters
	CC_SYNTHESIZE(float, _x, X);
	CC_SYNTHESIZE(float, _y, Y);
	CC_SYNTHESIZE(float, _height, Height);
	CC_SYNTHESIZE(float, _width, Width);
	CC_SYNTHESIZE(bool, _shootingPad, ShootingPad);
	CC_SYNTHESIZE(bool, _widerPad, WiderPad);
	CC_SYNTHESIZE(float, _shootingTime, ShootingTime);
	CC_SYNTHESIZE(float, _widerTime, WiderTime);
	CC_SYNTHESIZE(bool, _lifeBoost, LifeBoost);
	CC_SYNTHESIZE(bool, _doubleBoost, DoubleBoost);
	
	static Pad* addNewPadAtPosition(CCPoint p, cpSpace* space);				

	void boost(int type);
	void shoot();
	// schedule selector callback to count powerUp state time left
	void powerUpCounter(float delta);

	~Pad();

};

#endif //__ARKA_PAD_H__