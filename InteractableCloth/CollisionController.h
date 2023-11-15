#include "Ball.h"
#include "Point.h"

#include <glm\glm.hpp>

#pragma once
class CollisionController
{
public:
	CollisionController();

	void BallClothCollision(Point &point, Ball ball);

	~CollisionController();
};

