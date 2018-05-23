#version 410

in vec4 normal;
in vec2 UV;

uniform sampler2D uTexture;

out vec4 colour;

void main() {
	colour = texture(uTexture, UV);
}