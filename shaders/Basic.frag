// Fragment shader
//Started off from the lab3 shader and moved lighting from vertex to frag as shown in the lab

#version 420

in vec4 fcolour;

out vec4 outputColor;

void main()
{

	//Outputs
	outputColor = fcolour;

}