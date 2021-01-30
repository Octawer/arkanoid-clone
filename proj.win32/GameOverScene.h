#ifndef __GAME_OVER_SCENE_H__
#define __GAME_OVER_SCENE_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"
#include "VisibleRect.h"
#include "SimpleAudioEngine.h"

//
USING_NS_CC; 
USING_NS_CC_EXT;

class GameOverScene : public CCLayer
{

private:

public:
	// types of scenes
	enum{ Win, Lose };
	// custom initialization (win or lose depending on GameScene progress)
	bool init(int type);
	static GameOverScene* create(int type);
	static CCScene* createGameOverScene(int type);
	// a selector callback
	void menuCloseCallback(CCObject* pSender);
};

#endif // __GAME_OVER_SCENE_H__