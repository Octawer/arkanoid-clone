#include "Bullet.h"

// it's also an autorelease object
Bullet* Bullet::createBullet(const char * spriteFileName, float x, float y, int type) 
{
	Bullet *bullet = new Bullet();  
	if (bullet && bullet->initWithFile(spriteFileName))  
	{  
		bullet->autorelease();  
		return bullet;  
	}  
	CC_SAFE_DELETE(bullet);  
	return NULL; 
}

Bullet* Bullet::addNewBulletAtPosition(CCPoint pos, cpSpace* space, int type)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION   

	Bullet* bullet = Bullet::createBullet("powerups/shot.png",pos.x,pos.y, type);
    int num = 4;

	// we set the body and shape vertexes
	// according to imagen pixels (points) (mid,mid) : eg 129x33 size o pad
	cpVect verts[] = {

		cpv(-(bullet->getTextureRect().size.width/2),-(bullet->getTextureRect().size.height/2)),

		cpv(-(bullet->getTextureRect().size.width/2), (bullet->getTextureRect().size.height/2)),

		cpv( (bullet->getTextureRect().size.width/2), (bullet->getTextureRect().size.height/2)),

		cpv( (bullet->getTextureRect().size.width/2),-(bullet->getTextureRect().size.height/2)),
	};

	// we create a chipmunk body and shape and assign it to out sprite
	// similarly to the booster, here it doesnt matter much the mass and moment of inertia (it'll be destroyed on colliding with block)
	cpBody *body = cpBodyNew(1.0f, cpMomentForPoly(INFINITY, num, verts, cpvzero));

    body->p = cpv(pos.x, pos.y);
    cpSpaceAddBody(space, body);

	bullet->_shape = cpPolyShapeNew(body, num, verts, cpvzero);
    bullet->_shape->e = 1.0f; 
	bullet->_shape->u = 0.5f;

	cpShapeSetCollisionType(bullet->_shape,CollisionManager::BulletCollision);
	cpShapeSetUserData(bullet->_shape,bullet);					
    cpSpaceAddShape(space, bullet->_shape);

    bullet->setCPBody(body);
    bullet->setPosition(pos);
	bullet->setTag(2);

	bullet->_space = space;

	// impulse upwards on creation (when the user shoots with the pad)
	cpBodyApplyImpulse(bullet->getCPBody(), cpv(0,500), cpv(0,0));

	return bullet;
#endif
}

// callback to remove Shapes from the Body
void removeBulletShape( cpBody *body, cpShape *shape, void *data )
{
	cpShapeFree( shape );
}

Bullet::~Bullet()
{
	// dealloc the body's shapes
	cpBodyEachShape(getCPBody(), removeBulletShape, NULL);
	// dealloc the body
	cpBodyFree( getCPBody() );
}

void Bullet::destroyBullet()
{
		cpSpaceRemoveBody(_space, _shape->body);
		// cpBodyFree(shape->body);

		cpSpaceRemoveShape(_space, _shape);
		// cpShapeFree(shape);
		this->removeFromParentAndCleanup(true);
}