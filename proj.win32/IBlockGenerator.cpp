#include "IBlockGenerator.h"


IBlockGenerator::IBlockGenerator()
{
	// we set the padding width and height according to the block texture dimensions
	CCSprite* blockRef = CCSprite::create("bricks/brick_blue.png");
	_paddingW = blockRef->getTextureRect().size.width; 
	_paddingH = blockRef->getTextureRect().size.height;
}