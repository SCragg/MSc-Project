// Starter vertex shader for lab3
// THe goal is to update this shader to implement Gourand shading
// which is per-vertex lighting

#version 420

// These are the vertex attributes
layout(location = 0) in vec4 position;
layout(location = 2) in vec4 colour;

// Uniform variables are passed in from the application
layout (std140) uniform Matrices
{
							//base align	//aligned offset
	mat4 projection;		//16 * 4		//0
	mat4 view;				//16 * 4		//64
	mat4 model;				//16 * 4		//128
	mat3 normalmatrix;		//16 * 3		//192

							//total size = 240 bytes
};

// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;

void main()
{	
	//Output colour
	fcolour = colour;
	
	// Define the vertex position
	gl_Position = projection * view * model * position;


}