#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>

#include "Point.h"
#include "Mesh.h"
#include "CollisionController.h"
#include "Ball.h"

#include "CommonValues.h"

#pragma once
class Cloth
{
public:
	Cloth();
	Cloth(int n, float rangeBetweenPoints);

	void Simulate(GLfloat deltaTime, Ball &ball);
	void UpdateMesh();

	Mesh* clothMesh;

	~Cloth();

private:
	CollisionController collisionController;

	void CalculateNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
		unsigned int vLength, unsigned int normalOffset);

	int n;
	float rangeBetweenPoints;

	std::vector<Point> points;
	int IndexFrom2D(int x, int y);

	GLfloat* vertices;
	unsigned int* indices;
	std::vector<GLfloat> verticesList;
	std::vector<unsigned int> indicesList;
};

