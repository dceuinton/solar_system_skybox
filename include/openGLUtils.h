#ifndef OPEN_GL_UTILS_
#define OPEN_GL_UTILS_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>

void onError(int error, const char *description);
void onWindowClose(GLFWwindow *window);
void onFramebufferSize(GLFWwindow *window, int width, int height);
GLFWwindow* createWindow(int width, int height, const char *title, int major = 3, int minor = 2, GLFWmonitor *monitor = NULL, GLFWwindow *share = NULL);
char* readFile(const char *filename);
GLuint checkShader(GLuint shader);
GLuint checkProgram(GLuint program);
GLuint loadShader(GLuint type, const char *filename);
GLuint loadProgram(const char *vert_file, const char *ctrl_file, const char *eval_file, const char *geom_file, const char *frag_file);

void printMat4(float m[16]);
void printGLfloatVector(std::vector<GLfloat>& vec);
void printGLuintVector(std::vector<GLuint>& vec);
void printZValues(std::vector<GLfloat>& vec);

#endif 