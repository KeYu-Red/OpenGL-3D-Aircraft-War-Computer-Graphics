#ifndef _COLLISIONCONTROLLER_H
#define _COLLISIONCONTROLLER_H
#include "partical_life.h"
#include<glad/glad.h>

class CollisionController
{
public:
	CollisionController();
	~CollisionController();
	GLboolean checkCollision(BaseOBJ &one, BaseOBJ &two);
};



#endif // !_COLLISIONCONTROLLER_H

