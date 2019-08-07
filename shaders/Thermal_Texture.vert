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
//out vec4 fcolour;
out float flocal_time;

//Main
void main()
{
	//Transform normal and light direction and send to frag shader
	fNormal = normalize(normal);
	fLightDir = normalize(normalmatrix * lightdir.xyz);

	/*
	//Calculations for local time
	float local_slope;
	local_slope = tan(acos(dot(normalize(normal), vec3(0,1,0))));

	//remove height and normalise
	vec2 heading_vec = vec2(normalize(normal).x, normalize(normal).z);
	normalize(heading_vec);

	//calcuate slope azimuth angle
	float azimuth = atan(heading_vec.x, heading_vec.y);
	if (azimuth < 0)
	{
		azimuth = 6.28318530718 + azimuth; // convert to range 0 - 2pi
	}

	//corrction equation
	float local_time = global_time + (0.5 / PI) * atan(local_slope * sin(azimuth));

	//correct value to range 0 - 1 and output to frag shader
	if (local_time < 0)
		flocal_time = local_time + 1;
	else if (local_time > 1)
		flocal_time = local_time - 1;
	else
		flocal_time = local_time;
	*/

	/*
		Below are lines of code for debugging purposes, enable to help with debugging
	*/

	//flocal_time = global_time;

	//Highlights if time
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







