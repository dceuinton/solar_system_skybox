// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// This program shows a not-to-scale model of the solar system. It uses a skybox and the phong shading model and fairly 
// random periods and spins. 

// Controls - 
// W, A, S, D            - Movement of virtual camera
// R, F                  - Vertical movement of virtual camera
// UP, DOWN, LEFT, RIGHT - Look of the virtual camera
// Q, E                  - Roll in the virtual camera
// SPACE                 - Pause the program to investigate individual planets

#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "openGLUtils.h"
#include "glm_virtual_camera.h"
#include "image.h"
#include "geometry.h"
#include "GLObject.h"

using namespace std;

#define PI 3.14159

GLMVirtualCamera cam; 
const char *planetVertShader = "./shader/planet.vert.glsl";
const char *planetFragShader = "./shader/planet.frag.glsl";

const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

glm::mat4 projectionMatrix = glm::perspective(glm::radians(67.0f), 1.0f, 0.1f, 100.0f);

float timeSnapshot = 0.0f;
bool  pause = false; 

void rotatePlanet(GLObject &planet, float degrees, glm::vec3 axis) {
	planet.modelMatrix = glm::rotate(glm::mat4(), glm::radians(degrees), axis);
}

void translatePlanet(GLObject &planet, glm::vec3 vector) {
	planet.modelMatrix = glm::translate(glm::mat4(), vector);
	planet.position += vector;
}

void updateModelMatrix(GLObject &object) {
	glUseProgram(object.sp);
	glUniformMatrix4fv(object.modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(object.modelMatrix));
}

void updateViewMatrix(GLObject &object) {
	glUseProgram(object.sp);
	glUniformMatrix4fv(object.viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));
}

void orbit(GLObject & object, float time) {
	float x, y, z;
	timeSnapshot = time;
	x = object.orbitRadius * cos(2*M_PI*time / object.orbitPeriod);
	y = object.orbitGradient * x;
	z = object.orbitRadius * sin(2*M_PI*time / object.orbitPeriod);
	object.modelMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, z)) * 
		glm::rotate(glm::mat4(), object.rotationSpeed * time, yAxis) * 
		glm::translate(glm::mat4(), origin);
	updateModelMatrix(object);
}

GLObject generatePlanet(const char *textureFilename, float radius, float rotationSpeed, float orbitRadius, float orbitGrad, float orbitPeriod) {
	// How many segments in your sphere
	int slices = 100;
	int stacks = slices;

	GLObject object;
	object.orbitRadius = orbitRadius;
	object.orbitGradient = orbitGrad;
	object.rotationSpeed = rotationSpeed;

	if (orbitPeriod != 0.0f) {
		object.orbitPeriod = orbitPeriod;
	} else {
		object.orbitPeriod = 1.0f;
	}

	loadObjectTexture(object, textureFilename);
	createSphereData(object.buffer, object.indices, radius, slices, stacks);
	object.sp = loadProgram(planetVertShader, NULL, NULL, NULL, planetFragShader);
	glUseProgram(object.sp);
	bindAndSetBuffers(object, false);
	object.modelMatrixLoc = glGetUniformLocation(object.sp, "uModel");
	object.viewMatrixLoc = glGetUniformLocation(object.sp, "uView");
	orbit(object, 0.0);
	updateViewMatrix(object);
	glUniformMatrix4fv(glGetUniformLocation(object.sp, "uProjection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	if (object.orbitRadius == 0.0f) {
		glUniform4f(glGetUniformLocation(object.sp, "Ia"), 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(glGetUniformLocation(object.sp, "Kd"), 0.0f, 0.0f, 0.0f, 0.0f);
		glUniform4f(glGetUniformLocation(object.sp, "Ks"), 0.0f, 0.0f, 0.0f, 0.0f);
	}

	return object;
}

void drawPlanet(GLObject &object, float time) {
	glUseProgram(object.sp);
	if (pause) {
		orbit(object, timeSnapshot);
	} else {
		orbit(object, time);
	}
	// orbit(object, 0.0);
	updateViewMatrix(object);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.texture);
	glBindVertexArray(object.vao);
	glDrawElements(GL_TRIANGLES, object.indices.size() * 3, GL_UNSIGNED_INT, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, 1);
		}
	}
}

