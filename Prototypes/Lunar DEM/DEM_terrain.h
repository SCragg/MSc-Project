#pragma once

#define GLM_SWIZZLE

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "wrapper_glfw.h"
#include "Projection.h"

class DEM_terrain
{

public:

	DEM_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	~DEM_terrain();

	void createObject();
	bool load_DEM();
	void generateTerrain_flat();
	void generateTerrain_sphere();
	void drawTerrain(int drawmode);
	void setColour();

private:

	//Dimensions
	GLuint X_res, Z_res;
	GLfloat X_size, Z_size;

	//DEM file
	std::string filepath;
	std::ifstream DEMfile;
	float* dem_data;

	//Vertex Data
	GLuint numvertices;
	glm::vec4* vertices = nullptr;
	glm::vec3* normals = nullptr;
	glm::vec4* colours = nullptr;
	std::vector<GLuint> elements;

	//Vertex Buffer Indices
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_colour;
	GLuint ibo_elements;
	GLuint attrib_v_coord;
	GLuint attrib_v_normal;
	GLuint attrib_v_colour;

	//Projections
	Projection* projection = nullptr;
	friend void Cartesian_Projection::Project_DEM(DEM_terrain* dem);

	//Functions
	bool openFile();
	void calculateNormals();
};
