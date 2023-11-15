#include "Cloth.h"

Cloth::Cloth()
{
}

int Cloth::IndexFrom2D(int x, int y)
{
	return y * n + x;
}

Cloth::Cloth(int n, float rangeBetweenPoints)
{
	this->n = n;
	this->rangeBetweenPoints = rangeBetweenPoints;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			bool isStatic = (i==0);
			glm::vec3 position = glm::vec3(j * rangeBetweenPoints, 0.0f, i * rangeBetweenPoints);
			Point* currentPoint = new Point(position, isStatic, 0.125f, 0.75f);
			points.push_back(*currentPoint);

			verticesList.push_back(position.x);
			verticesList.push_back(position.y);
			verticesList.push_back(position.z);

			verticesList.push_back((float)(j / (float)(n - 1)));
			verticesList.push_back((float)(i / (float)(n - 1)));

			verticesList.push_back(0);
			verticesList.push_back(0);
			verticesList.push_back(1);
		}
	}

	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - 1; j++) {
			indicesList.push_back(i * n + j);
			indicesList.push_back(n * (i + 1) + j + 1);
			indicesList.push_back(i * n + j + 1);

			indicesList.push_back(i * n + j);
			indicesList.push_back(n * (i + 1) + j);
			indicesList.push_back(n * (i + 1) + j + 1);
		}
	}

	vertices = &verticesList[0];
	indices = &indicesList[0];

	clothMesh = new Mesh();
	clothMesh->CreateMesh(vertices, indices, verticesList.size(), indicesList.size());
}

void Cloth::Simulate(GLfloat deltaTime, Ball &ball)
{
	for (int i = 0; i < points.size(); i++) {
		if (!points[i].isStatic) {
			glm::vec3 prevPos = points[i].position;
			glm::vec3 displacement = points[i].position - points[i].previousPos;
			points[i].velocity = displacement / deltaTime;
			points[i].position += displacement;
			points[i].position += glm::vec3(0.0f, -1.0f, 0.0f) * g * deltaTime * deltaTime;

			float speed = glm::sqrt(glm::length(points[i].velocity));
		    points[i].position -= displacement * points[i].friction * glm::pow(speed, 2.0f) * deltaTime;
			points[i].previousPos = prevPos;
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int index0 = IndexFrom2D(j, i);
			int index1 = IndexFrom2D(j + 1, i);
			int index2 = IndexFrom2D(j, i + 1);

			collisionController.BallClothCollision(points[index0], ball);

			glm::vec3 stickCenter;
			glm::vec3 stickDir;
			float length;

			if (j < n - 1)
			{
				stickCenter = (points[index0].position + points[index1].position) * 0.5f;
				stickDir = glm::normalize(points[index0].position - points[index1].position);
				length = glm::length(points[index0].position - points[index1].position);

				if (length > rangeBetweenPoints) {
					if (!points[index0].isStatic)
						points[index0].position = stickCenter + stickDir * 0.5f * rangeBetweenPoints;
					if (!points[index1].isStatic)
						points[index1].position = stickCenter - stickDir * 0.5f * rangeBetweenPoints;
				}
			}
			if (i < n - 1)
			{
				stickCenter = (points[index0].position + points[index2].position) * 0.5f;
				stickDir = glm::normalize(points[index0].position - points[index2].position);
				length = glm::length(points[index0].position - points[index2].position);

				if (length > rangeBetweenPoints) {
					if (!points[index0].isStatic)
						points[index0].position = stickCenter + stickDir * 0.5f * rangeBetweenPoints;
					if (!points[index2].isStatic)
						points[index2].position = stickCenter - stickDir * 0.5f * rangeBetweenPoints;
				}
			}
		}
	}
}

void Cloth::UpdateMesh()
{
	verticesList.clear();

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {

			verticesList.push_back(points[IndexFrom2D(j ,i)].position.x);
			verticesList.push_back(points[IndexFrom2D(j, i)].position.y);
			verticesList.push_back(points[IndexFrom2D(j, i)].position.z);

			verticesList.push_back((float)(j / (float)(n - 1)));
			verticesList.push_back((float)(i / (float)(n - 1)));

			verticesList.push_back(0);
			verticesList.push_back(0);
			verticesList.push_back(1);
		}
	}
	vertices = &verticesList[0];
	CalculateNormals(indices, indicesList.size(), vertices, verticesList.size(), 8, 5);

	clothMesh = new Mesh();
	clothMesh->CreateMesh(vertices, indices, verticesList.size(), indicesList.size());
}

void Cloth::CalculateNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


Cloth::~Cloth()
{
}
