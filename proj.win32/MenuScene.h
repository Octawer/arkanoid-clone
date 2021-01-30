#ifndef __MY_MENU_SCENE_H__
#define __MY_MENU_SCENE_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "GameScene.h"

//
USING_NS_CC; 
USING_NS_CC_EXT;

class MenuScene : public CCLayer
{

private:

public:
	enum{MainMenu, SelectPlacement};
	bool init(int type);
	static CCScene* createMenuScene(int type);
	// 'create' method macro
	static MenuScene* create(int type);

	// menu selector callbacks
	void menuCloseCallback(CCObject* pSender);
	void menuStartCallback(CCObject* pSender);
	void menuSelectCallback(CCObject* pSender);
};

#endif // __MY_MENU_SCENE_H__