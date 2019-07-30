#version 420
#define PI 3.141592653589793

//inputs
in vec3 fNormal;
in vec3 fLightDir;
flat in float flocal_time;

//Uniforms
uniform float global_time;
uniform sampler1D thermaltexture;

out vec4 outputColor;

//Function declarations
vec4 Colour_Greyscale(float temp, float min_val, float max_val);
vec4 Colour_False(float temp, float min_val, float max_val);

//normalize input vectors incase length changed at interpolation
vec3 N = normalize(fNormal);
//vec3 L = normalize(fLightDir);

void main()
{
	

	//Get temp from texture
	float temp = texture(thermaltexture, flocal_time).r;

	//Output colour
	outputColor= Colour_False(temp, 70, 420); //Linear greyscale with 40K as black and 400K as white.
}

//Function definitions

/*
Function used to transform calculated temperature to colour for output
*/
vec4 Colour_Greyscale(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	return vec4(value, value, value, 1);
}

vec4 Colour_False(float temp, float min_val, float max_val)
{
	float value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	float R, G, B;

	//Calculate Red Values
	if (value >= 0.05) R = (1 / 0.25) * (value - 0.05);
	else if(value > 0.3) R = 1;
	else R = 0;

	//Calculate Green Values
	if (value < 0.6) G = 0;
	else if (value <= 0.8) G = (1/0.2) * (value - 0.6);
	else G = 1;

	//Calculate Blue values
	if (value < 0.3) B = (0.5/0.3) * value;
	else if (value <= 0.6) B = (0.5/0.3) * (0.3 - (value - 0.3));
	else if (value >= 0.75) B = (1/0.25) * (value - 0.75);
	else B = 0;





	return vec4(R,G,B,1);
}
