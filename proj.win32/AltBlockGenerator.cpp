#include "AltBlockGenerator.h"

USING_NS_CC;

void AltBlockGenerator::generateBlocks(cpSpace* space, CCLayer* scene)
{
	// from left->right & top->down fill rand rows of random type blocks
	// placing blocks alternatively (1 block, 1 hole, 1 block, ...)
	// initial block: pt.x = blockWidth/2, pt.y = height - blockHeight/2;
	// x increments: blockWidth
	// y decrements (going down): blockheight
	int blockType = 0;
	bool block_ok = true;
	float initialX = VisibleRect::leftBottom().x + _paddingW/2;
	float initialY = VisibleRect::leftTop().y - _paddingW/2;
	CCPoint pt = ccp(initialX, initialY);
	for(int i = 0; i < _rows; i++) {
		for(int j = 0; j < _cols; j++) {
			blockType = rand() % 3;
			if(block_ok) {
				pt.x = initialX + _paddingW * j;
				pt.y = initialY - _paddingH * i;
				Block* b = Block::addNewBlockAtPosition(pt,space,blockType);
				scene->addChild(b);
				block_ok = false;
			} else {
				block_ok = true;
			}
		}
	}
}