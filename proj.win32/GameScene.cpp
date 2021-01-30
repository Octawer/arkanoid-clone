#include "GameScene.h"

USING_NS_CC;


/* Collision Handling methods */

// post physics step remove callback handler
// it deletes the game objects susceptible to be removed due to collisions (blocks, bullets, etc)
static void postStepRemove(cpSpace *space, cpShape *shape, void *unused)
{
	if(shape->collision_type == CollisionManager::BlockCollision) {
		Block* block = (Block*)shape->data;
		block->takeDamage();
	} else if (shape->collision_type == CollisionManager::BoosterCollision) {
		Booster* booster = (Booster*)shape->data;
		booster->banishBooster();
	}  else if (shape->collision_type == CollisionManager::BulletCollision) {
		Bullet* bullet = (Bullet*)shape->data;
		bullet->destroyBullet();
	}
}

// boost the pad when it receive a booster (post physics cycle)
static void postStepBoostPad(cpSpace *space, cpShape *shape, void *unused)
{
	Pad* pad = (Pad*)shape->data;
	Booster* powerUp = (Booster*)unused;
	pad->boost(powerUp->getType());
}

// handle ball collisions with various shapes
static int ballBegin(cpArbiter *arb, cpSpace *space, void *unused)
{
	// Get the cpShapes involved in the collision
	// The order will be the same as you defined in the handler definition
	// we use the CP_ARBITER_GET_SHAPES() macro
	// It defines and sets the variables for you.
	CP_ARBITER_GET_SHAPES(arb, a, b);

	// if(a->data instanceof Pad) {} (how to check this?)
	Ball* myBall = (Ball*)a->data;

	// encapsulates the calculations of the new direction after collision in the Ball
	myBall->bounce(arb,b);			

	// if b is a block call a function to handle damage and destruction of blocks (in Block) - cpSpaceAddPostStepCallback
	if(b->collision_type == CollisionManager::BlockCollision)
	{
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, b, NULL);
	}

	// The object is dead, don’t process the collision further
	return 0;
}

// handle booster collisions with pad or bottom wall segment (else ignore)
static int boosterBegin(cpArbiter *arb, cpSpace *space, void *unused)
{
	// Get the cpShapes involved in the collision
	// The order will be the same as you defined in the handler definition
	// we use the CP_ARBITER_GET_SHAPES() macro
	// It defines and sets the variables for you.
	CP_ARBITER_GET_SHAPES(arb, a, b);

	// defined as (booster, pad) - so booster is a
	Booster* powerUp = (Booster*)a->data;

	// if booster collides with the pad we boost the pad and destroy the booster
	if(b->collision_type == CollisionManager::PadCollision)
	{
		// we pass the actual booster as user data pointer
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepBoostPad, b, powerUp);
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, a, NULL);
		// destroy on bottom wall collision
	} else if (b->collision_type == CollisionManager::WallCollision) {
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, a, NULL);
		// nothing else happens 
	} else {
		// ignore
	}

	// The object is dead, don’t process the collision further
	return 0;
}

// handle bullet collisions with blocks
static int bulletBegin(cpArbiter *arb, cpSpace *space, void *unused)
{
	// Get the cpShapes involved in the collision
	// The order will be the same as you defined in the handler definition
	// we use the CP_ARBITER_GET_SHAPES() macro
	// It defines and sets the variables for you.
	CP_ARBITER_GET_SHAPES(arb, a, b);

	// defined as (bullet, other) - so bullet is a
	Bullet* bullet = (Bullet*)a->data;

	// if bullet collides with a block we destroy bullet and damage the block
	if(b->collision_type == CollisionManager::BlockCollision)
	{
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, a, NULL);
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, b, NULL);
		// destroy on top wall collision
	} else if (b->collision_type == CollisionManager::WallCollision) {
		cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemove, a, NULL);
		// nothing else happens 
	} else {
		// ignore
	}

	// The object is dead, don’t process the collision further
	return 0;
}

/* End of Collision Section */












/* Actual GameScene method implementation */


