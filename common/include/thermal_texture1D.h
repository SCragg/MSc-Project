/*
thermal_texture1D.h - Sean Cragg

Class for 1d thermal textures, has functions to load texture from tab delimited file.

Must know a priori number of data points in texture file and filepath when creating object.
*/

#include <string>
#include <iostream>
#include <fstream>
#include "wrapper_glfw.h"

class Thermal_Texture1D
{

public:
	Thermal_Texture1D(unsigned int w, std::string path);
	~Thermal_Texture1D();
	unsigned int & ID() { return tex_ID; }
	void Read_data();
	void Generate_texture();

private:
	unsigned int tex_ID; //texture ID
	float *data = nullptr; //Data from texture file
	unsigned int width; //Width of texture, ie number of samples
	std::string filepath; //filepath of texture

};