#include "Ball.h"

// the ball is an autorelease object (auto. released when it is attached to nothing; 0 references)
Ball* Ball::createBall(const char * spriteFileName, float x, float y) 
{
	Ball *ball = new Ball();  
	if (ball && ball->initWithFile(spriteFileName))  
	{  
		ball->init(x,y);
		ball->autorelease();  
		return ball;  
	}  
	CC_SAFE_DELETE(ball);  
	return NULL; 
}

bool Ball::init(float x, float y) 
{
	_x = x;  
	_y = y;
	_speed = 300;
	_width = getTextureRect().size.width;  
	_height = getTextureRect().size.height;
	// CCLOG("Ball initial position (%.2f,%.2f)", _x,_y);
	return true;
}

// callback to remove Shapes from the Space
void removeBallShape( cpBody *body, cpShape *shape, void *data )
{
	cpShapeFree( shape );
}

Ball::~Ball()
{
	// dealloc the body's shapes
	cpBodyEachShape(getCPBody(), removeBallShape, NULL);
	// dealloc the body
	cpBodyFree( getCPBody() );
}

Ball* Ball::addNewBallAtPosition(CCPoint pos, cpSpace* space)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION   

	// create custom sprites
	Ball* _ball = Ball::createBall("balls/ball.png",pos.x,pos.y);

	// set the shape and body vertexes according to the sprite texture dimensions (enclosing rectangle)
	int num = 4;
	cpVect verts[] = {

		cpv(-(_ball->_width/2),-(_ball->_height/2)),

		cpv(-(_ball->_width/2), (_ball->_height/2)),

		cpv( (_ball->_width/2), (_ball->_height/2)),

		cpv( (_ball->_width/2),-(_ball->_height/2)),
	};

	// we create a chipmunk body and shape and assign it to our sprite
	// make the moment of inertia so as to prevent the ball from spinning (INFINITY)
	// and the mass low compared to other bodies so as to move only the ball
	cpBody *body = cpBodyNew(1.0f, cpMomentForPoly(INFINITY, num, verts, cpvzero));

	body->p = cpv(pos.x, pos.y);
	cpSpaceAddBody(space, body);

	// set the shape, elasticity (e) and friction coefficient (u)
	_ball->_shape = cpPolyShapeNew(body, num, verts, cpvzero);
	_ball->_shape->e = 1.0f; 
	_ball->_shape->u = 0.5f;
	// set the collision type, user data (the actual ball object) and add to physical chipmunk space
	cpShapeSetCollisionType(_ball->_shape,CollisionManager::BallCollision);
	cpShapeSetUserData(_ball->_shape,_ball);					
	cpSpaceAddShape(space, _ball->_shape);

	// set body and initial position
	_ball->setCPBody(body);
	_ball->setPosition(pos);
	_ball->setTag(4);
	// apply initial impulse (random in x, upwards in y)
	float randX = (((rand() % 20) + 1) - 10) / 10.0f;		// random in range [-0.9, 1]
	// CCLOG("rand X == %.2f", randX);
	cpBodyApplyImpulse(_ball->getCPBody(), cpv(randX*_ball->getSpeed(),1*_ball->getSpeed()), cpv(0,0));

	return _ball;
#endif
}


void Ball::bounce(cpArbiter *arb, cpShape* b)
{
	// play hit sound effect
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("music/arkanoid_hit.wav");
	int collType = cpShapeGetCollisionType(b); 
	switch(collType) {
	case CollisionManager::PadCollision:		// collided with pad
		bounceWithPad(arb,b);
		break;
	case CollisionManager::WallCollision:		// collided with bottom wall
		bounceWithWall(b);
		break;
	case CollisionManager::BlockCollision:		// collided with block - we make it same as Pad (the only difference is the shape data type) - inheritance?
		bounceWithBlock(arb,b);
		break;
	case CollisionManager::BoosterCollision:		// collided with block - we make it same as Pad (the only difference is the shape data type) - inheritance?
		// ignore collision
		break;
	default:
		// also ignore - show a message for debug
		CCLOG("collision type of b shape not defined");
	}
}

