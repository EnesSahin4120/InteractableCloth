#include "CollisionController.h"

CollisionController::CollisionController()
{
}

void CollisionController::BallClothCollision(Point &point, Ball ball)
{
	glm::vec3 diff = point.position - ball.pos;
	float radiusSum = point.radius + ball.radius;
	bool isIntersect = glm::abs(glm::dot(diff, diff)) <= radiusSum * radiusSum;

	if (isIntersect) {
		glm::vec3 normal = glm::normalize(point.position - ball.pos);
		float distance = glm::abs(glm::length(ball.pos - point.position));
		float depth = radiusSum - distance;

		point.position += normal * depth;
	}
}


CollisionController::~CollisionController()
{
}
