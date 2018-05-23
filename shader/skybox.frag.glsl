// OpenGL 4.0
#version 400

// Input from Vertex Shader
in vec3 frag_str;

// Texture
uniform samplerCube u_texture_Map;

// Output from Fragment Shader
out vec4 pixel_Colour;

void main () {
	//----------------------------------------------
	// Fragment Colour
	//----------------------------------------------
	pixel_Colour = texture(u_texture_Map, frag_str);
}