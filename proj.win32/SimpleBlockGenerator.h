#ifndef __SIMPLE_BLOCK_GENERATOR_H__
#define __SIMPLE_BLOCK_GENERATOR_H__

#include "IBlockGenerator.h"

using namespace cocos2d;
USING_NS_CC_EXT;

// Simple block placement algorithm class

class SimpleBlockGenerator : public IBlockGenerator
{

private:
	
public:
	
	SimpleBlockGenerator() : IBlockGenerator() 
	{
		// we call the base class constructor (to get the paddings) and initialize some custom parameters
		_rows = 3;
		_cols = VisibleRect::rightTop().x / _paddingW;
		// _level = ??;
	}

	virtual void generateBlocks(cpSpace* space, CCLayer* scene);

};

#endif /* __SIMPLE_BLOCK_GENERATOR_H__ */