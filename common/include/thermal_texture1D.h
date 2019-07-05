#include <string>
#include <iostream>
#include "wrapper_glfw.h"

class Thermal_Texture1D
{

public:
	const unsigned int & ID() { return tex_ID; }
	void Read_data(std::string filepath);
	void Generate_texture();

private:
	unsigned int tex_ID; //texture ID
	float *data = nullptr; //Data from texture file
	unsigned int width; //Width of texture, ie number of samples

};