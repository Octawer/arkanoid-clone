#include "GameOverScene.h"


CCScene* GameOverScene::createGameOverScene(int type)
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	GameOverScene *layer = GameOverScene::create(type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

GameOverScene* GameOverScene::create(int type)
{
    GameOverScene *overScene = new GameOverScene(); 
    if (overScene && overScene->init(type)) 
    { 
        overScene->autorelease(); 
        return overScene; 
    } 
	CC_SAFE_DELETE(overScene);  
	return NULL; 

}


bool GameOverScene::init(int type)
{
	if (!CCLayer::init())
	{
		return false;
	}

	CCSize _visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint _origin = CCDirector::sharedDirector()->getVisibleOrigin();

	// create background image and Game Over label according to win / lose scenario
	if(type == GameOverScene::Lose) {
		CCSprite* lose_background = CCSprite::create("backgrounds/blackHole.jpg");
		lose_background->setPosition(VisibleRect::center());
		this->addChild(lose_background, 0);

		CCLabelTTF* lose_Label = CCLabelTTF::create("Game Over", "Helvetica", 48);
		lose_Label->setPosition(VisibleRect::center());
		this->addChild(lose_Label, 1);

		// lose music
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("music/EndoLoop.mp3", false);

	} else if (type == GameOverScene::Win) {
		CCSprite* win_background = CCSprite::create("backgrounds/eye_supernova.jpg");
		win_background->setPosition(VisibleRect::center());
		this->addChild(win_background, 0);

		CCLabelTTF* win_Label = CCLabelTTF::create("You Win!!", "Helvetica", 48);
		win_Label->setPosition(VisibleRect::center());
		this->addChild(win_Label, 1);

		// win music
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("music/GameForest.mp3", false);
	}

	
	// add a "close" icon to exit. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
		"buttons/CloseNormal.png",
		"buttons/CloseSelected.png",
		this,
		menu_selector(GameOverScene::menuCloseCallback));

	pCloseItem->setPosition(ccp(_origin.x + _visibleSize.width - pCloseItem->getContentSize().width/2 ,
		_origin.y + pCloseItem->getContentSize().height/2));

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 2);
	return true;

}

void GameOverScene::menuCloseCallback(CCObject* pSender)
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