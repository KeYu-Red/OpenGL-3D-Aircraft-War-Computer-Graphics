#include "Stone.h"



Stone::Stone()
	:BaseOBJ(ObjectionType::stone,
	glm::vec3(0.0, 0.0, -5.0),
		glm::vec3(0.0, 0.0, 1.0),
		3,
		3.0)
	, Model("objects/stone.obj",
		"objects/RockSmooth0076_4_L.jpg",
		"objects/stone.mtl")
{
	extents = glm::vec3(1.0f, 1.0f, 1.0f) * glm::vec3(0.8,0.8,0.8);
}


Stone::~Stone()
{
}

ObjectionType Stone::getType()
{
	return type;
}

