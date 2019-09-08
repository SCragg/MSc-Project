/*
	sphere_terrain.h - Sean Cragg

	Derived class of DEM_terrain.
	
	Use to generate spherical model from DEM.
	Uses reverse cylindrical projection to generate spherical model

	KNOWN ISSUES:
	There is a missing triangle strip causing a seam in the model, add element array indices and fix draw call
	tried in the past but couldn't get it to work.

	top and bottom vertices need fixed, but at high resolution unnoticeable
*/

#pragma once

#include"DEM_terrain.h"

class Sphere_terrain : public DEM_terrain
{
public:
	Sphere_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	~Sphere_terrain() override;
	virtual void createObject() override;
	virtual void generate_terrain() override;
	virtual void drawTerrain(int drawmode) override;

protected:
	virtual void calculateNormals();
};