CCScene* GameScene::scene(int type)
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	GameScene *layer = GameScene::create(type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

GameScene* GameScene::create(int type)
{
    GameScene *gScene = new GameScene(); 
    if (gScene && gScene->init(type)) 
    { 
        gScene->autorelease(); 
        return gScene; 
    } 
	CC_SAFE_DELETE(gScene);  
	return NULL; 

}

// on "init" you need to initialize your instance
bool GameScene::init(int type)
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION
	//////////////////////////////
	// super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	_visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	_origin = CCDirector::sharedDirector()->getVisibleOrigin();

	this->setTouchEnabled(true);

	/////////////////////////////
	// add a menu item with "X" image, which is clicked to quit the program

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"buttons/CloseNormal.png",
		"buttons/CloseSelected.png",
		this,
		menu_selector(GameScene::menuCloseCallback));

	pCloseItem->setPosition(ccp(_origin.x + _visibleSize.width - pCloseItem->getContentSize().width/2 ,
		_origin.y + pCloseItem->getContentSize().height/2));

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);


	// init chipmunk physics
	initPhysics();

	/////////////////////////////
	// init background and labels

	// _numberOfBalls = 0;
	_lives = 5;
	// create lives label display
	char liveStr[20];
	sprintf(liveStr, "Lives: %d", _lives);
	std::string s = liveStr;

	_lifeLabel = CCLabelTTF::create(s.c_str(), "Arial", 24);
	_lifeLabel->setPosition(ccp(VisibleRect::bottom().x, VisibleRect::bottom().y + 10));
	this->addChild(_lifeLabel, 1);


	// add background image
	CCSprite* background = CCSprite::create("backgrounds/space.png");
	background->setPosition(VisibleRect::center());
	this->addChild(background, 0);


	/////////////////////////////
	// init game entities

	// we set the user data of the chipmunk space to be the GameScene
	cpSpaceSetUserData(m_pSpace, this);

	// add the paddle
	_pad = Pad::addNewPadAtPosition(ccp(_origin.x + _visibleSize.width/2,_origin.y + _visibleSize.height/7), m_pSpace);
	this->addChild(_pad);
	// init the ever bouncing ball
	_ball = Ball::addNewBallAtPosition(VisibleRect::center(), m_pSpace);
	this->addChild(_ball);
	// set the extra ball pointer to null (for safety checks later)
	_extraBall = NULL;

	// generate blocks with a given strategy (passed by type param) or Random by default if bad param
	if(type == 0 || type == 1 || type == 2) {
		BlockGeneratorFactory::generateBlocks(m_pSpace, this, type);
	} else {
		BlockGeneratorFactory::generateBlocks(m_pSpace, this, BlockGeneratorFactory::Random);
	}

	// init bgm
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("music/2ndBallad.mp3", true);
	// set collision handlers
	setCollisionHandlers();
	// schedule update callback
	scheduleUpdate();

	return true;
#else
	CCLabelTTF *pLabel = CCLabelTTF::create("Should define CC_ENABLE_CHIPMUNK_INTEGRATION=1\n to run this test case",
		"Arial",
		18);
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	pLabel->setPosition(ccp(size.width/2, size.height/2));

	addChild(pLabel);

#endif
}


void GameScene::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}

void GameScene::onEnter(){
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
	CCLayer::onEnter();
	CCLOG("GameScene entered scene");

}

void GameScene::onExit(){
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	CCLayer::onExit();
}

// we capture the user clicks inside the pad to shoot when enabled
bool GameScene::ccTouchBegan(CCTouch* touch, CCEvent* event){
	CCPoint touchPoint = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
	// CCPoint touchPoint = touch->getLocationInView();
	CCLOG("Touched point %.2f %.2f", touchPoint.x, touchPoint.y);
	// if _pad exists and we touched it (inside bounds)
	CCRect spriteRect = _pad->boundingBox();
	if(_pad && spriteRect.containsPoint(touchPoint)) 
	{
		// shoot bullets upwards
		_pad->shoot();
	}
	return true;
}

// we capture the user mouse drag (or touch drag) gesture to move the pad horizontally
void GameScene::ccTouchMoved(CCTouch* touch, CCEvent* event){
	CCPoint touchPoint = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
	// if _pad exists and we touched it (inside bounds)
	CCRect spriteRect = _pad->boundingBox();
	if(_pad && spriteRect.containsPoint(touchPoint)) 
	{
		// we get next position and check if its a valid location (inside walls)
		// if so we update the pad position
		CCPoint nextPos = ccp(touch->getLocation().x,_pad->getPosition().y);
		if(validPadPosition(nextPos)) {
			_pad->setPosition(nextPos);
		}
	}
}

void GameScene::ccTouchEnded(CCTouch* touch, CCEvent* event){
	// not used atm
}

bool GameScene::validPadPosition(CCPoint pt)
{
	// see if the next position is valid by checking if next pad bounds
	// are inside the space x coords
	if(pt.x - (_pad->getWidth() / 2) <= _origin.x || pt.x + (_pad->getWidth() / 2) >= _visibleSize.width) {
		return false;
	} else {
		return true;
	}
}

