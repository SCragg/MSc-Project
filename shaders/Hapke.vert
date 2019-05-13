// Starter vertex shader for lab3
// THe goal is to update this shader to implement Gourand shading
// which is per-vertex lighting

#version 420

// These are the vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 colour;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;
uniform mat3 normalmatrix;
uniform vec4 lightpos;

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;
out float fU;
out float fUo;
out float fg;

void main()
{	

	//Define Model-View matrix
	mat4 mv_matrix = view * model;

	//Transform Vertex Position: P
	vec4 P = mv_matrix * position;

	// Define light direction: L
	vec4 transformed_lightpos = mv_matrix * lightpos;
	vec3 L = lightpos.xyz - P.xyz;

	//view direction
	vec3 V = normalize(-P.xyz);

	//normals
	vec3 N = normalize(normal);
	
	//BRDF parameters for Hapke... are these meant to be calculated from normal or z-axis....?
	fUo = (dot(-L, normal)) / length(-L); //cos i
	fU = (dot(V, normal)); //cos e

	fg = radians(acos(dot(-L.xy, V.xy) / length(-L.xy))); //phase angle

	//Output colour
	fcolour = colour;
	
	// Define the vertex position
	gl_Position = projection * view * model * position;


}