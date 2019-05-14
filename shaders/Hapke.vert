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
out vec3 fV;
out vec3 fN;
out vec3 fL;

void main()
{	

	//Define Model-View matrix
	mat4 mv_matrix = view * model;

	//Transform Vertex Position: P
	vec4 P = mv_matrix * position;

	// Define light direction: L
	vec4 transformed_lightpos = mv_matrix * lightpos;

	//Output vectors to vertex shader
	fL = normalize(lightpos.xyz - P.xyz);
	fV = normalize(-P.xyz);
	fN = normalize((normal * normalmatrix));

	//Output colour
	fcolour = colour;
	
	// Define the vertex position
	gl_Position = projection * view * model * position;

	//BRDF parameters for Hapke... are these meant to be calculated from normal or z-axis....?
	//fUo = (dot(-L, normal)) / length(-L); //cos i
	//fU = (dot(V, normal)); //cos e

	//fg = radians(acos(dot(L.xy, V.xy) / length(L.xy))); //phase angle
}