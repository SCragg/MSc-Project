// Starter vertex shader for lab3
// THe goal is to update this shader to implement Gourand shading
// which is per-vertex lighting

#version 420

// These are the vertex attributes
layout(location = 0) in vec4 position;
layout(location = 2) in vec4 colour;

// Uniform variables are passed in from the application
uniform mat4 model, view, projection;


// Output the vertex colour - to be rasterized into pixel fragments
out vec4 fcolour;

void main()
{	
	//Output colour
	fcolour = colour;
	
	// Define the vertex position
	gl_Position = projection * view * model * position;


}