int main() {

	float currentTime = 0.0f;
	float previousTime = glfwGetTime();
	float dt = 0.0f;
	float elapsed = 0.0f;

	// Initialises the GLFW library
	if (!glfwInit()){
		return 1;
	}	

	GLFWwindow* window = createWindow(1000, 1000, "The Amazing, Magical, and Fantastic Solar System", 3, 2); 
	if (window == NULL) {
		std::cout << "Window creation or context failed." << std::endl;
		return 1;
	}

	glfwSetKeyCallback(window, keyCallback);

	glewInit();

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Skybox stuff
	// ----------------------------------------------------------------------

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	GLuint skyboxProgram = loadProgram("./shader/skybox.vert.glsl", NULL, NULL, NULL, "./shader/skybox.frag.glsl");

	int x, y, n;

	const char *filenames[6] = {
		"images/GalaxyTex_NegativeX.tga",
		"images/GalaxyTex_PositiveX.tga",
		"images/GalaxyTex_PositiveY.tga",
		"images/GalaxyTex_NegativeYChanged.tga",
		"images/GalaxyTex_NegativeZ.tga",
		"images/GalaxyTex_PositiveZ.tga",
	};

	// const char *filenames[6] = {
	// 	"images/GalaxyTex_NegativeX.png",
	// 	"images/Space-Background.jpg",
	// 	"images/Space-Background.jpg",
	// 	"images/Space-Background.jpg",
	// 	"images/Space-Background.jpg",
	// 	"images/Space-Background.jpg",
	// };

	GLuint texture = loadTextureCubeMap(filenames, x, y, n);

	// ----------------------------------------------------------------------

	glUseProgram(skyboxProgram);

	vector<glm::vec4> skyboxBuffer;
	vector<glm::ivec3> skyboxIndices;

	createSkybox(skyboxBuffer, skyboxIndices, 10);

	GLuint skyboxVAO = 0, skyboxVBO = 0, skyboxEBO = 0;

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);

	// printf("skybox vao, vbo, and ebo: (%i, %i, %i)\n",skyboxVAO, skyboxVBO, skyboxEBO);

	// Binding
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);

	glBufferData(GL_ARRAY_BUFFER, skyboxBuffer.size() * sizeof(glm::vec4), skyboxBuffer.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, skyboxIndices.size() * sizeof(glm::ivec3), skyboxIndices.data(), GL_STATIC_DRAW);

	GLuint skyboxPosLoc = glGetAttribLocation(skyboxProgram, "vert_Position");
	glVertexAttribPointer(skyboxPosLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(skyboxPosLoc);

	// Unbinding
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ----------------------------------------------------------------------

	//Setting the skybox program uniforms
	// Set the texture unit
	glUseProgram(skyboxProgram);
	glUniform1i(glGetUniformLocation(skyboxProgram, "u_texture_Map"), 0);

	GLuint skyboxViewLoc = glGetUniformLocation(skyboxProgram, "u_View");
	glm::mat4 skyboxModel;
	// glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));
	glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseRotationMatrix()));
	// glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(skyboxModel));

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUseProgram(0);

	// ----------------------------------------------------------------------
	// End of skybox stuff
	// ----------------------------------------------------------------------
	float sunDiameter = 10.0f;
	float earthDiameter = 0.1;
	// 									(Texture, Diameter, Spinspeed, Orbit Radius, Orbit Gradient, Orbit Period)
	GLObject sun     = generatePlanet("./images/sun.jpg",           sunDiameter,         0.00f, 0.00f,                0.0f,  0.0f);
	GLObject mercury = generatePlanet("./images/mercurymap.jpg",     0.50*earthDiameter,  15.0f, 2.00f + sunDiameter, -0.05f, 5.0f);
	GLObject venus   = generatePlanet("./images/venusmap.jpg",      0.95*earthDiameter,  15.0f, 3.00f + sunDiameter,  0.05f, 8.0f);
	GLObject earth   = generatePlanet("./images/earth_texture.tga", earthDiameter,       10.0f, 4.00f + sunDiameter,  0.25f, 12.0f);
	GLObject mars    = generatePlanet("./images/marsMap.jpg",       0.53*earthDiameter, -5.0f, 6.00f + sunDiameter,  -0.30f, 16.0f);
	GLObject jupiter = generatePlanet("./images/jupiter.jpg",       11.2*earthDiameter,  2.00f, 16.0f + sunDiameter,  0.10f, 19.0f);
	GLObject saturn  = generatePlanet("./images/saturn.jpg",        9.5*earthDiameter,   3.50f, 25.0f + sunDiameter, -0.25f, 21.0f);
	GLObject uranus  = generatePlanet("./images/uranusmap.jpg",     4*earthDiameter,     6.00f, 30.0f + sunDiameter,  0.00f, 25.0f);
	GLObject neptune = generatePlanet("./images/neptunemap.jpg",    3.88*earthDiameter,  2.00f, 33.0f + sunDiameter,  0.15f, 30.0f);
	
	std::vector<GLObject> planets;
	planets.push_back(sun);
	planets.push_back(mercury);
	planets.push_back(venus);
	planets.push_back(earth);
	planets.push_back(mars);
	planets.push_back(jupiter);
	planets.push_back(saturn);
	planets.push_back(uranus);
	planets.push_back(neptune);


	// ----------------------------------------------------------------------

	float modelThetaX = 0, modelThetaY = 0;

	cam.moveBackwards(11.3f);
	cam.lookRight(1.5);

	while (!glfwWindowShouldClose(window)) {

		// Timer
		currentTime = glfwGetTime();
		dt = currentTime - previousTime;
		previousTime = currentTime;
		elapsed += dt;

		glfwMakeContextCurrent(window);
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ----------------------------------------------------------------------

		float camMoveSpeed = 5.0f;
		float camRotateSpeed = PI;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)     { cam.moveForwards(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)     { cam.moveBackwards(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)     { cam.moveLeft(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)     { cam.moveRight(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)     { cam.moveUp(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)     { cam.moveDown(dt * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    { cam.lookUp(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  { cam.lookDown(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  { cam.lookLeft(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { cam.lookRight(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)     { cam.rollLeft(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)     { cam.rollRight(dt * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { 
			if (pause) {
				elapsed = timeSnapshot;
			}
			pause = !pause;
		 }
		
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)     { cam.printPosition(); }		

		// ----------------------------------------------------------------------

		glUseProgram(skyboxProgram);
		// glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseRotationMatrix()));
		// glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(skyboxModel));

		glUseProgram(skyboxProgram);
		glBindVertexArray(skyboxVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawElements(GL_TRIANGLES, skyboxIndices.size() * 3, GL_UNSIGNED_INT, NULL);
		glEnable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		// ----------------------------------------------------------------------

		for (auto o: planets) {
			drawPlanet(o, elapsed);
		}		

		// ----------------------------------------------------------------------

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto o: planets) {
		glDeleteVertexArrays(1, &o.vao);
		glDeleteBuffers(1, &o.vbo);
		glDeleteBuffers(1, &o.ebo);
		glDeleteTextures(1, &o.texture);
		glDeleteProgram(o.sp);
	}

	// printf("%s\n", glGetString(GL_VERSION));

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &skyboxEBO);

	glDeleteProgram(skyboxProgram);

	glfwDestroyWindow(window);

	glfwTerminate();
}
