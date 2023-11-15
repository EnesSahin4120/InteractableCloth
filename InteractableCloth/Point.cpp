#include "Point.h"

Point::Point()
{
}

Point::Point(glm::vec3 position, bool isStatic, float radius, float friction)
{
	this->position = position;
	previousPos = this->position;
	velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	this->isStatic = isStatic;
	this->radius = radius;
	this->friction = friction;
}



Point::~Point()
{
}
