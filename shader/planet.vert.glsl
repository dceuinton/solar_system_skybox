#version 410

in vec4 vertexPosition;
in vec4 normalVector;
in vec4 textureCoordinate;

out vec4 normal;
out vec2 UV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
	normal = uView * uModel * normalVector;
	UV = textureCoordinate.xy;
	gl_Position = uProjection * uView * uModel * vertexPosition;
}