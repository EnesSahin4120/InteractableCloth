#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Ball.h"
#include "Cloth.h"

#include "Skybox.h"

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

Mesh *groundMesh;
std::vector<Mesh*> stickMeshes;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Window mainWindow;
Camera camera;

Texture clothTexture;
Texture groundTexture;
Texture ballTexture;
Texture stickTexture;

Material gameMaterial;

Ball ball;
Cloth *cloth;

DirectionalLight mainLight;
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObjects()
{
	unsigned int stickIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		1, 0, 4,
		5, 1, 4,
		2, 1, 5,
		2, 5, 6,
		3, 2, 7,
		2, 6, 7,
		5, 4, 6,
		6, 4, 7,
		0, 3, 4,
		4, 3, 7
	};

	GLfloat stickVertices[] =
	{
		0, 1, 1, 0, 0, 0, 0, 1,
		1, 1, 1, 0, 0, 0, 0, 1,
		1, 1, 0, 0, 0, 0, 0, 1,
		0, 1, 0, 0, 0, 0, 0, 1,
		0, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 1
	};

	unsigned int groundIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat groundVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	groundMesh = new Mesh();
	groundMesh->CreateMesh(groundVertices, groundIndices, 32, 6);

	Mesh *stickMesh = new Mesh();
	stickMesh->CreateMesh(stickVertices, stickIndices, 64, 36);
	stickMeshes.push_back(stickMesh);
	stickMeshes.push_back(stickMesh);
	stickMeshes.push_back(stickMesh);

	cloth = new Cloth(8, 0.5f);
	ball = Ball(20, 20, 0.75f, glm::vec3(2, -2.0f, -1.0f));
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");
}

void RenderScene()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	clothTexture.UseTexture();
	gameMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	cloth->clothMesh->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, ball.pos);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	ballTexture.UseTexture();
	ball.mesh->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	groundTexture.UseTexture();
	groundMesh->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 5.0f, 0.2f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	stickTexture.UseTexture();
	stickMeshes[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -5.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 5.0f, 0.2f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	stickMeshes[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.1f, 0.0f, 0.0f));
	model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(0.1f, 5.0f, 0.1f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	stickMeshes[2]->RenderMesh();
}

void DirectionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();

	glViewport(0, 0, light->getShadowMap()->GetShadowWidth(), light->getShadowMap()->GetShadowHeight());

	light->getShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glViewport(0, 0, 1366, 768);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.getShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shaderList[0].Validate();

	RenderScene();
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(3.0f, -1.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 120.0f, 0.0f, 5.0f, 0.5f);

	clothTexture = Texture("Textures/ClothTexture.jpg");
	clothTexture.LoadTexture();
	groundTexture = Texture("Textures/GroundTexture.tga");
	groundTexture.LoadTextureA();
	ballTexture = Texture("Textures/BallTexture.jpg");
	ballTexture.LoadTexture();
	stickTexture = Texture("Textures/StickTexture.tga");
	stickTexture.LoadTextureA();

	gameMaterial = Material(0.3f, 4);

	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.1f, 0.9f,
		0.0f, -1.0f, 0.0f);


	spotLights[0] = SpotLight(1024, 1024,
		0.01f, 100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		50.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/Left.png");
	skyboxFaces.push_back("Textures/Skybox/Right.png");
	skyboxFaces.push_back("Textures/Skybox/Up.png");
	skyboxFaces.push_back("Textures/Skybox/Down.png");
	skyboxFaces.push_back("Textures/Skybox/Front.png");
	skyboxFaces.push_back("Textures/Skybox/Back.png");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;

		glfwPollEvents();

		int iterations = 25;

		for (int i = 0; i < iterations; i++) {
			GLfloat fixedDeltaTime = deltaTime / (float)iterations;

			ball.Controlling(mainWindow.getsKeys(), fixedDeltaTime);
			cloth->Simulate(fixedDeltaTime, ball);
			cloth->UpdateMesh();
		}

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		DirectionalShadowMapPass(&mainLight);
		for (size_t i = 0; i < spotLightCount; i++)
			OmniShadowMapPass(&spotLights[i]);

		RenderPass(camera.calculateViewMatrix(), projection);

		mainWindow.swapBuffers();
	}

	return 0;
}