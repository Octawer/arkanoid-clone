#ifndef __I_BLOCK_GENERATOR_H__
#define __I_BLOCK_GENERATOR_H__

#include "cocos2d.h"
#include "chipmunk.h" 
#include "cocos-ext.h"	
#include "Block.h"
#include "VisibleRect.h"

//
using namespace cocos2d;
USING_NS_CC_EXT;

// interface for generic block generation algorithm
// implemented by several block generation strategies

class IBlockGenerator : public CCNode
{

protected:
	
	float _paddingW;		// block width spacing
	float _paddingH;		// block height spacing
	int _rows;
	int _cols;
	int _level;				// not used atm (foreseen for difficulty settings)

public:

	IBlockGenerator();
	virtual void generateBlocks(cpSpace* space, CCLayer* scene) = 0;		// pure virtual function (interface behaviour)
	// virtual ~IBlockGenerator();

};

#endif /* __I_BLOCK_GENERATOR_H__ */