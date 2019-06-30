// Fragment shader
//Started off from the lab3 shader and moved lighting from vertex to frag as shown in the lab

#version 420

in vec4 fcolour;
in vec2 texturecoord;
in vec3 fragN;
in vec3 fragL;
in vec3 fragV;
in vec3 fragR;

out vec4 outputColor;

struct Material
{
sampler2D texture_diffuse1;
sampler2D texture_specular1;
vec4 diffuse_col;
int shininess;
};


//Global variables
int shininess = 10;

void main()
{

	//Define lighting colours
	vec4 specular_col = vec4(1.0f, 0.9f, 0.9f, 1.0f);
	vec4 diffuse_col = fcolour;

	//Renormalise inputs from Vertex shader
	vec3 N = normalize(fragN);
	vec3 L = normalize(fragL);
	vec3 R = normalize(fragR);
	vec3 V = normalize(fragV);

	//ambient calculation
	vec4 ambient = 0.2 * diffuse_col;
	//diffuse calculation
	vec4 diffuse = (max(dot(N, L), 0)) * diffuse_col;
	//specular calculation
	float spec_val = pow(max(dot(R, V), 0.0), shininess);
	vec3 specular;

	specular = specular_col.xyz * spec_val;

	//Outputs
	outputColor = ambient + diffuse;// + (vec4(specular, 1.0) * 0.3);

}