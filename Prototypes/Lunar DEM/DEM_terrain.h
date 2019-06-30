#pragma once

#define GLM_SWIZZLE

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "wrapper_glfw.h"

class DEM_terrain
{

public:

	DEM_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	~DEM_terrain();

	void createObject();
	void generateTerrain();
	void drawTerrain(int drawmode);
	void setColour();

private:

	//Dimensions
	GLuint X_res, Z_res;
	GLfloat X_size, Z_size;

	//DEM file
	std::string filepath;
	std::ifstream DEMfile;

	//Vertex Data
	GLuint numvertices;
	glm::vec4* vertices;
	glm::vec3* normals;
	glm::vec4* colours;
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

	//Functions
	bool openFile();
	void calculateNormals();
};
