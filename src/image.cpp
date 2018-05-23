// Project Header
#include "image.h"

// stb_image Header
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --------------------------------------------------------------------------------
// Image Functions
// --------------------------------------------------------------------------------

// Load an image from file
unsigned char* loadImage(const char *filename, int &width, int &height, int &n, bool flip) {
	// Force RGBA 
	int force_channels = 4;

	// Load image
	unsigned char *image = stbi_load(filename, &width, &height, &n, force_channels);

	// Channels forced to 4
	n = 4;

	// Check result
	if(!image) {
		// Print error message
		std::cerr << "Error: could not load image: " << filename << std::endl;
		
		// Return error
		return NULL;
	}

	// Temporary buffer
	unsigned char *t = new unsigned char[width*n];

	// If flip on
	if(flip) {
		// Flip image vertically
		for(int iy = 0; iy < height/2; iy++) {
			// Copy row iy into temporary buffer
			memcpy(t, &image[iy*width*n], width*n);

			// Copy row ((height-1)-iy) into row iy
			memcpy(&image[iy*width*n], &image[((height-1)-iy)*width*n], width*n);

			// Copy temporary buffer into row ((height-1)-iy)
			memcpy(&image[((height-1)-iy)*width*n], t, width*n);
		}
	}

	// Delete temporary buffer
	delete[] t;

	// Check dimensions are power-of-2
	if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
		// Print warning message
		std::cerr << "Warning: image " << filename << " is not power-of-2 dimensions" << std::endl;
	}

	// Print log message
	std::cout << "Loaded: " << filename << std::endl;

	// Return image
	return image;
}

GLuint loadTexture2D(const char *filename, int &width, int &height, int &n) {
	// ----------------------------------------
	// Load Texture Map from file
	unsigned char *image = loadImage(filename, width, height, n, true);

	// Check image result
	if(image == NULL) {
		// Return error
		return 0;
	}

	// Texture
	GLuint texture;

	// Generate texture
	glGenTextures(1, &texture);

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, texture);

	//int max_levels = glm::max(glm::log2(width), glm::log2(height));
	int levels_x = (int)glm::log2((float)width);
	int levels_y = (int)glm::log2((float)height);
	int max_levels = glm::max(levels_x, levels_y);
	
	// ------------------------------
	// Mip-Mapping
	// ------------------------------
	// Set storage - log_2(image size)
	glTexStorage2D(GL_TEXTURE_2D, max_levels, GL_RGBA8, width, height);

	// Copy image data into texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);

	// Generate Mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// Configure texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Configure Texture Coordinate Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
	
	// ------------------------------
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Delete image data
	delete[] image;
	image = NULL;

	// Return Texture
	return texture;
}

// Load a CubeMap Texture from file
GLuint loadTextureCubeMap(const char *filename[6], int &width, int &height, int &n) {

	printf("Loading Cube Map\n");
	// Texture
	GLuint texture;

	// Generate texture
	glGenTextures(1, &texture);

	// Bind texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// Load six faces
	for(int i = 0; i < 6; i++) {
		// Load image from file
		unsigned char *image = loadImage(filename[i], width, height, n, true);

		// ----------------------------------------
		// No Mip-Mapping - Copy Image into texture
		// glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// ----------------------------------------
		// Mip-Mapping
		if(i == 0) {
			// Mip-Mapping
			int levels_x = (int)glm::log2((float)width);
			int levels_y = (int)glm::log2((float)height);
			int max_levels = glm::max(levels_x, levels_y);

			// Set storage - log_2(image size)
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, max_levels, GL_RGBA8, width, height);
		}

		// Copy image data into texture
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// ----------------------------------------

		// Delete image data
		delete[] image;
	}

	// ----------------------------------------
	// No Mip-Mapping
	// Configure Texture
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

	// ----------------------------------------
	// Mip-Mapping
	// Configure Texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

	// Generate Mipmap
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	// ----------------------------------------

	// ------------------------------
	// Anistropic Filtering
	// ------------------------------
	// Get Maximum Anistropic level
	GLfloat maxAnistropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnistropy);

	// Enable Anistropic Filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnistropy);


	// Unbind texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texture;
}