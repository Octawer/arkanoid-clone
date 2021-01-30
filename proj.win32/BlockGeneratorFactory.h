#ifndef __BLOCK_GENERATOR_FACTORY_H__
#define __BLOCK_GENERATOR_FACTORY_H__

#include "cocos2d.h"
#include "chipmunk.h"
#include "cocos-ext.h"	
#include "IBlockGenerator.h"
#include "SimpleBlockGenerator.h"
#include "RandomBlockGenerator.h"
#include "AltBlockGenerator.h"

using namespace cocos2d;
USING_NS_CC_EXT;

// Factory of blocks the accept several placement algorithms (strategies)
// implemented as classes that extend the IBlockGenerator interface

class BlockGeneratorFactory : public CCNode
{

private:

	// generation algorithm (strategy)
	IBlockGenerator* _generator;

	BlockGeneratorFactory();
	void setGenerator(int type);
	
public:
	// strategies available
	enum{ Simple, Alt, Random };
	static void generateBlocks(cpSpace* space,  CCLayer* scene, int type);

};

#endif  /*__BLOCK_GENERATOR_FACTORY_H__*/