#pragma once

#include"DEM_terrain.h"

class Flat_terrain : public DEM_terrain
{
public:
	Flat_terrain(GLuint XRes, GLuint ZRes, std::string file, GLfloat XSize, GLfloat ZSize);
	~Flat_terrain()override;
	virtual void createObject() override;
	virtual void generate_terrain() override;
	virtual void drawTerrain(int drawmode) override;

protected:
	virtual void calculateNormals();
};
