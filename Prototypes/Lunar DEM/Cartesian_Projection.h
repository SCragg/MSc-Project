#pragma once

class Cartesian_Projection : public Projection
{
public:

	void Project_DEM(DEM_terrain*) override;
};

