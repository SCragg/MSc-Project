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
out float flocal_time;
out float latitude;

//Main
void main()
{
	//Transform normal and light direction and send to frag shader
	fNormal = normalize(normal);
	fLightDir = normalize(normalmatrix * lightdir.xyz);
	latitude = acos(dot(normalize(position.xyz), normalize(vec3(position.x, position.y, 0))));

	//Output vertex position
	gl_Position = projection * view * model * position;
}







