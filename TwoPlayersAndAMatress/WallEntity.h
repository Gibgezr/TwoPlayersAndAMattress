#pragma once


#include "Entity.h"

#include "CollisionMask.h"

class WallEntity : public Entity
{
public:
	float xScale, yScale;
	WallEntity()
	{
		typeID = ENTITYWALL;
	}

	void Draw()
	{
		position = body->GetPosition();
		position = Physics2Pixels(position);

		sprite->angle = body->GetAngle();

		// Draw the objects
		sprite->Blit(position.x , position.y, xScale, yScale );
	}
};

WallEntity * MakeNewWall(float BoxShapeX, float BoxShapeY, b2Vec2 PixelCoords);
