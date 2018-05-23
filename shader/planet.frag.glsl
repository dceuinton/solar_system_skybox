#version 410

in vec4 normal;
in vec2 UV;

out vec4 colour;

void main() {
	colour = normal;
}