#include "geometry.h"

void generateRectangle(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, /*GLfloat* modelMatrix,*/ 
						GLfloat width, GLfloat height, GLuint widthSegments, GLuint heightSegments) {

	GLfloat dx = width/widthSegments;
	GLfloat dy = height/heightSegments;

	for (int c = 0; c <= heightSegments; c++) {
		for (int r = 0; r <= widthSegments; r++) {
			vertices.push_back(-width/2.0f + r*dx);
			vertices.push_back(height/2.0f - c*dy);
			vertices.push_back(0.0f);
			vertices.push_back(1.0f);
		}
	}

	for (GLuint i = 0; i < heightSegments; i++) {
		for (GLuint j = 0; j < widthSegments; j++) {
			GLuint k = j + i + (i * widthSegments);
			indices.push_back(k);
			indices.push_back(k + 1);
			indices.push_back(k + widthSegments + 2);
			indices.push_back(k);
			indices.push_back(k + widthSegments + 2);
			indices.push_back(k + widthSegments + 1);
		}
	}
}

void generateSphere(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, GLfloat radius, GLuint stacks, GLuint slices) {
	GLfloat dPhi = M_PI/stacks;
	GLfloat dTheta = 2*M_PI/slices;

	vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(radius * 1.0f); vertices.push_back(1.0f);

    for (int i = 1; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
        	vertices.push_back(radius * cos(j*dTheta) * sin(i*dPhi));
        	vertices.push_back(radius * sin(j*dTheta) * sin(i*dPhi));
        	vertices.push_back(radius * cos(i*dPhi));
        	vertices.push_back(1.0);
        }
    }
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(-radius * 1.0f); vertices.push_back(1.0f);

	for (int i = 1; i <= slices; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i%slices + 1);
	}

	for (int k = 0; k < stacks - 2; k++) {
		for (int i = 1; i <= slices; i++) {
			indices.push_back(i + k*slices); 
			indices.push_back(i + (k+1)*slices); 
			indices.push_back(i%slices + k*slices + 1);
			indices.push_back(i + (k+1)*slices); 
			indices.push_back(i%slices + (k+1)*slices + 1); 
			indices.push_back(i%slices + k*slices + 1);
		}
	}
	int lastPt = (vertices.size()/4) - 1; //The 4 is the size of the vectors you're using. Usually 4D.

	for (int j = 1; j <= slices; j++) {
		indices.push_back(lastPt);
		indices.push_back(lastPt - j);
		indices.push_back(lastPt - j%(slices) - 1);
	}

}

void generateCylinder(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices, GLfloat radius, GLfloat height, GLuint circleSegments, GLuint heightSegments) {

	GLfloat dh = height/heightSegments;
	GLfloat dc = 2*M_PI/circleSegments; // An angle
	GLfloat theta = 0.0f; 
	GLfloat currentHeight = 0.0f;

	// Center of the bottom circle
	vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);

	while (currentHeight <= height) {
		theta = 0.0f;
		while (theta < 2* M_PI) {
			if ((theta <= M_PI/2.0f) || (theta >= 3 * M_PI/2.0f)) {
				vertices.push_back(radius * cos(theta));
			} else {
				vertices.push_back(-radius * cos(M_PI - theta));
			}
			vertices.push_back(currentHeight); 
			vertices.push_back(radius * sin(theta));
			vertices.push_back(1.0f);
			
			theta += dc; 
		}
		currentHeight += dh; 
	}

	// Center of the top circle
	vertices.push_back(0.0f); vertices.push_back(height); vertices.push_back(0.0f); vertices.push_back(1.0f);

	// Indices part

	for (int i = 1; i <= circleSegments; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i%circleSegments + 1);
	}

	for (int k = 0; k < heightSegments; k++) {
		for (int i = 1; i <= circleSegments; i++) {
			indices.push_back(i + k*circleSegments); 
			indices.push_back(i + (k+1)*circleSegments); 
			indices.push_back(i%circleSegments + k*circleSegments + 1);
			indices.push_back(i + (k+1)*circleSegments); 
			indices.push_back(i%circleSegments + (k+1)*circleSegments + 1); 
			indices.push_back(i%circleSegments + k*circleSegments + 1);
		}
	}

	int lastPt = vertices.size() / 4 - 1;

	for (int j = 1; j <= circleSegments; j++) {
		indices.push_back(lastPt);
		indices.push_back(lastPt - j);
		indices.push_back(lastPt - j%(circleSegments) - 1);
	}	
}