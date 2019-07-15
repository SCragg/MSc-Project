#version 420

//Stefan-Boltzman constant for calculating blackbody radiation: σ = 5.670374419...×10−8 W⋅m^−2⋅K^−4
#define STEFAN_BOLTZMAN 0.00000005670374419
#define PI 3.141592653589793

//inputs
in vec3 fNormal;
in vec3 fLightDir;
in float flocal_time;

//Uniforms
/*
Will need to pass in solar irradiance (solar constant, S in the above equation), the Williams et al paper used a value of 1370 W m^-2.
Could be used a constant, but decided to pass as a uniform as this value will change depending on the distance from the sun.
*/
uniform float solar_constant;

/*
While eventually I would want aldebo to be determined by a texture, for this prototype I will assume all points have the same albedo and will pass it as a uniform
*/
uniform float albedo;

/*
Emissivity value will need to be passed through. From the Vasavada et al paper (2012) it seems as though emissivity is not as varied across points as other thermal properties
so I do not think it would be necessary in the future to include it as a texture or vertex attribute. I will pass it as a uniform as I think it would suffice to assume all points share
the same emissivity, at least for now.
*/
uniform float emissivity;

/*
Thermal texture... 1D texture, initial prototype will use single channel of data
*/
uniform sampler1D thermaltexture;

out vec4 outputColor;

//Function declarations
float Albedo_angle(float theta, float Ao);
float Daytime_temp(float theta, float Ao, float S, float e);
vec4 Colour_Greyscale(float temp, float min_val, float max_val);

//normalize input vectors incase length changed at interpolation
vec3 N = normalize(fNormal);
vec3 L = normalize(fLightDir);

void main()
{
	//Calculate angle of incidence
	//float angle_incidence = acos(clamp(dot(N, L), -1, 1));

	//Calculate temp
	//float temp = Daytime_temp(angle_incidence, albedo, solar_constant, emissivity);

	//Get temp from texture
	float temp = texture(thermaltexture, flocal_time).r;

	//Output colour
	if (flocal_time > 1 || flocal_time < 0)
		outputColor = vec4(1, 0, 1, 1);
	else
		outputColor= Colour_Greyscale(temp, 80, 450); //Linear greyscale with 40K as black and 400K as white.


	//outputColor = vec4(flocal_time, 0, 0, 1);
}

//Function definitions

/*
Albedo approximation from angle of incidence from Vasavada et al (2012)
*/
float Albedo_angle(float theta, float Ao)
{
	const float a = 0.045;
	const float b = 0.14;

	return Ao + a * pow((theta/45), 3) + b * pow((theta/90), 8);
}

/*
Approximation of daylight temperature based on angle of incidence from Williams et al (2017)

Assumes that surface temperature equilibrates with solar flux.
*/
float Daytime_temp(float theta, float Ao, float S, float e)
{
	//if (cos(theta) < 0) //if cos(theta) is negative it is night time on a local basis.
		//return 0;
	//else
		return pow((S * (1 - Albedo_angle(theta, Ao)) * cos(theta) / (e * STEFAN_BOLTZMAN)), (0.25));
}

/*
Function used to transform calculated temperature to colour for output
*/
vec4 Colour_Greyscale(float temp, float min_val, float max_val)
{
	float value;
	value = clamp((temp - min_val)/(max_val - min_val), 0, 1);

	return vec4(value, value, value, 1);
}