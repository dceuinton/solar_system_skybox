// OpenGL 4.0
#version 400

// Input to Vertex Shader
in vec4 vert_Position;

// Transform Matrices
uniform mat4 u_View;
uniform mat4 u_Projection;

// Output to Fragment Shader
out vec3 frag_str;

void main() {
	//----------------------------------------------
	// Output to Fragment Shader
	//----------------------------------------------

	// Frag STR Vector
	frag_str = normalize(vert_Position.xyz * vec3(-1.0f, -1.0f, 1.0f));
	// frag_str = normalize(vert_Position.xyz);

	//----------------------------------------------
	// Vertex Position
	//----------------------------------------------
	gl_Position = u_Projection * u_View * vert_Position;
}
