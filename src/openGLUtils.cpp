#include "openGLUtils.h"

// Called on Error Event
void onError(int error, const char *description) {
	// Print Error message
	std::cerr << "Error: " << error << " : " << description << std::endl;
}

// Called on Window Close Event
void onWindowClose(GLFWwindow *window) {
	// Nothing to do right now
	// Do not call glfwDestroyWindow from here
}

// Called on Window Size Event
void onFramebufferSize(GLFWwindow *window, int width, int height) {
	// Set-up the window/screen coordinates
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	glfwMakeContextCurrent(NULL);
}

// --------------------------------------------------------------------------------
// GLFW Functions
// --------------------------------------------------------------------------------

// Create a GLFW Window
GLFWwindow* createWindow(int width, int height, const char *title, int major, int minor, GLFWmonitor *monitor, GLFWwindow *share) {
	// Request an OpenGL context with specific features
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

	// If Version is 3 or higher
	if (major >= 3) {
		// Request Forward Compatibility
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		// If version is 3.2 or higher
		if (major > 3 || minor >= 2) {
			// Request Core Profile
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
	}

	// Create GLFW Window
	GLFWwindow *window = glfwCreateWindow(width, height, title, monitor, share);

	// Check Returned Window
	if (window == NULL) {
		return NULL;
	}

	// Set Context
	glfwMakeContextCurrent(window);

	// Set window callback functions
	glfwSetFramebufferSizeCallback(window, onFramebufferSize);
	glfwSetWindowCloseCallback(window, onWindowClose);

	// Return GLFW window
	return window;
}

// --------------------------------------------------------------------------------
// Shader Functions
// --------------------------------------------------------------------------------

// Read file contents
char* readFile(const char *filename) {
	// Open File
	std::ifstream input(filename);

	// Check file is open
	if(!input.good()) {
		// Print Error
		std::cerr << "Error: Could not open " << filename << std::endl;

		// Return Error
		return 0;
	}

	// Find end of file
	input.seekg(0, std::ios::end);

	// Calculate Size
	size_t size = input.tellg();

	// Allocate required memory
	char *data = new char[size+1];

	// Rewind to beginning
	input.seekg(0, std::ios::beg);

	// Read file into memory
	input.read(data, size);

	// Append '\0'
	data[size] = '\0';

	// Close file
	input.close();

	// Return file contents
	return data;
}

// Check the status of a Shader
GLuint checkShader(GLuint shader) {
	// Compile status
	GLint status = 0;

	// Check compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	// Error detected
	if(status != GL_TRUE) {
		// Get error message length
		int size;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

		// Get error message
		char *message = new char[size];
		glGetShaderInfoLog(shader, size, &size, message);

		// Print error message
		std::cerr << message << std::endl;

		// Delete message
		delete[] message;

		// Return error
		return GL_FALSE;
	}

	// Return success
	return GL_TRUE;
}

// Check the status of a Program
GLuint checkProgram(GLuint program) {
	// Link status
	GLint status = 0;

	// Check link status
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	// Error detected
	if(status != GL_TRUE) {
		// Get error message length
		int size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);

		// Get error message
		char *message = new char[size];
		glGetProgramInfoLog(program, size, &size, message);

		// Print error message
		std::cerr << message << std::endl;

		// Delete message
		delete[] message;

		// Return error
		return GL_FALSE;
	}

	// Return success
	return GL_TRUE;
}

// Load and Compiler Shader for source file
GLuint loadShader(GLuint type, const char *filename) {
	// Read the shader source from file
	char *source = readFile(filename);

	// Check shader source
	if(source == 0) {
		// Return Error
		return 0;
	}

	// Create the OpenGL Shaders
	GLuint shader = glCreateShader(type);

	// Load the source into the shaders
	glShaderSource(shader, 1, &source, NULL);

	// Compile the Shaders
	glCompileShader(shader);

	// Check shaders for errors
	if(checkShader(shader) == GL_TRUE) {
		// Log
		std::cout << "Loaded: " << filename << std::endl;
	} else {
		// Print Error
		std::cerr << "Error: could not compile " << filename << std::endl;

		// Delete shader source
		delete[] source;

		// Return Error
		return 0;
	}

	// Delete shader source
	delete[] source;

	// Return shader
	return shader;
}