void Ball::bounceWithPad(cpArbiter *arb, cpShape* b)
{
	// get the collision point
	cpVect collisionPt = cpArbiterGetPoint(arb,1); // 1??
	CCLOG("Collision point = (%.2f , %.2f) ",collisionPt.x, collisionPt.y);

	// get the initial velocity and direction vectors (direction = velocity normalized)
	float imp_x = this->getCPBody()->v.x;
	float imp_y = this->getCPBody()->v.y;
	CCLOG("Ball hit Pad!: ball velocity vector = (%.2f , %.2f) ",imp_x, imp_y);
	CCPoint direction_v = ccpNormalize(ccp(imp_x, imp_y));
	CCLOG("Ball hit Pad!: ball direction vector = (%.2f , %.2f) ",direction_v.x, direction_v.y);


	// get the outgoing velocity vector
	// we get the x component as follows: (impact point - pad center) / half pad width
	// and the y component just inverting the incoming y velocity vector
	// this way the outgoing vector has an angle that depends on the proximity to the center of the pad when colliding with it
	Pad* myPad = (Pad*)b->data;
	direction_v.x = (collisionPt.x - b->body->p.x) / (myPad->getWidth() / 2);
	CCLOG("Pad mid point = %.2f", b->body->p.x);
	CCLOG("Pad width = %.2f", myPad->getWidth());
	// we set the body velocity to 0 before updating the actual outgoing velocity (for setting absolute impluse next - not adding with prev)
	cpBodySetVel(this->getCPBody(),cpv(0,0));											

	// we apply an impulse with that outgoing velocity vector
	cpBodyApplyImpulse(this->getCPBody(), cpv(direction_v.x*_speed,-direction_v.y*_speed), cpv(0,0));	
	CCLOG("Ball Bounced!: Ball outgoing impulse vector = (%.2f , %.2f) ",direction_v.x*_speed, -direction_v.y*_speed);
}


void Ball::bounceWithWall(cpShape* shape) 
{
	// declare outgoing velocity vector
	cpVect outgoing_vel;
	// get the incoming velocity and direction vector
	float imp_x = this->getCPBody()->v.x;
	float imp_y = this->getCPBody()->v.y;
	CCLOG("Ball hit Wall!: ball velocity vector = (%.2f , %.2f) ",imp_x, imp_y);
	CCPoint direction_v = ccpNormalize(ccp(imp_x, imp_y));
	CCLOG("Ball hit Wall!: ball direction vector = (%.2f , %.2f) ",direction_v.x, direction_v.y);

	// we get the middle point of the segment to distinguish between walls (better approach than normal, more robust)
	cpVect a = cpSegmentShapeGetA(shape);
	cpVect b = cpSegmentShapeGetB(shape);
	cpVect midPt = cpv((a.x+b.x)/2,(a.y+b.y)/2);
	CCLOG("Wall mid Point vector = (%.2f , %.2f) ",midPt.x, midPt.y);

	// check wall type
	// and change direction to left/right normal vector
	// according to initial velocity (direction) vector (+/- X , +/- Y) 

	// we set the outgoing vector as 90º from input (its normal vector)
	// choosing left or right vector from initial incidence vector
	if(midPt.x == VisibleRect::bottom().x && midPt.y == VisibleRect::bottom().y){											// bottom wall
		// we just let the ball continue in the same direction (it is destroyed in GameScene)
		outgoing_vel = cpv(direction_v.x *_speed,direction_v.y * _speed);
	} else if (midPt.x == VisibleRect::top().x && midPt.y == VisibleRect::top().y) {										// top wall
		// if x direction is negative then left normal outgoing vector (-y,x)
		if(direction_v.x < 0) {
			outgoing_vel = cpv(-direction_v.y *_speed ,direction_v.x * _speed);
			// if it goes just upwards (x direction == 0) it bounces back
		} else if (direction_v.x == 0) {
			outgoing_vel = cpv(direction_v.x *_speed,-direction_v.y * _speed);
			// else right normal outgoing vector (y, -x)
		} else {
			outgoing_vel = cpv(direction_v.y *_speed , -direction_v.x * _speed);
		}
	} else if (midPt.x == VisibleRect::left().x && midPt.y == VisibleRect::left().y) {										// left wall
		// if y direction is negative then left normal outgoing vector (-y,x)
		if(direction_v.y < 0) {
			outgoing_vel = cpv(-direction_v.y *_speed ,direction_v.x * _speed);
			// if it goes just left (y direction == 0) it bounces back at a given angle
		} else if (direction_v.y == 0) {
			outgoing_vel = cpv(1 *_speed,-1 * _speed);
			// else right normal outgoing vector (y, -x)
		} else {
			outgoing_vel = cpv(direction_v.y *_speed , -direction_v.x * _speed);
		}
	} else if (midPt.x == VisibleRect::right().x && midPt.y == VisibleRect::right().y){										// right wall
		// if y direction is positive then left normal outgoing vector (-y,x)
		if(direction_v.y > 0) {
			outgoing_vel = cpv(-direction_v.y *_speed ,direction_v.x * _speed);
			// if it goes just right (y direction == 0) it bounces back at a given angle
		} else if (direction_v.y == 0) {
			outgoing_vel = cpv(-1 *_speed,-1 * _speed);
			// else right normal outgoing vector (y, -x)
		} else {
			outgoing_vel = cpv(direction_v.y *_speed , -direction_v.x * _speed);
		}
	} else {
		CCLOG("wall type not defined");
	}
	
	// we reset the ball velocity and apply then the actual outgoing impulse
	cpBodySetVel(this->getCPBody(),cpv(0,0));
	cpBodyApplyImpulse(this->getCPBody(), outgoing_vel, cpv(0,0));
	CCLOG("Ball bounced off Wall!: ball velocity vector = (%.2f , %.2f) ",outgoing_vel.x, outgoing_vel.y);
}


