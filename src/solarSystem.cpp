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

using namespace std;

#define PI 3.14159

GLMVirtualCamera cam; 

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

	createSkybox(skyboxBuffer, skyboxIndices);

	GLuint skyboxVAO = 0, skyboxVBO = 0, skyboxEBO = 0;

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);

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
	glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(67.0f), 1.0f, 0.1f, 50.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "u_Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUseProgram(0);

	// ----------------------------------------------------------------------

	float currentTime = 0.0;
	float previousTime = glfwGetTime();
	float elapsed = 0.0;

	while (!glfwWindowShouldClose(window)) {

		// Timer
		currentTime = glfwGetTime();
		elapsed = currentTime - previousTime;
		previousTime = currentTime;

		glfwMakeContextCurrent(window);
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ----------------------------------------------------------------------

		float camMoveSpeed = 1.0f;
		float camRotateSpeed = PI;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)     { cam.moveForwards(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)     { cam.moveBackwards(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)     { cam.moveLeft(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)     { cam.moveRight(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)     { cam.moveUp(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)     { cam.moveDown(elapsed * camMoveSpeed); }
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    { cam.lookUp(elapsed * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  { cam.lookDown(elapsed * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  { cam.lookLeft(elapsed * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { cam.lookRight(elapsed * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)     { cam.rollLeft(elapsed * camRotateSpeed); }
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)     { cam.rollRight(elapsed * camRotateSpeed); }

		// ----------------------------------------------------------------------

		glUseProgram(skyboxProgram);
		glUniformMatrix4fv(skyboxViewLoc, 1, GL_FALSE, glm::value_ptr(*cam.getInverseViewMatrix()));

		glUseProgram(skyboxProgram);
		glBindVertexArray(skyboxVAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glDrawElements(GL_TRIANGLES, skyboxIndices.size() * 3, GL_UNSIGNED_INT, NULL);
		glEnable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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
