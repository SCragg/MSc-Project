
#include "thermal_texture1D.h"

//Reads texture data from text tab delimited file and stores in to a vector of unsigned floats
void Thermal_Texture1D::Read_data(std::string filepath)
{
	/*
		Need to write code to read data from file here;

		Will first generate a simple file in order to know what file format to use...

	*/
}

//Generates texture from stored data
void Thermal_Texture1D::Generate_texture()
{
	if (data == nullptr)
	{
		std::cout << "No texture data bound to generate texture." << std::endl;
		return;
	}

	else
	{
		//Gemerate and bind texture
		glGenTextures(1, &tex_ID);
		glBindTexture(GL_TEXTURE_1D, tex_ID);

		//Texture parameters
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/*
				!!!!???   CODE MISSING HERE STILL    !!???
			Need to Read_Data function to read texture data...
			
			Maybe change data to vector if it will work, then create a texture file to test

		*/

		//Generate Texture
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, width, 0, GL_RED, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_1D);
	}
}