void Ball::bounceWithBlock(cpArbiter *arb, cpShape* b)
{
	// we get collision point
	cpVect collisionPt = cpArbiterGetPoint(arb,1); // 1??
	CCLOG("Collision point = (%.2f , %.2f) ",collisionPt.x, collisionPt.y);

	// we get incoming velocity and direction
	float imp_x = this->getCPBody()->v.x;
	float imp_y = this->getCPBody()->v.y;
	CCLOG("Ball hit Block!: ball velocity vector = (%.2f , %.2f) ",imp_x, imp_y);
	CCPoint direction_v = ccpNormalize(ccp(imp_x, imp_y));
	CCLOG("Ball hit Block!: ball direction vector = (%.2f , %.2f) ",direction_v.x, direction_v.y);

	// get the outgoing velocity vector (same as with pad)
	// we get the x component as follows: (impact point - block center) / half block width
	// and the y component just inverting the incoming y velocity vector
	// this way the outgoing vector has an angle that depends on the proximity to the center of the block when colliding with it
	Block* myBlock = (Block*)b->data;
	direction_v.x = (collisionPt.x - b->body->p.x) / (myBlock->getWidth() / 2);	
	CCLOG("Block mid point = %.2f", b->body->p.x);
	CCLOG("Block width = %.2f", myBlock->getWidth());

	// so, we set the body velocity to 0 (for setting absolute impluse next - not adding)
	cpBodySetVel(this->getCPBody(),cpv(0,0));					
	// we set the actual outgoing impulse
	cpBodyApplyImpulse(this->getCPBody(), cpv(direction_v.x*_speed,-direction_v.y*_speed), cpv(0,0));		
	CCLOG("Ball Bounced off block!: Ball outgoing impulse vector = (%.2f , %.2f) ",direction_v.x*_speed, -direction_v.y*_speed);
}


// remove physics components (body and shape) from chipmunk space and dealloc body, shape and actual ball
void Ball::exileBall(cpSpace* space)
{
	cpSpaceRemoveBody(space, _shape->body);
	// cpBodyFree(shape->body);

	cpSpaceRemoveShape(space, _shape);
	// cpShapeFree(shape);
	this->removeFromParentAndCleanup(true);
}