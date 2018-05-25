#version 410

in vec4 fragPosition;
in vec4 fragNormal;
in vec4 lightDirection;
in vec2 UV;

// Light source
uniform vec4 Ia = vec4(0.25f, 0.25f, 0.25f, 1.0f);
uniform vec4 Id = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform vec4 Is = vec4(1.0f, 1.0f, 1.0f, 1.0f);

// Material values
// uniform vec4 Ka = vec4(0.329412, 0.223529, 0.027451, 1.0);
// uniform vec4 Kd = vec4(0.780392, 0.568627, 0.113725, 1.0);
// uniform vec4 Ks = vec4(0.992157, 0.941176, 0.807843, 1.0);

uniform vec4 Ka = vec4(1.0, 0.90, 0.90, 1.0);
uniform vec4 Kd = vec4(0.64, 0.64, 0.64, 1.0);
uniform vec4 Ks = vec4(0.0, 0.0, 0.0, 1.0);
uniform float a = 3.0;

uniform sampler2D uTexture;

out vec4 colour;

void main() {

	vec4 l = normalize(lightDirection);

	vec4 n = normalize(fragNormal);

	vec4 r = reflect(-l, n);

	vec4 v = normalize(-fragPosition);

	vec4 KaValue = Ka;
	vec4 KdValue = Kd;
	vec4 KsValue = Ks;

	KaValue = texture(uTexture, UV);
	KdValue = Ka; 
	KsValue = Ks; 

	vec4 Ta = KaValue * Ia;

	vec4 Td = KdValue * max(dot(l, n), 0.0f) * Id;

	vec4 Ts = KsValue * pow(max(dot(r, v), 0.0), a) * Is;

	colour = Ta + Td + Ts;
}