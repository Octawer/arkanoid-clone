#ifndef __ARKA_BOOSTER_H__
#define __ARKA_BOOSTER_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"	
#include "CollisionManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class Booster : public CCPhysicsSprite
{

private:
	cpShape* _shape;
	cpSpace* _space;

	static Booster* createBooster(const char * spriteFileName, float x, float y, int type);
	bool init(float x, float y, int type);
public:
	// booster types (shooting pad, wider pad, extra life, extra ball)
	enum{ Bullet, Wider, Life, Double };
	CC_SYNTHESIZE(float, _x, X);
	CC_SYNTHESIZE(float, _y, Y);
	CC_SYNTHESIZE(float, _height, Height);
	CC_SYNTHESIZE(float, _width, Width);
	CC_SYNTHESIZE(int, _type, Type);
	
	static Booster* addNewBoosterAtPosition(CCPoint p, cpSpace* space, int type);						
	
	// remove the booster from space and dealloc its components (body, shape and the object itself)
	void banishBooster();
	~Booster();
};

#endif // __ARKA_BOOSTER_H__