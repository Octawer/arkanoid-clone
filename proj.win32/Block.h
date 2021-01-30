#ifndef __ARKA_BLOCK_H__
#define __ARKA_BLOCK_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"
#include "Booster.h"
#include "CollisionManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

class Block : public CCPhysicsSprite
{

private:

	bool hasBooster; // whether it has a booster sprite
	int hp; // number of hit points
	Booster* _booster;						
	cpShape* _shape;
	cpSpace* _space;

	static Block* createBlock(const char * spriteFileName, float x, float y, int type);
	bool init(float x, float y, int type);
	void releaseBooster();
	void changeColor(int type);

public:
	// block types
	enum{ Red, Yellow, Blue };
	// block properties (properties and getters & setters)
	CC_SYNTHESIZE(float, _x, X);
	CC_SYNTHESIZE(float, _y, Y);
	CC_SYNTHESIZE(float, _height, Height);
	CC_SYNTHESIZE(float, _width, Width);
	CC_SYNTHESIZE(int, _dropChance, DropChance);
	CC_SYNTHESIZE(int, _hp, Hp);

	// we need to create various types of blocks (color and life)
	// createPad and createBody in one method? (needs to include GameScene param??)
	
	static Block* addNewBlockAtPosition(CCPoint p, cpSpace* space, int type);					

	~Block();

	// callback called when ball collides with it
	void takeDamage();

};

#endif // __ARKA_BLOCK_H__
