#include "Cartesian_Projection.h"
#include "DEM_terrain.h"

void Cartesian_Projection::Project_DEM(DEM_terrain* dem)
{
	//Starting position and step size
	GLfloat xpos = -dem->X_size / 2.f;
	GLfloat xstep = dem->X_size / dem->X_res;
	GLfloat zpos_start = -dem->Z_size / 2.f;
	GLfloat zstep = dem->Z_size / dem->Z_res;

	int counter = 0;
	//Vertex positions for flat surface
	for (GLuint x = 0; x < dem->X_res; x++)
	{
		GLfloat zpos = zpos_start;
		for (GLuint z = 0; z < dem->Z_res; z++)
		{
			//Add to buffer
			dem->vertices[x * dem->X_res + z] = glm::vec4(xpos, dem->dem_data[counter], zpos, 1);
			dem->normals[x * dem->X_res + z] = glm::vec3(0, 0, 0);
			dem->colours[x * dem->X_res + z] = glm::vec4(0.55, 0.55, 0.55, 1);
			zpos += zstep;
			counter++;
		}
		xpos += xstep;
	}
}