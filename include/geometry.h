#include <GL/glew.h>
#include <vector>
#include <cmath>

void generateRectangle(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, 
						GLfloat width, GLfloat height, GLuint widthSegments = 1, GLuint heightSegments = 1);
void generateSphere(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, GLfloat radius, GLuint stacks, GLuint slices);
void generateCylinder(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, GLfloat radius, GLfloat height, GLuint circleSegments, GLuint heightSegments);