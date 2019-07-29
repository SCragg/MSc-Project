/*
Thermal_Texture.vert
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
out float flocal_time;

//Main
void main()
{
	//Transform normal and light direction and send to frag shader
	fNormal = normalize(normal);
	fLightDir = normalize(normalmatrix * lightdir.xyz);

	/*
		Below are lines of code for debugging purposes, enable to help with debugging
	*/

	//flocal_time = global_time;

	/*
	if (flocal_time < 0 || flocal_time > 1)
		{
			fcolour = vec4(1, 0, 0, 1);
		}
	else
		{
			fcolour = vec4(flocal_time, flocal_time, flocal_time, 1);
		}
	*/


	//colour vertices by azimuth to test correct azimuth calculation
	//fcolour = vec4(azimuth/6.28318530718, azimuth/6.28318530718, azimuth/6.28318530718, 1);

	//colour vertices by gradient to test correct gradient calculation
	//fcolour = vec4(local_slope, local_slope, local_slope, 1);

	//Output vertex position
	gl_Position = projection * view * model * position;
}







