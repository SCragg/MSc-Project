/* 
	show_normals.frag - Sean Cragg

*/

#version 400 core

out vec4 colour;

in vec3 fcolour;

void main()
{
	colour = vec4(fcolour, 1.0);
}