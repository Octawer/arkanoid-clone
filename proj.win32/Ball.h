#ifndef __ARKA_BALL_H__
#define __ARKA_BALL_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"
#include "CollisionManager.h"
#include "VisibleRect.h"
#include "Pad.h"
#include "Block.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class Ball  : public CCPhysicsSprite		// we are extending CCPhysicsSprite, provided that we use Chipmunk
{

private:

	cpShape* _shape;
	static Ball* createBall(const char * spriteFileName, float x, float y);
	bool init(float x, float y);
	void bounceWithPad(cpArbiter *arb, cpShape* b);
	void bounceWithWall(cpShape* shape);
	void bounceWithBlock(cpArbiter *arb, cpShape* b);
	
public:

	// getters and setters for ball properties
	CC_SYNTHESIZE(float, _x, X);
	CC_SYNTHESIZE(float, _y, Y);
	CC_SYNTHESIZE(float, _height, Height);
	CC_SYNTHESIZE(float, _width, Width);
	CC_SYNTHESIZE(float, _speed, Speed);
	// create ball and physics body and shape, and custom init
	static Ball* addNewBallAtPosition(CCPoint p, cpSpace* space);
	// generic bounce that calls private methods for each collision case
	void bounce(cpArbiter *arb, cpShape* b);
	// destroy ball body, shape, detach them from physics space and call destructor
	void exileBall(cpSpace* space);
	~Ball();

};

#endif //__ARKA_BALL_H__