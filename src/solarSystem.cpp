// Assignment number 2, 159.709, 2018 S1
// Euinton, Dale, 14026002,
// Explain what the program is doing . . .

#include <iostream>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "openGLUtils.h"

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

	while (!glfwWindowShouldClose(window)) {
		glfwMakeContextCurrent(window);
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Hello World!\n");
}
