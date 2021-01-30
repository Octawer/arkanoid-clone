#include "MenuScene.h"


CCScene* MenuScene::createMenuScene(int type)
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	MenuScene *layer = MenuScene::create(type);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

MenuScene* MenuScene::create(int type)
{
	MenuScene *mScene = new MenuScene(); 
	if (mScene && mScene->init(type)) 
	{ 
		mScene->autorelease(); 
		return mScene; 
	} 
	CC_SAFE_DELETE(mScene);  
	return NULL; 

}


bool MenuScene::init(int type)
{
	if (!CCLayer::init())
	{
		return false;
	}

	// create background image and Game Over label according to win / lose scenario

	CCSprite* background = CCSprite::create("backgrounds/supernova.jpg");
	background->setPosition(VisibleRect::center());
	this->addChild(background, 0);


	CCSprite* arkanoidTitle = CCSprite::create("buttons/arkanoid_logo.png");
	arkanoidTitle->setPosition(ccp(VisibleRect::top().x, 3* VisibleRect::top().y / 4));
	this->addChild(arkanoidTitle, 1);

	// we add different menus depending on type param 
	if(type == MainMenu) {
		// add a "close" icon to exit. it's an autorelease object
		CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
			"buttons/quit_button.png",
			"buttons/quit_button_selected.png",
			this,
			menu_selector(MenuScene::menuCloseCallback));

		pCloseItem->setPosition(ccp(VisibleRect::center().x, 1* VisibleRect::center().y / 8));

		// add a "select" icon to select placement strategy. it's an autorelease object
		CCMenuItemImage *pSelectItem = CCMenuItemImage::create(
			"buttons/select_button.png",
			"buttons/select_button_selected.png",
			this,
			menu_selector(MenuScene::menuSelectCallback));

		pSelectItem->setPosition(ccp(VisibleRect::center().x, 2* VisibleRect::center().y / 8));

		// add a "start" icon to start game. it's an autorelease object
		CCMenuItemImage *pStartItem = CCMenuItemImage::create(
			"buttons/play_button.png",
			"buttons/play_button_selected.png",
			this,
			menu_selector(MenuScene::menuStartCallback));

		pStartItem->setPosition(ccp(VisibleRect::center().x, 3* VisibleRect::center().y / 8));
		pStartItem->setTag(BlockGeneratorFactory::Random);

		// create menu, it's an autorelease object
		CCMenu* pMenu = CCMenu::create(pCloseItem, pStartItem, pSelectItem, NULL);
		pMenu->setPosition(CCPointZero);

		this->addChild(pMenu, 2);

	} else if (type == SelectPlacement) {
		// add a simple placement strategy button
		CCMenuItemImage *pSimpleItem = CCMenuItemImage::create(
			"buttons/simple_button.png",
			"buttons/simple_button_selected.png",
			this,
			menu_selector(MenuScene::menuStartCallback));

		pSimpleItem->setPosition(ccp(VisibleRect::center().x, 4* VisibleRect::center().y / 10));
		pSimpleItem->setTag(BlockGeneratorFactory::Simple);

		// add a alternative placement strategy button
		CCMenuItemImage *pAltItem = CCMenuItemImage::create(
			"buttons/alter_button.png",
			"buttons/alter_button_selected.png",
			this,
			menu_selector(MenuScene::menuStartCallback));

		pAltItem->setPosition(ccp(VisibleRect::center().x, 3* VisibleRect::center().y / 10));
		pAltItem->setTag(BlockGeneratorFactory::Alt);

		// add a random placement strategy button
		CCMenuItemImage *pRandItem = CCMenuItemImage::create(
			"buttons/random_button.png",
			"buttons/random_button_selected.png",
			this,
			menu_selector(MenuScene::menuStartCallback));

		pRandItem->setPosition(ccp(VisibleRect::center().x, 2* VisibleRect::center().y / 10));
		pRandItem->setTag(BlockGeneratorFactory::Random);

		// add a "close" icon to exit. it's an autorelease object
		CCMenuItemImage *pQuitItem = CCMenuItemImage::create(
			"buttons/quit_button.png",
			"buttons/quit_button_selected.png",
			this,
			menu_selector(MenuScene::menuCloseCallback));

		pQuitItem->setPosition(ccp(VisibleRect::center().x, 1* VisibleRect::center().y / 10));

		// create menu, it's an autorelease object
		CCMenu* pMenu = CCMenu::create(pSimpleItem, pAltItem, pRandItem,pQuitItem, NULL);
		pMenu->setPosition(CCPointZero);

		this->addChild(pMenu, 2);
	}

	// init menu bgm
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("music/ALightIntro.mp3", true);

	return true;
}

void MenuScene::menuCloseCallback(CCObject* pSender)
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

void MenuScene::menuStartCallback(CCObject* pSender)
{
	CCMenuItem* pMenuItem = (CCMenuItem *)pSender;
    int tag = (int)pMenuItem->getTag();

	// create game scene
	CCScene *game_scene = GameScene::scene(tag);
	// move to game scene
	CCDirector::sharedDirector()->replaceScene(game_scene);
}

void MenuScene::menuSelectCallback(CCObject* pSender)
{
	// create game scene
	CCScene *select_scene = MenuScene::createMenuScene(MenuScene::SelectPlacement);
	// move to game scene
	CCDirector::sharedDirector()->replaceScene(select_scene);
}