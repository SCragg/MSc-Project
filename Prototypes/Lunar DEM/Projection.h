#pragma once

#include "Cartesian_Projection.h"
#include "ReverseCylindrical_Projection.h"

class Projection
{
public:
	virtual void Project_DEM(DEM_terrain* dem) = 0;
};
