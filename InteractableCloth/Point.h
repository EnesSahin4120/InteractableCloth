#include <glm\glm.hpp>

#pragma once
class Point
{
public:
	Point();
	Point(glm::vec3 position, bool isStatic, float radius, float friction);

	glm::vec3 position, previousPos, velocity;
	bool isStatic;
	float radius, friction;

	~Point();
};

