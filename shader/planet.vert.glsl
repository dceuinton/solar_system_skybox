#version 410

in vec4 vertexPosition;
in vec4 normalVector;
in vec4 textureCoordinate;

out vec2 UV;
out vec4 fragPosition;
out vec4 fragNormal;
out vec4 lightDirection;

uniform vec4 lightSource = vec4(0.0f, 0.0f, 0.0f, 0.0f);

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {

	fragPosition = uView * uModel * vertexPosition;

	fragNormal = uView * uModel * normalVector;

	lightDirection = uView * (fragPosition - lightSource);

	UV = textureCoordinate.xy;

	gl_Position = uProjection * uView * uModel * vertexPosition;
}