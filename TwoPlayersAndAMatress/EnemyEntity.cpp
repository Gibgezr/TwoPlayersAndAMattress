#include "EnemyEntity.h"
#include "Game.h"
extern Game *game;


EnemyEntity::EnemyEntity(float pixelX, float pixelY, EnemyType type)
{
	typeID = ENTITYENEMY;
	moving = b2Vec2(0.f, 0.f);
	angle = 90;
	currentFrame = 1;
	timePerFrame = 0.1f;
	frameTime = 0.f;
	pingpongDir = 1;
	animating = false;
	x_scale = y_scale = 2.5f;
		
	enemyType = type;
	switch (enemyType)
	{
	case EnemyType::SHOOTER:
		currentSpeed = 50.f;
		break;

	default:
		currentSpeed = 40.f;
		break;
	}


	sprite = game->spriteList[0]; //fix this

	//physics body
	b2BodyDef bodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	bodyDef.position.Set(pixelX / PTM_RATIO, pixelY / PTM_RATIO);
	bodyDef.type = b2_dynamicBody; //make it a dynamic body i.e. one moved by Box2D

	body = game->world->CreateBody(&bodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape boxShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	boxShape.SetAsBox(12.0f*x_scale / (2.f*PTM_RATIO), 12.0f*y_scale / (2.f*PTM_RATIO));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 0.1;
	fixtureDef.friction = 0.5f;

	//collison masking
	fixtureDef.filter.categoryBits = CMASK_ENEMY ;  //this is an enemy
	fixtureDef.filter.maskBits = CMASK_MY_SHOT | CMASK_PLAYER | CMASK_EDGES | CMASK_POWERUP; //it collides wth a few things

	body->CreateFixture(&fixtureDef);

	//make the userdata point back to this entity
	body->SetUserData(this);

}

void EnemyEntity::Update(float seconds)
{
	if(animating)
	{
		//update animation frames as needed
		frameTime += seconds;
		if(frameTime >= timePerFrame)
		{
			frameTime -= timePerFrame;
			currentFrame += pingpongDir;

			//reverse pingpong direction if past end of loop
			if(currentFrame > 2 || currentFrame < 0)
			{
				currentFrame = 1;
				pingpongDir *= -1;
			}
		}
	}

	//sprite = spriteArray[(int)facingDir][currentFrame];

	//move us
	b2Vec2 force = currentSpeed * moving;
	//body->ApplyForceToCenter(force, true);
	body->SetLinearVelocity(force);
	//body->ApplyLinearImpulse(force, body->GetPosition(), true);

}

void EnemyEntity::Move(float x_input, float y_input)
{
	moving.x = x_input;
	moving.y = y_input;

	//determine closest facing
	if(moving.x == 0.f && moving.y == 0.f)
		animating = false;
	else
	{
		angle = vec2deg(moving);
		animating = true;
	}

	while(angle > 360.f) angle -= 360.f;
	while(angle < 0.f) angle += 360.f;



}

void EnemyEntity::Draw()
{
	position = body->GetPosition();
	position = Physics2Pixels(position);

	sprite->angle = 0.f; //fix

	float scaleLeft = x_scale;

	// Draw the objects
	sprite->Blit(position.x, position.y, scaleLeft, y_scale);
}


void EnemyEntity::AI(float seconds)
{
	//this AI moves towards the player
	position = body->GetPosition();
	b2Vec2 dir = game->playerEntity1->body->GetPosition() - position;
	dir.Normalize();
	//randomize the motion?

	Move(dir.x, dir.y);
}