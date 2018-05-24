
#ifndef GLOBJECT_DALE
#define GLOBJECT_DALE

#include <vector>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "image.h"
#include "glm_virtual_camera.h"


using namespace std;

struct GLObject {
	vector<glm::vec4> buffer;
	vector<glm::ivec3> indices;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint sp;
	GLuint modelMatrixLoc;
	GLuint viewMatrixLoc;
	GLuint texture;
	GLMVirtualCamera cam; 
	glm::mat4 modelMatrix;
	float rotationSpeed;
	glm::vec3 position;
	float orbitRadius;
	float orbitGradient;
	float orbitPeriod;
};

void loadObjectTexture(GLObject &object, const char *filename);
void bindAndSetBuffers(GLObject &object, bool lines);

#endif