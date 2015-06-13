#include "LevelWinEntity.h"
#include "Game.h"
extern Game *game;

LevelWinEntity::LevelWinEntity(float pixelX, float pixelY)
{
	typeID = ENTITYLEVELWIN;
	position = Pixels2Physics(b2Vec2(pixelX, pixelY));
	b2BodyDef bodyDef;
	bodyDef.position = position;
	bodyDef.type = b2_staticBody;

	body = game->world->CreateBody(&bodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2CircleShape circleShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	circleShape.m_radius = 32.f / PTM_RATIO;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 0.1;
	fixtureDef.friction = 0.5f;

	//collison masking
	fixtureDef.filter.categoryBits = CMASK_LEVELWIN;  //this 
	fixtureDef.filter.maskBits = CMASK_PLAYER | CMASK_MATTRESS; //it collides wth a few things

	body->CreateFixture(&fixtureDef);

	//make the userdata point back to this entity
	body->SetUserData(this);

	sprite = game->defaultSprite;
}

void LevelWinEntity::Draw() 
{
	position = body->GetPosition();
	position = Physics2Pixels(position);

	sprite->angle = body->GetAngle();

	// Draw the objects
	sprite->Blit(position.x , position.y, 32, 32 );
}; //empty draw