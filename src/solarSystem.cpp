// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// Explain what the program is doing . . .

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

void orbit(GLObject & object, float radius, float time, float gradient) {
	float x, y, z;
	x = radius * cos(time);
	y = gradient * x;
	z = radius * sin(time);
	object.modelMatrix = glm::translate(glm::mat4(), glm::vec3(x, y, z)) * 
		glm::rotate(glm::mat4(), object.rotationSpeed * time, yAxis) * 
		glm::translate(glm::mat4(), origin);
	updateModelMatrix(object);
}

GLObject generatePlanet(const char *textureFilename, float radius, float rotationSpeed, float orbitRadius, float orbitGrad) {
	// How many segments in your sphere
	int slices = 100;
	int stacks = slices;

	GLObject object;
	object.orbitRadius = orbitRadius;
	object.orbitGradient = orbitGrad;
	object.rotationSpeed = rotationSpeed;
	loadObjectTexture(object, textureFilename);
	createSphereData(object.buffer, object.indices, radius, slices, stacks);
	object.sp = loadProgram(planetVertShader, NULL, NULL, NULL, planetFragShader);
	glUseProgram(object.sp);
	bindAndSetBuffers(object, false);

	return object;
}

void drawPlanet(GLObject &object, float time) {
	glUseProgram(object.sp);
	orbit(object, object.orbitRadius, time, object.orbitGradient);
	updateViewMatrix(object);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object.texture);
	glBindVertexArray(object.vao);
	glDrawElements(GL_TRIANGLES, object.indices.size() * 3, GL_UNSIGNED_INT, NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

// void spinOnYMatrix(GLObject &object, float time) {
// 	object.modelMatrix = glm::rotate(object.modelMatrix, time * object.rotationSpeed, yAxis);
// 	glUseProgram(object.sp);
// 	glUniformMatrix4fv(object.modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(object.modelMatrix));
// }

// void updateModelMatrix(GLObject &object, float time) {
// 	object.modelMatrix = glm::rotate(object.modelMatrix, time * object.rotationSpeed, yAxis) * 
// 						glm::translate() * 
// 						glm::rotate() * 
// 						glm::translate();
// }

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, 1);
		}
	}
}

int main() {

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
		"images/Space-Background.jpg",
		"images/Space-Background.jpg",
		"images/Space-Background.jpg",
		"images/Space-Background.jpg",
		"images/Space-Background.jpg",
		"images/Space-Background.jpg",
	};

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

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(67.0f), 1.0f, 0.1f, 50.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUseProgram(0);

	// ----------------------------------------------------------------------
	// End of skybox stuff
	// ----------------------------------------------------------------------

	GLObject sun = generatePlanet("./images/sun.jpg", 1.09f, 0.0f, 0.0f, 0.0f);
	GLObject earth = generatePlanet("./images/earth_texture.tga", 0.1, 10.0f, 3.0f, 0.25f);

	// ----------------------------------------------------------------------

	glUseProgram(sun.sp);
	sun.modelMatrixLoc = glGetUniformLocation(sun.sp, "uModel");
	sun.viewMatrixLoc = glGetUniformLocation(sun.sp, "uView");

	glm::mat4 sunModelMatrix;
	glUniformMatrix4fv(sun.modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(sunModelMatrix));
	// glUniformMatrix4fv(sun.viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));
	updateViewMatrix(sun);
	glUniformMatrix4fv(glGetUniformLocation(earth.sp, "uProjection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUseProgram(earth.sp);
	earth.modelMatrixLoc = glGetUniformLocation(earth.sp, "uModel");
	earth.viewMatrixLoc  = glGetUniformLocation(earth.sp, "uView");

	// earth.modelMatrix = glm::rotate(glm::mat4(), glm::radians(-30.0f), yAxis) * 
	// 				    glm::rotate(glm::mat4(), glm::radians(180.0f), zAxis) *
	// 				    glm::translate(glm::mat4(), glm::vec3(-3.0f, 0.0f, 0.0f));

	translatePlanet(earth, glm::vec3(3.0f, 0.0f, 0.0f));
	// rotatePlanet(earth, 210.0, zAxis);
	rotatePlanet(earth, 180.0, zAxis);
	
	// glUniformMatrix4fv(earth.modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(*earth.cam.getViewMatrix()));
	// glUniformMatrix4fv(earth.modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(earth.modelMatrix));
	updateModelMatrix(earth);
	updateViewMatrix(earth);
	// glUniformMatrix4fv(earth.viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(earth.sp, "uProjection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// ----------------------------------------------------------------------

	float currentTime = 0.0f;
	float previousTime = glfwGetTime();
	float dt = 0.0f;
	float elapsed = 0.0f;

	float modelThetaX = 0, modelThetaY = 0;

	cam.moveBackwards(10.0f);

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

		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)     { modelThetaX += dt * camRotateSpeed; }		
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)     { modelThetaX -= dt * camRotateSpeed; }		
		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)     { modelThetaY += dt * camRotateSpeed; }		
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)     { modelThetaY -= dt * camRotateSpeed; }		

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

		drawPlanet(sun, elapsed);

		drawPlanet(earth, elapsed);

		// ----------------------------------------------------------------------

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("%s\n", glGetString(GL_VERSION));

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &skyboxEBO);

	glDeleteProgram(skyboxProgram);

	glfwDestroyWindow(window);

	glfwTerminate();
}
