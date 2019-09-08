/*
	DEM_terrain.cpp - Sean Cragg
*/

#include "DEM_terrain.h"

DEM_terrain::DEM_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize)
{

	attrib_v_coord = 0;
	attrib_v_normal = 1;
	attrib_v_colour = 2;

	X_res = XRes;
	Z_res = ZRes;
	filepath = file;
	X_size = XSize;
	Z_size = ZSize;
	numvertices = X_res * Z_res;
}

DEM_terrain::~DEM_terrain()
{
	if (dem_data) delete[] dem_data;
	if (vertices) delete[] vertices;
	if (normals) delete[] normals;
	if (colours) delete[] colours;
	if (texture) delete texture;
}


void DEM_terrain::createObject()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Generate the vertex buffer object */
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, numvertices * sizeof(glm::vec4), &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the normals in a buffer object */
	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, numvertices * sizeof(glm::vec3), &(normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the colours in a buffer object */
	glGenBuffers(1, &vbo_colour);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colour);
	glBufferData(GL_ARRAY_BUFFER, numvertices * sizeof(glm::vec4), &(colours[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate a buffer for the indices
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), &(elements[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool DEM_terrain::load_DEM()
{
	if (dem_data != nullptr) return false;

	dem_data = new float[numvertices];

	//Open file
	if (!openFile())
	{
		std::cout << "Could not open DEM file at " << '\'' << filepath << "'. \n";
		return false;
	}

	//read DEM file to dem_data
	char* buffer = new char[4];
	char* value = new char[4];

	int counter = 0;
	for (GLuint x = 0; x < X_res; x++)
	{
		for (GLuint z = 0; z < Z_res; z++)
		{
			//Decode binary integers
			DEMfile.read(buffer, sizeof(value));
			value[0] = buffer[3];
			value[1] = buffer[2];
			value[2] = buffer[1];
			value[3] = buffer[0];
			dem_data[counter] = *(float*)value;
			counter++;
		}
	}

	//Clean up
	delete buffer;
	delete value;
	DEMfile.close();

	return true;
}


//setTexture to 1D texture using width and filepath as inputs, file is text tab delimited file
void DEM_terrain::setTexture(int width, std::string filepath)
{
	if (texture)
	{
		delete texture;
	}

	//Create new 1D texture object with width and filepath
	texture = new Thermal_Texture1D(width, filepath);
	texture->Read_data();
	texture->Generate_texture();

	return;
}


void DEM_terrain::setColour()
{

}

bool DEM_terrain::openFile()
{
	DEMfile.open(filepath, std::ios::binary);
	if (DEMfile.is_open())
		return true;
	else
		return false;
}

/* Calculate normals by using cross products along the triangle strips
   and averaging the normals for each vertex */
void DEM_terrain::calculateNormals()
{
	GLuint element_pos = 0;
	glm::vec3 AB, AC, cross_product;

	// Loop through each triangle strip  
	for (GLuint x = 0; x < X_res - 1; x++)
	{
		// Loop along the strip
		for (GLuint tri = 0; tri < Z_res * 2 - 2; tri++)
		{
			// Extract the vertex indices from the element array 
			GLuint v1 = elements[element_pos];
			GLuint v2 = elements[element_pos + 1];
			GLuint v3 = elements[element_pos + 2];

			// Vertex positions
			glm::vec3 pos1(vertices[v1]);
			glm::vec3 pos2(vertices[v2]);
			glm::vec3 pos3(vertices[v3]);

			// Define the two vectors for the triangle
			AB = pos2 - pos1;
			AC = pos3 - pos1;

			// Calculate the cross product - ensures correct cross product taken each time
			if (tri % 2 == 0) cross_product = glm::normalize(glm::cross(AC, AB));
			else cross_product = glm::normalize(glm::cross(AB, AC));
			

			// Add this normal to the vertex normal for all three vertices in the triangle
			normals[v1] += cross_product;
			normals[v2] += cross_product;
			normals[v3] += cross_product;

			// Move on to the next vertex along the strip
			element_pos++;
		}

		// Jump past the last two element positions to reach the start of the strip
		element_pos += 2;
	}

	// Normalise the normals (this gives us averaged, vertex normals)
	for (GLuint v = 0; v < numvertices; v++)
	{
		normals[v] = glm::normalize(normals[v]);
	}
}