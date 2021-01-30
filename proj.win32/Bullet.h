#ifndef __ARKA_BULLET_H__
#define __ARKA_BULLET_H__

#include "cocos2d.h"
#include "chipmunk.h" 
#include "cocos-ext.h"	
#include "CollisionManager.h"

using namespace cocos2d;
USING_NS_CC_EXT;

//
class Bullet : public CCPhysicsSprite
{

private:
	cpShape* _shape;
	cpSpace* _space;
	static Bullet* createBullet(const char * spriteFileName, float x, float y, int type);

public:
	
	static Bullet* addNewBulletAtPosition(CCPoint p, cpSpace* space, int type);					

	// delete the bullet and its physics components, and remove them from space and gameScene
	void destroyBullet();
	~Bullet();
	
};

#endif // __ARKA_BULLET_H__