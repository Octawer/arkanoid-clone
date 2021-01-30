#include "BlockGeneratorFactory.h"

BlockGeneratorFactory::BlockGeneratorFactory()
{
	// default generator
	_generator = new SimpleBlockGenerator();
}

void BlockGeneratorFactory::setGenerator(int type) 
{
	if(_generator) delete _generator;
	switch(type)
	{
	case Simple:
		_generator = new SimpleBlockGenerator();
		break;
	case Alt:
		_generator = new AltBlockGenerator();
		break;
	case Random:
		_generator = new RandomBlockGenerator();
		break;
	default:
		_generator = new SimpleBlockGenerator();
		break;
	}
}

void BlockGeneratorFactory::generateBlocks(cpSpace* space, CCLayer* scene, int type)
{
	// create generator factory
	// using RAII (factory on the stack, auto. freed)
	BlockGeneratorFactory factory;
	factory.setGenerator(type);	
	if(factory._generator) {
		factory._generator->generateBlocks(space, scene);
	}

	delete factory._generator;
}