GLuint loadProgram(const char *vert_file, const char *ctrl_file, const char *eval_file, const char *geom_file, const char *frag_file) {
	// Create new OpenGL program
	GLuint program = glCreateProgram();

	// Shader Handles
	GLuint vert_shader = 0;
	GLuint ctrl_shader = 0;
	GLuint eval_shader = 0;
	GLuint geom_shader = 0;
	GLuint frag_shader = 0;

	// Load Shaders
	if(vert_file != NULL) vert_shader = loadShader(GL_VERTEX_SHADER,          vert_file);
	if(ctrl_file != NULL) ctrl_shader = loadShader(GL_TESS_CONTROL_SHADER,    ctrl_file);
	if(eval_file != NULL) eval_shader = loadShader(GL_TESS_EVALUATION_SHADER, eval_file);
	if(geom_file != NULL) geom_shader = loadShader(GL_GEOMETRY_SHADER,        geom_file);
	if(frag_file != NULL) frag_shader = loadShader(GL_FRAGMENT_SHADER,        frag_file);

	// Attach shaders
	if(vert_shader != 0) glAttachShader(program, vert_shader);
	if(ctrl_shader != 0) glAttachShader(program, ctrl_shader);
	if(eval_shader != 0) glAttachShader(program, eval_shader);
	if(geom_shader != 0) glAttachShader(program, geom_shader);
	if(frag_shader != 0) glAttachShader(program, frag_shader);

	// Check Vertex Shader
	if(vert_shader == 0) {
		// Print Error
		std::cerr << "Error: program missing vertex shader." << std::endl;

		// Delete Shaders
		if(vert_shader != 0) glDeleteShader(vert_shader);
		if(ctrl_shader != 0) glDeleteShader(ctrl_shader);
		if(eval_shader != 0) glDeleteShader(eval_shader);
		if(geom_shader != 0) glDeleteShader(geom_shader);
		if(frag_shader != 0) glDeleteShader(frag_shader);

		// Return Error
		return 0;
	}

	// Check Fragment Shader
	if(frag_shader == 0) {
		// Print Error
		std::cerr << "Error: program missing fragment shader." << std::endl;

		// Delete Shaders
		if(vert_shader != 0) glDeleteShader(vert_shader);
		if(ctrl_shader != 0) glDeleteShader(ctrl_shader);
		if(eval_shader != 0) glDeleteShader(eval_shader);
		if(geom_shader != 0) glDeleteShader(geom_shader);
		if(frag_shader != 0) glDeleteShader(frag_shader);

		// Return Error
		return 0;
	}

	// Link program
	glLinkProgram(program);

	// Delete Shaders (no longer needed)
	if(vert_shader != 0) glDeleteShader(vert_shader);
	if(ctrl_shader != 0) glDeleteShader(ctrl_shader);
	if(eval_shader != 0) glDeleteShader(eval_shader);
	if(geom_shader != 0) glDeleteShader(geom_shader);
	if(frag_shader != 0) glDeleteShader(frag_shader);

	// Check program for errors
	if(checkProgram(program) == GL_TRUE) {
		// Print Log
		std::cout << "Loaded: program" << std::endl;
	} else {
		// Print Error
		std::cerr << "Error: could not link program" << std::endl;

		// Return Error
		return 0;
	}

	// Return program
	return program;
}

void printMat4(float m[16]) {
	for (int i = 0; i < 4; i++) {
			std::cout 
			<< m[0 + i] << ", " 
			<< m[4 + i] << ", " 
			<< m[8 + i] << ", " 
			<< m[12 + i] << ", " 
			<< std::endl;
	}
}

void printGLfloatVector(std::vector<GLfloat>& vec) {
	int i = 0;
	while (i < vec.size()) {
		std::cout << vec[i] << std::endl;
		i++;
	}
}

void printGLuintVector(std::vector<GLuint>& vec) {
	int i = 0;
	while (i < vec.size()) {
		std::cout << vec[i] << std::endl;
		i++;
	}
}

void printZValues(std::vector<GLfloat>& vec) {
	int k = 2;
	while (k < vec.size()) {
		std::cout << vec[k] << std::endl;
		k += 4;
	}
}