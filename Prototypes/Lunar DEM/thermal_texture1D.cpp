
#include "thermal_texture1D.h"

//Constructor
Thermal_Texture1D::Thermal_Texture1D(unsigned int w, std::string path):
	width(w), filepath(path) {}

//Reads texture data from text tab delimited file and stores in to a vector of unsigned floats
void Thermal_Texture1D::Read_data()
{
	//Ensure data is pointing to nothing to prevent memory leaks
	if (data != nullptr)
		delete [] data;

	//create array of size width to store data
	data = new float[width];

	//create and open file stream
	std::ifstream inputfile;
	inputfile.open(filepath);

	//check if stream has correctly opened
	if (!inputfile.is_open())
	{
		std::cout << "Error could not open file, ensure filepath is correctly set." << std::endl;
		return;
	}

	else
	{
		for (int i = 0; i < width; i++)
		{
			inputfile >> data[i];

		}
		inputfile.close();
	}

	return;
}

//Generates texture from stored data, ensure data has already been loaded from a text tabdelimited file using Read_data() function;
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
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Generate/Load Texture
		if (data)
		{
			glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, width, 0, GL_RED, GL_FLOAT, data);
			glGenerateMipmap(GL_TEXTURE_1D);
		}
		else
		{
			std::cout << "failed to load texture... \n";
		}

		//Clean up data
		delete[] data;
		data = nullptr;
		return;
	}
}