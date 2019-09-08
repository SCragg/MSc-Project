#include "flat_terrain.h"

Flat_terrain::Flat_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize)
	: DEM_terrain(XRes, ZRes, file, XSize, ZSize)
{}

Flat_terrain::~Flat_terrain()
{
	if (dem_data) delete[] dem_data;
	if (vertices) delete[] vertices;
	if (normals) delete[] normals;
	if (colours) delete[] colours;
	if (texture) delete texture;
}


void Flat_terrain::createObject()
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

void Flat_terrain::generate_terrain()
{
	//Check if dem_data is pointing to data;
	if (dem_data == nullptr)
	{
		std::cout << "DEM data has not been loaded... \n";
		return;
	}

	//Arrays for vertex information
	if (vertices) delete vertices;
	if (normals) delete normals;
	if (colours) delete colours;

	vertices = new glm::vec4[numvertices];
	normals = new glm::vec3[numvertices];
	colours = new glm::vec4[numvertices];

	//Starting position and step size
	GLfloat xpos = -X_size / 2.f;
	GLfloat xstep = X_size / X_res;
	GLfloat zpos_start = -Z_size / 2.f;
	GLfloat zstep = Z_size / Z_res;

	int counter = 0;
	//Vertex positions for flat surface
	for (GLuint x = 0; x < X_res; x++)
	{
		GLfloat zpos = zpos_start;
		for (GLuint z = 0; z < Z_res; z++)
		{
			//Add to buffer
			vertices[counter] = glm::vec4(xpos, dem_data[counter], zpos, 1);
			normals[counter] = glm::vec3(0, 0, 0);
			colours[counter] = glm::vec4(0.55, 0.55, 0.55, 1);
			zpos += zstep;
			counter++;
		}
		xpos += xstep;
	}

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

	// Calculate the normals by averaging cross products for all triangles 
	calculateNormals();

	return;
}

void Flat_terrain::drawTerrain(int drawmode)
{
	int size;	// Used to get the byte size of the element (vertex index) array
	glBindVertexArray(vao);

	// Describe our vertices array to OpenGL (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glVertexAttribPointer(
		attrib_v_coord,  // attribute index
		4,                  // number of elements per vertex, here (x,y,z,g)
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
		4,                  // number of elements per vertex, here (x,y,z,g)
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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (drawmode == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/* Draw the triangle strips */
		for (GLuint i = 0; i < X_res - 1; i++)
		{
			GLuint location = sizeof(GLuint) * (i * Z_res * 2);
			glDrawElements(GL_TRIANGLE_STRIP, Z_res * 2, GL_UNSIGNED_INT, (GLvoid*)(location));
		}
	}
	else if (drawmode == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		/* Draw the triangle strips */
		for (GLuint i = 0; i < X_res - 1; i++)
		{
			GLuint location = sizeof(GLuint) * (i * Z_res * 2);
			glDrawElements(GL_TRIANGLE_STRIP, Z_res * 2, GL_UNSIGNED_INT, (GLvoid*)(location));
		}
	}
	else if (drawmode == 2)
	{	/*
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		//glDrawArrays(GL_POINTS, 0, numvertices);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		*/
		/*
		Draw the triangle strips - Less efficient to draw as triangle strips but wanted to rule out element order
		*/
		for (GLuint i = 0; i < X_res - 1; i++)
		{
			GLuint location = sizeof(GLuint) * (i * Z_res * 2);
			glDrawElements(GL_POINTS, Z_res * 2, GL_UNSIGNED_INT, (GLvoid*)(location));
		}
	}
}

/* Calculate normals by using cross products along the triangle strips
   and averaging the normals for each vertex */
void Flat_terrain::calculateNormals()
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