// init chipmunk physics
void GameScene::initPhysics()
{
#if CC_ENABLE_CHIPMUNK_INTEGRATION    
	// init chipmunk
	// cpInitChipmunk();

	m_pSpace = cpSpaceNew();

	// we set 0 gravity scenario, as our game doesnt depend on it
	m_pSpace->gravity = cpv(0, 0);		
	CCLOG("Gravity added");

	//
	// rogue shapes: wall segments that comprises the game visible bounds
	// We have to free them manually
	//
	// bottom
	m_pWalls[0] = cpSegmentShapeNew( m_pSpace->staticBody,
		cpv(VisibleRect::leftBottom().x,VisibleRect::leftBottom().y),
		cpv(VisibleRect::rightBottom().x, VisibleRect::rightBottom().y), 0.0f);			// A ---> B

	// top
	m_pWalls[1] = cpSegmentShapeNew( m_pSpace->staticBody,								// A ---> B
		cpv(VisibleRect::leftTop().x, VisibleRect::leftTop().y),
		cpv(VisibleRect::rightTop().x, VisibleRect::rightTop().y), 0.0f);

	// left
	m_pWalls[2] = cpSegmentShapeNew( m_pSpace->staticBody,								// B
		cpv(VisibleRect::leftBottom().x,VisibleRect::leftBottom().y),					// |
		cpv(VisibleRect::leftTop().x,VisibleRect::leftTop().y), 0.0f);					// A

	// right
	m_pWalls[3] = cpSegmentShapeNew( m_pSpace->staticBody,								//		B
		cpv(VisibleRect::rightBottom().x, VisibleRect::rightBottom().y),				//		|
		cpv(VisibleRect::rightTop().x, VisibleRect::rightTop().y), 0.0f);				//		A

	// we set its collision type as WallCollision and set its shape parameters (elasticity and friction)
	int collisionType = CollisionManager::WallCollision;
	for( int i=0;i<4;i++) {
		m_pWalls[i]->e = 1.0f;
		m_pWalls[i]->u = 1.0f;
		cpShapeSetCollisionType(m_pWalls[i],collisionType);
		cpSpaceAddStaticShape(m_pSpace, m_pWalls[i] );
		//collisionType++;
	}

	// Physics debug layer
	// m_pDebugLayer = CCPhysicsDebugNode::create(m_pSpace);
	// this->addChild(m_pDebugLayer, Z_PHYSICS_DEBUG);
#endif
}

// update callback, called every frame
void GameScene::update(float delta)
{
	// Should use a fixed size step based on the animation interval.
	int steps = 2;
	float dt = CCDirector::sharedDirector()->getAnimationInterval()/(float)steps;

	for(int i=0; i<steps; i++){
		cpSpaceStep(m_pSpace, dt);
	}

	// poll the pad flags to see if we need an extra ball or life (boosters)
	if(_pad) {
		checkPadLifeBoost();
		checkPadDoubleBoost();
	}

	// _ball and _extraBall logic flaw when there is only _extra left and we get another booster ( 2 _extras ?? )
	// maybe an array of 2 Balls is more robust (watch out: autorelease; adding the ball to the array may cause deletion problems (another reference for ball))

	// if any ball goes off screen destroy it
	if(_ball) {
		if(!ballVisible(_ball)) {
			_ball->exileBall(m_pSpace);
			_ball = NULL;
		}
	}

	// extra ball not init by default (needs a booster to exist)
	if(_extraBall) {
		if(!ballVisible(_extraBall)) {
			_extraBall->exileBall(m_pSpace);
			_extraBall = NULL;
		}
	}


	// check if there are no balls on screen and updateLives(-1) if so
	// change to lose life scene and restore this GameScene with 1 ball on center
	if(ballsOnScreen() == 0) {
		// decrease the lives by 1
		updateLives(_lives - 1);
		// maybe pause game for some secs ...
		// reset ball
		_ball = Ball::addNewBallAtPosition(VisibleRect::center(), m_pSpace);
		this->addChild(_ball);
	}

	// if we have no more lives -> change scene (director) to game over (lose)
	if(_lives <= 0) {
		// change scene (director) to game over
		// create a scene. it's an autorelease object
		CCScene *game_over_scene = GameOverScene::createGameOverScene(GameOverScene::Lose);
		// replace actual scene
		CCDirector::sharedDirector()->replaceScene(game_over_scene);
	}

	// if there are no blocks on scene -> change scene (director) to game over (win)
	if(!blocksLeft()) {
		// change scene (director) to game over
		// create a scene. it's an autorelease object
		CCScene *game_over_scene = GameOverScene::createGameOverScene(GameOverScene::Win);
		// replace actual scene
		CCDirector::sharedDirector()->replaceScene(game_over_scene);
	}
}

GameScene::~GameScene()
{
	// manually Free rogue shapes
	for( int i=0;i<4;i++) {
		cpShapeFree( m_pWalls[i] );
	}

	// delete physics chipmunk space
	cpSpaceFree( m_pSpace );

}

// update lives and life label
void GameScene::updateLives(int lives)
{
	_lives = lives;
	char liveStr[20];
	sprintf(liveStr, "Lives: %d", _lives);
	std::string s = liveStr;

	_lifeLabel->setString(s.c_str());
}

