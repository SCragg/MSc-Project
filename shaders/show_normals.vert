/* 
	show_normals.vert - Sean Cragg
	Veretx shader to use to draw object showing vertex normals

*/

#version 400

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 vnormal;

void main()
{
	vnormal = normal;
	
	gl_Position = vec4(position);
}