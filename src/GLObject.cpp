#include "GLObject.h"

void loadObjectTexture(GLObject &object, const char *filename) {
	int x, y, n;
	object.texture = loadTexture2D(filename, x, y, n);
}

void bindAndSetBuffers(GLObject &object, bool lines) {

	glUseProgram(object.sp);

	glGenVertexArrays(1, &object.vao);
	glGenBuffers(1, &object.vbo);
	glGenBuffers(1, &object.ebo);

	glBindVertexArray(object.vao);
	glBindBuffer(GL_ARRAY_BUFFER, object.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ebo);

	glBufferData(GL_ARRAY_BUFFER, object.buffer.size() * sizeof(glm::vec4), object.buffer.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indices.size() * sizeof(glm::ivec3), object.indices.data(), GL_STATIC_DRAW);

	GLuint objectVertexLoc = glGetAttribLocation(object.sp, "vertexPosition");
	GLuint objectNormalLoc = glGetAttribLocation(object.sp, "normalVector");
	GLuint objectTCLoc = glGetAttribLocation(object.sp, "textureCoordinate");

	glVertexAttribPointer(objectVertexLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), NULL);
	glVertexAttribPointer(objectNormalLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (4 * sizeof(GLfloat)));
	glVertexAttribPointer(objectTCLoc, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*) (8 * sizeof(GLfloat)));

	glEnableVertexAttribArray(objectVertexLoc);
	glEnableVertexAttribArray(objectNormalLoc);
	glEnableVertexAttribArray(objectTCLoc);

	if (lines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
