/*
Thermal_1.vert

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
#define PI 3.141592653589793

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
Uniform global time needed to calculate local time. 
*/
uniform float global_time;

/*
Outputs to fragment shader - colour, normal, light direction
*/
out vec3 fNormal;
out vec3 fLightDir;
out vec4 fcolour;
//out float local_time;

//Main
void main()
{
	//Transform normal and light direction and send to frag shader
	fNormal = normalize(normalmatrix * normal);
	fLightDir = normalize(normalmatrix * lightdir.xyz);
	
	//Calculations for local time
	float local_slope;
	//local_slope = acos(dot(normal, vec3(0,1,0))) / (PI/2);
	local_slope = tan(acos(dot(normal, vec3(0,1,0))));
	//remove height and normalise
	vec2 heading_vec = vec2(normal.x, normal.z);
	normalize(heading_vec);

	//calcuate slope azimuth angle
	float azimuth = atan(heading_vec.x, heading_vec.y);
	if (azimuth < 0)
	{
		azimuth = 6.28318530718 + azimuth; // convert to range 0 - 2pi
	}

	//corrction equation
	float local_time = global_time + (0.5 / PI) * atan(local_slope * sin(azimuth));

	//correct value to range 0 - 1
	if (local_time < 0)
		local_time += 1;
	else if (local_time > 1)
		local_time -= 1;

	if (local_time < 0 || local_time > 1)
		{
			fcolour = vec4(1, 0, 0, 1);
		}
	else
		{
			fcolour = vec4(local_time, local_time, local_time, local_time);
		}
	


	//fcolour = vec4(azimuth/6.28318530718, azimuth/6.28318530718, azimuth/6.28318530718, azimuth/6.28318530718);

	//Output vertex position
	gl_Position = projection * view * model * position;
}







