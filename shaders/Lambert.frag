//Lambert frag shader

#version 420

in vec4 fcolour;
in vec3 fV;
in vec3 fN;
in vec3 fL;

out vec4 outputColor;

vec3 N = normalize(fN);
vec3 L = normalize(fL);

void main()
{
	outputColor = fcolour * max(0.0, dot(N, L));
}

