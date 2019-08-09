#pragma once

#define GLM_SWIZZLE

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "wrapper_glfw.h"
#include "thermal_texture1D.h"

class DEM_terrain
{
public:

	DEM_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	~DEM_terrain();

	virtual void createObject() = 0;
	bool load_DEM();
	virtual void generate_terrain() = 0;
	void generateTerrain_flat();
	void generateTerrain_sphere();
	virtual void drawTerrain(int drawmode) = 0;
	void setTexture(int width, std::string filepath);

	void setColour();

protected:

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

	//Thermal Texture Object
	Thermal_Texture1D* texture = nullptr;

	//Functions
	bool openFile();
	virtual void calculateNormals() = 0;
};