// init ball pointers to NULL always for safety here (consider deletion list approach)
bool GameScene::ballVisible(Ball* ball)
{
	// we check the four walls due to some unknown bug that makes the ball go offscreen, other than from bottom wall, in rare occasions
	if(ball && (ball->getPositionY() >= VisibleRect::bottom().y && ball->getPositionY() <= VisibleRect::top().y 
		&& ball->getPositionX() >= VisibleRect::left().x && ball->getPositionY() <= VisibleRect::right().x)){
			return true;
	} else {
		return false;
	}
}

int GameScene::ballsOnScreen()
{
	int count = 0;
	// loop through every Ball object and return false if there is none (cpu cost)
	// maybe if we just have 2 references we may only check those 2 (update fc - reduce cpu overhead)
	
	// we count up for each visible ball
	if(_ball) {
		if(ballVisible(_ball)) {
			count++;
		}
	}

	if(_extraBall) {
		if(ballVisible(_extraBall)) {
			count++;
		}
	}

	// return the visible ball counter
	return count;
}

// used in the game loop poll to see if there is a pad life boost available
void GameScene::checkPadLifeBoost()
{
	// if pad has this flag on we add 1 life and turn it off
	if(_pad->getLifeBoost() == true) {
		updateLives(_lives + 1);
		// we disable life boost once we used it up
		_pad->setLifeBoost(false);
	}
}

// test if a shape's user data is a block
void blockTest(cpShape *shape, bool *blocksLeft){
	Block* b = (Block*)shape->data;
	if(b && b->getTag() == 5) *blocksLeft = true;
}

// counts the number of remaining blocks
bool GameScene::blocksLeft()
{
	// iterates over all shapes testing if they are blocks
	// to check if there is any left
	bool thereAreBlocks = false;
	cpSpaceEachShape(m_pSpace, (cpSpaceShapeIteratorFunc)blockTest, &thereAreBlocks);
	return thereAreBlocks;
}

// used in the game loop poll to see if there is a pad ball boost available
void GameScene::checkPadDoubleBoost()
{
	// if pad has this flag on we add 1 extra ball to space and scene, and turn flag off
	if(_pad->getDoubleBoost() == true) {
		// add extra ball if we have no one yet
		if(!ballVisible(_extraBall)) {
			// create the actual extra ball
			_extraBall = Ball::addNewBallAtPosition(VisibleRect::center(), m_pSpace);
			_extraBall->runAction(CCTintTo::create(0.1f,255,0,0));				// we tint the extra ball for visual tagging
			this->addChild(_extraBall);
		}
		// we disable extra ball boost once we used it
		_pad->setDoubleBoost(false);
	}
}


// establish the various collision handlers (set of functions for every colliding pair or shapes)
void GameScene::setCollisionHandlers()
{
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BallCollision, CollisionManager::PadCollision, ballBegin, NULL, NULL, NULL, NULL);		// ball vs pad
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BallCollision, CollisionManager::WallCollision, ballBegin, NULL, NULL, NULL, NULL);		// ball vs wall
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BallCollision, CollisionManager::BlockCollision, ballBegin, NULL, NULL, NULL, NULL);		// ball vs block
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BallCollision, CollisionManager::BoosterCollision, ballBegin, NULL, NULL, NULL, NULL);	// ball vs booster
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BallCollision, CollisionManager::BulletCollision, ballBegin, NULL, NULL, NULL, NULL);	// ball vs booster

	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BoosterCollision, CollisionManager::BallCollision, boosterBegin, NULL, NULL, NULL, NULL);		// booster vs ball
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BoosterCollision, CollisionManager::PadCollision, boosterBegin, NULL, NULL, NULL, NULL);			// booster vs pad
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BoosterCollision, CollisionManager::WallCollision, boosterBegin, NULL, NULL, NULL, NULL);		// booster vs wall
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BoosterCollision, CollisionManager::BlockCollision, boosterBegin, NULL, NULL, NULL, NULL);		// booster vs block
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BoosterCollision, CollisionManager::BulletCollision, boosterBegin, NULL, NULL, NULL, NULL);		// booster vs block

	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BulletCollision, CollisionManager::BallCollision, bulletBegin, NULL, NULL, NULL, NULL);			// bullet vs ball
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BulletCollision, CollisionManager::PadCollision, bulletBegin, NULL, NULL, NULL, NULL);			// bullet vs pad
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BulletCollision, CollisionManager::WallCollision, bulletBegin, NULL, NULL, NULL, NULL);			// bullet vs wall
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BulletCollision, CollisionManager::BlockCollision, bulletBegin, NULL, NULL, NULL, NULL);			// bullet vs block
	cpSpaceAddCollisionHandler(m_pSpace, CollisionManager::BulletCollision, CollisionManager::BoosterCollision, bulletBegin, NULL, NULL, NULL, NULL);		// bullet vs block

}
