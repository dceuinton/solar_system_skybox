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

// Create Skybox (Cube with Positions only)
void createSkybox(std::vector<glm::vec4> &buffer, std::vector<glm::ivec3> &indices, int size) {
	// Vertices
	buffer.push_back(glm::vec4(size * -1.0f,  size * 1.0f, size * -1.0f,  size * 1.0f)); // Left  - Top    - Back  - 0
	buffer.push_back(glm::vec4( size * 1.0f,  size * 1.0f, size * -1.0f,  size * 1.0f)); // Right - Top    - Back  - 1
	buffer.push_back(glm::vec4(size * -1.0f,  size * 1.0f,  size * 1.0f,  size * 1.0f)); // Left  - Top    - Front - 2
	buffer.push_back(glm::vec4( size * 1.0f,  size * 1.0f,  size * 1.0f,  size * 1.0f)); // Right - Top    - Front - 3
	buffer.push_back(glm::vec4(size * -1.0f, size * -1.0f, size * -1.0f,  size * 1.0f)); // Left  - Bottom - Back  - 4
	buffer.push_back(glm::vec4( size * 1.0f, size * -1.0f, size * -1.0f,  size * 1.0f)); // Right - Bottom - Back  - 5
	buffer.push_back(glm::vec4(size * -1.0f, size * -1.0f,  size * 1.0f,  size * 1.0f)); // Left  - Bottom - Front - 6
	buffer.push_back(glm::vec4( size * 1.0f, size * -1.0f,  size * 1.0f,  size * 1.0f)); // Right - Bottom - Front - 7

	// Indexes
	indices.push_back(glm::ivec3(0, 2, 3)); // Top
	indices.push_back(glm::ivec3(0, 3, 1));
	indices.push_back(glm::ivec3(4, 5, 7)); // Bottom
	indices.push_back(glm::ivec3(4, 7, 6));
	indices.push_back(glm::ivec3(0, 2, 6)); // Left
	indices.push_back(glm::ivec3(0, 6, 4));
	indices.push_back(glm::ivec3(3, 1, 5)); // Right
	indices.push_back(glm::ivec3(3, 5, 7));
	indices.push_back(glm::ivec3(2, 3, 7)); // Front
	indices.push_back(glm::ivec3(2, 7, 6));
	indices.push_back(glm::ivec3(1, 0, 4)); // Back
	indices.push_back(glm::ivec3(1, 4, 5));
}

// --------------------------------------------------------------------------------
// Create Sphere with Positions and Normals
void createSphereData(std::vector<glm::vec4> &buffer, std::vector<glm::ivec3> &indexes, float r, int sub1, int sub2) {
	// Longitude
	for(int i1 = 0; i1 < sub1; i1++) {
		// Theta [0, pi]
		float theta = i1 * M_PI / (sub1-1);

		// Longitude offset
		int offset1 = -sub2;

		// Latitude
		// ----------------------------------------------------------------------
		// Expand this!!!! 
		// ----------------------------------------------------------------------
		for(int i2 = 0; i2 <= sub2; i2++) {
			// Phi [0, 2pi)
			float phi = i2 * M_PI * 2.0 / sub2;

			// Calculate point
			glm::vec4 p = glm::vec4(r*sin(theta)*cos(phi), r*cos(theta), r*sin(theta)*sin(phi), 1.0f);

			// Calculate normal
			glm::vec4 u = glm::normalize(glm::vec4(p.x, p.y, p.z, 0.0f));

			// Add position, normal and UV to buffer
			buffer.push_back(p);
			buffer.push_back(u);
			buffer.push_back(glm::vec4(phi / (M_PI*2.0f), theta / M_PI, 0.0f, 1.0f));

			// Latitude offset
			int offset2 = (i2 < (sub2 - 1)) ? 1 : -(sub2 - 1);

			// Add triangles between layers
			if(i1 > 0) {
				// Index of current vertex
				int k = i1*sub2 + i2;

				// Add Indexes
				indexes.push_back(glm::ivec3(k + offset1, k,           k + offset2));
				indexes.push_back(glm::ivec3(k + offset1, k + offset2, k + offset1 + offset2));
			}
		}
	}
}