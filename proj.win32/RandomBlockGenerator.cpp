#include "RandomBlockGenerator.h"

USING_NS_CC;


void RandomBlockGenerator::generateBlocks(cpSpace* space, CCLayer* scene)
{
	// from left->right & top->down fill rand rows of blocks (random type)
	// rolling a dice (block_ok) to see if next one is blank or has block
	// initial block: pt.x = blockWidth/2, pt.y = height - blockHeight/2;
	// x increments: blockWidth
	// y decrements (going down): blockheight
	int blockType = 0;
	bool block_ok = 0;
	float initialX = VisibleRect::leftBottom().x + _paddingW/2;
	float initialY = VisibleRect::leftTop().y - _paddingW/2;
	CCPoint pt = ccp(initialX, initialY);
	for(int i = 0; i < _rows; i++) {
		for(int j = 0; j < _cols; j++) {
			blockType = rand() % 3;
			block_ok = rand() % 2;
			if(block_ok) {
				pt.x = initialX + _paddingW * j;
				pt.y = initialY - _paddingH * i;
				Block* b = Block::addNewBlockAtPosition(pt,space,blockType);
				scene->addChild(b);	
			}
		}
	}
}