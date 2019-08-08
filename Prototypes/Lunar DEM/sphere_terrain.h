#pragma once

#include"DEM_terrain.h"

class Sphere_terrain : public DEM_terrain
{
public:
	Sphere_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	virtual void createObject() override;
	virtual void generate_terrain() override;
	virtual void drawTerrain(int drawmode) override;

protected:
	virtual void calculateNormals();
};
