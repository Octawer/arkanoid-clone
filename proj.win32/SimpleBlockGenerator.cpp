#include "SimpleBlockGenerator.h"

USING_NS_CC;

void SimpleBlockGenerator::generateBlocks(cpSpace* space, CCLayer* scene)
{
	// from left->right & top->down fill 3 rows of blocks (1-hit red ones)
	// initial block: pt.x = blockWidth/2, pt.y = height - blockHeight/2;
	// x increments: blockWidth
	// y decrements (going down): blockheight

	float initialX = VisibleRect::leftBottom().x + _paddingW/2;
	float initialY = VisibleRect::leftTop().y - _paddingW/2;
	CCPoint pt = ccp(initialX, initialY);
	for(int i = 0; i < _rows; i++) {
		for(int j = 0; j < _cols; j++) {
			pt.x = initialX + _paddingW * j;
			pt.y = initialY - _paddingH * i;
			Block* b = Block::addNewBlockAtPosition(pt,space, Block::Red);
			scene->addChild(b);
		}
	}
}