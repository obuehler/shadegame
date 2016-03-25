#pragma once

#include "M_MovingObject.h"

class Runner
{
public:
	typedef ActionQueue::ActionNode Node;
	Runner() {};
	~Runner() {};
	void run();
};

