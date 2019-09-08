/*
Thermal_1.vert - Sean Cragg

Prototype for thermal rendering, this will be for the current loaded model which is 512x512
model projected on to a plane.

Initial prototype will carry out calculations in the vertex shader, however I would like to move this to the fragment shader to enable the use of textures in the future
for albedo maps and something to capture thermal properties such a.

This will use the approximation from Williams et al (2017), The global surface temperatures of the moon as measured by the diviner lunar radiometer experiment:

	T(θ) = (S(1 - A)cos(θ)/εσ)^1/4

	Where:
	θ is angle of incidence.
	S is solar constant.
	ε is emissivity.
	σ is the Stefan-Boltzmann constant used to calculate blackbody radiation.
	A is albedo, they used the approximation of albedo dependant on inclination defined by Vasavada et al (2012):

	A(θ) = Ao + a(θ/45)^3 + b (θ/90)^8
	
	Where:
	Ao is the albedo at 0 degrees inclination, they used Ao = 0.08.
	a and b are constants, 0.045 and 0.14 respectively


*/
#version 420

//Stefan-Boltzman constant for calculating blackbody radiation: σ = 5.670374419...×10−8 W⋅m^−2⋅K^−4
#define STEFAN_BOLTZMAN 0.00000005670374419

// These are the vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 colour; //this could be used to contain albedo Ao values, but for now I will assume all vertices have same albedo and use a uniform.

// Uniform variables which are passed in from the application to calculate temperature

//Essential matrices for rendering
layout (std140) uniform Matrices
{
							//base align	//aligned offset
	mat4 projection;		//16 * 4		//0
	mat4 view;				//16 * 4		//64
	mat4 model;				//16 * 4		//128
	mat3 normalmatrix;		//16 * 3		//192
};

/*
Will need some form of time passed in to the shader, whether or not it should be passed in as time or light direction I am still not so sure, but as the daytime approximation
uses angle of incidence which will also capture the effect of latitude, I will calculate light direction from time and solar declination in the application and pass it as a uniform.
In the future when night temperature approximation becomes more refined I may also need time as a uniform.
*/
uniform vec4 lightdir;

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
Initial prototype will calculate in the vertex shader therefore will only output colour.
In the future when calculating in the fragment shader outputs will need to be, normal, light direction and the above uniforms can be used from the fragment shader.
*/
out vec4 fcolour;

//Function declarations
float Albedo_angle(float theta, float Ao);
float Daytime_temp(float theta, float Ao, float S, float e);
vec4 Colour_Greyscale(float temp, float min_val, float max_val);

//Main
void main()
{
	//Transform normal
	vec3 N = normalize(normalmatrix * normal);
	vec3 L = normalize(normalmatrix * lightdir.xyz);

	//Calculate angle of incidence
	float angle_incidence = acos(clamp(dot(N, L), -1, 1));

	//Calculate temp
	float temp = Daytime_temp(angle_incidence, albedo, solar_constant, emissivity);

	//Output colour
	fcolour = Colour_Greyscale(temp, 80, 400); //Linear greyscale with 40K as black and 400K as white.
	


	//Output vertex position
	gl_Position = projection * view * model * position;
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






