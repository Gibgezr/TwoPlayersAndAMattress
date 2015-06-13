#include "WallEntity.h"																						//Include Box Entity header. 
#include "Game.h"
extern Game *game;

WallEntity *MakeNewWall(float BoxShapeX, float BoxShapeY, b2Vec2 PixelCoords)
{
	WallEntity *NewWall = new WallEntity();																	//Create new, local pointer to Wall object
	NewWall->xScale = BoxShapeX;
	NewWall->yScale = BoxShapeY;

	b2BodyDef WallBodyDef;

	WallBodyDef.position.Set(PixelCoords.x / PTM_RATIO, PixelCoords.y / PTM_RATIO);							//set the position of the center of the body. 
	//Sets in pixel coordinates to physics. 
	WallBodyDef.type = b2_staticBody;																	//Sets the object as kinematic

	NewWall->body = game->world->CreateBody(&WallBodyDef);

	b2PolygonShape WallShape;
	WallShape.SetAsBox(BoxShapeX / (2.f * PTM_RATIO), BoxShapeY / (2.f*PTM_RATIO));

	b2FixtureDef WallFixtureDef;
	WallFixtureDef.shape = &WallShape;
	WallFixtureDef.density = 1.0f;
	WallFixtureDef.restitution = 0;
	WallFixtureDef.friction = 1.f;

	WallFixtureDef.filter.categoryBits = CMASK_WALL;
	WallFixtureDef.filter.maskBits = CMASK_PLAYER | CMASK_ENEMY | CMASK_MATTRESS;

	NewWall->body->CreateFixture(&WallFixtureDef);


	NewWall->sprite = game->defaultSprite;						//Temporary sprite to show location of the object created 

	NewWall->body->SetUserData(NewWall);

	return NewWall;

}