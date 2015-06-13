#pragma once

#include "Entity.h"
class Entity;
class EdgeEntity : public Entity
{
public:
	EdgeEntity()
	{
		typeID = ENTITYEDGE;
	}

	void Draw() {}; //empty draw, as edges do not draw
};