//Lambert frag shader

#version 420

in vec4 fcolour;
in vec3 fV;
in vec3 fN;
in vec3 fL;

out vec4 outputColor;

vec3 N = normalize(fN);
vec3 L = normalize(fL);

vec4 Colour_Rainbow(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	float R, G, B;

	//Calculate Red values
	if (value < 0.222) R = -(value / 0.222) + 1;
	else if (value < 0.333) R = 0;
	else if (value < 0.556) R = (value-0.333) / 0.222;
	else if (value < 0.667) R = 1;
	else if (value < 0.889) R = -0.5*((value-0.667) / 0.222) + 1;
	else R = 0.5*((value - 0.889) / 0.111) + 0.5;

	//Calculate Green values
	if (value < 0.1667) G = -0.33*(value / 0.1667) + 0.33;
	else if (value < 0.2778) G = (value - 0.1667) / 0.1111;
	else if (value < 0.444) G = 1;
	else if (value < 0.667) G = -((value - 0.444) / 0.223) + 1;
	else if (value < 0.889) G = 0;
	else G = (value - 0.889)/0.111;

	//Calculate Blue values
	if (value < 0.222) B = 1;
	else if (value < 0.444) B = -((value - 0.222) / 0.222) + 1;
	else if (value < 0.889) B = 0;
	else B = (value - 0.889)/0.111;

	return vec4(R,G,B,1);
}

void main()
{
	outputColor = vec4(1,1,1,1) * max(0.0, dot(N, L));

}

