#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "Pad.h"
#include "Ball.h"
#include "Block.h"
#include "Booster.h"
#include "Bullet.h"
#include "BlockGeneratorFactory.h"
#include "CollisionManager.h"
#include "GameOverScene.h"
#include "SimpleAudioEngine.h"
// #include "CCPhysicsDebugNode.h"

// #ifndef CC_ENABLE_CHIPMUNK_INTEGRATION
// #define CC_ENABLE_CHIPMUNK_INTEGRATION 1

#define BALL_TYPE 1;
#define PAD_TYPE 2;

using namespace cocos2d;
USING_NS_CC_EXT;

class GameScene : public CCLayer
{

private:
	// window size and coordinate origin (not used really as we make use of VisibleRect class)
	CCSize _visibleSize; 
	CCPoint _origin;
	// the pad, ball and blocks are autorelease objects
	Pad* _pad;
	Ball* _ball;
	Ball* _extraBall;
	// life displaying label
	CCLabelTTF* _lifeLabel;
	CCTexture2D* m_pSpriteTexture; // weak ref
	// #if CC_ENABLE_CHIPMUNK_INTEGRATION    
	//     CCPhysicsDebugNode* m_pDebugLayer; // weak ref
	// #endif
	cpSpace* m_pSpace; // strong ref
	cpShape* m_pWalls[4];

	// game logic checks
	bool validPadPosition(CCPoint pt);		// check whether the pad is inside the window bounds
	void updateLives(int lives);			// update the life counter on various events
	bool ballVisible(Ball* ball);			// check whether a ball is inside game bounds
	int ballsOnScreen();					// check whether there are balls in game
	void setCollisionHandlers();			// set several collision handlers
	void checkPadLifeBoost();				// poll the pad to see if it's got a life boost
	void checkPadDoubleBoost();				// poll the pad to see if it's got an extra ball boost
	bool blocksLeft();

public:
	// number of lives (+ getter & setter: maybe we just use the private field)
	CC_SYNTHESIZE(int, _lives, Lives);

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(int type);  

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static CCScene* scene(int type);

	// a selector callback
	void menuCloseCallback(CCObject* pSender);

	// implement the "static node()" method
	static GameScene* create(int type);

	// touch event handling
	virtual void onEnter();
	virtual void onExit();
	virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
	virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);

	// chipmunk physics init
	void initPhysics();
	void update(float dt);		// scheduled update callback selector in init method after init physics and adding sprite

	~GameScene();
	
};

#endif // __GAME_SCENE_H__
