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
	//Insert clean up in here...
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

void DEM_terrain::generateTerrain()
{
	//Arrays for vertex information
	vertices = new glm::vec4[numvertices];
	normals = new glm::vec3[numvertices];
	colours = new glm::vec4[numvertices];

	//Starting position and step size
	GLfloat xpos = -X_size / 2.f;
	GLfloat xstep = X_size / X_res;
	GLfloat zpos_start = -Z_size / 2.f;
	GLfloat zstep = Z_size / Z_res;

	//Open file
	if (!openFile())
	{
		std::cout << "Could not open DEM file at " << '\'' << filepath << "'. \n";
		return;
	}
	else
	{
		GLfloat height;
		//Vertex positions for flat surface
		for (GLuint x = 0; x < X_res; x++)
		{
			GLfloat zpos = zpos_start;
			for (GLuint z = 0; z < Z_res; z++)
			{
				DEMfile >> height;
				vertices[x * X_res + z] = glm::vec4(xpos, height, zpos, 1);
				normals[x * X_res + z] = glm::vec3(0, 1, 0);
				colours[x * X_res + z] = glm::vec4(0.5, 0.5, 0.5, 1);
				zpos += zstep;
			}
			xpos += xstep;
		}
		DEMfile.close(); //close file stream

		/* Define vertices for triangle strips */
		for (GLuint x = 0; x < X_res - 1; x++)
		{
			GLuint top = x * Z_res;
			GLuint bottom = top + Z_res;
			for (GLuint z = 0; z < Z_res; z++)
			{
				elements.push_back(top++);
				elements.push_back(bottom++);
			}
		}
		return;
	}
}

void DEM_terrain::drawTerrain()
{
	int size;	// Used to get the byte size of the element (vertex index) array
	glBindVertexArray(vao);

// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glVertexAttribPointer(
		attrib_v_coord,  // attribute index
		4,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);
	glEnableVertexAttribArray(attrib_v_coord);

	// Describe our colours array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colour);
	glVertexAttribPointer(
		attrib_v_colour,  // attribute index
		4,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);
	glEnableVertexAttribArray(attrib_v_colour);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glVertexAttribPointer(
		attrib_v_normal, // attribute
		3,                  // number of elements per vertex, here (x,y,z)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);
	glEnableVertexAttribArray(attrib_v_normal);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	// Enable this line to show model in wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* Draw the triangle strips */
	for (GLuint i = 0; i < X_res - 1; i++)
	{
		GLuint location = sizeof(GLuint) * (i * Z_res * 2);
		glDrawElements(GL_TRIANGLE_STRIP, Z_res * 2, GL_UNSIGNED_INT, (GLvoid*)(location));
	}
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

void DEM_terrain::calculateNormals()
{

}