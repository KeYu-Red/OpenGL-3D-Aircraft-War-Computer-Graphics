#pragma once
#include "partical_life.h"
#include "Model.h"
class Stone :
	public BaseOBJ,public Model
{
public:
	Stone();
	~Stone();

	ObjectionType getType();
};

