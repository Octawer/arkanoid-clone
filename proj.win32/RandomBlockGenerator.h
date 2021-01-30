#ifndef __RANDOM_BLOCK_GENERATOR_H__
#define __RANDOM_BLOCK_GENERATOR_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"	
#include "IBlockGenerator.h"
#include "VisibleRect.h"

//
using namespace cocos2d;
USING_NS_CC_EXT;

// Random block placement algorithm class

class RandomBlockGenerator : public IBlockGenerator
{

private:

public:

	// we call the base class constructor (to get the paddings) and initialize some custom parameters
	RandomBlockGenerator() : IBlockGenerator() 
	{
		// init rand seed
		srand (time(NULL));
		_rows = rand() % 7 + 1;	// [1-7]
		// we set the cols as the window width / block width
		_cols = VisibleRect::rightTop().x / _paddingW;
		// _level = ??;
	}


	virtual void generateBlocks(cpSpace* space, CCLayer* scene);

};

#endif /* __RANDOM_BLOCK_GENERATOR_H__ */