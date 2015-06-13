#include "PlayerEntity.h"
#include "Game.h"
extern Game *game;

PlayerEntity::PlayerEntity()
{
	typeID = ENTITYPLAYER;
	movingPlayer1 = movingPlayer2 = b2Vec2(0.f, 0.f);
	anglePlayer1 = anglePlayer2 = 90;
	currentFrame = 1;
	timePerFrame = 0.1f;
	frameTime = 0.f;
	pingpongDir = 1;
	animatingPlayer1 = animatingPlayer2 = false;
	x_scale = y_scale = 1.f;
	currentSpeed = 50.f;


	sprite = game->spriteList[0];
	player1Sprites.push_back(game->spriteList[0]);
	player2Sprites.push_back(game->spriteList[0]);
	mattressSprite = game->spriteList[1];

	//physics body
	b2BodyDef bodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	bodyDef.position.Set(960 / PTM_RATIO, 250 / PTM_RATIO);
	bodyDef.type = b2_dynamicBody; //make it a dynamic body i.e. one moved by Box2D

	body = game->world->CreateBody(&bodyDef); //create the body and add it to the world

	// Define a shape for our dynamic body.
	b2CircleShape playerShape;
	playerShape.m_radius = 64.f / PTM_RATIO;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &playerShape;
	fixtureDef.density = 1.0f; 
	fixtureDef.restitution = 0.1;
	fixtureDef.friction = 0.5f;

	//collison masking
	fixtureDef.filter.categoryBits = CMASK_PLAYER;  //this is a player
	fixtureDef.filter.maskBits = CMASK_ENEMY | CMASK_EDGES | CMASK_WALL;		//it collides wth lotsa shit

	body->CreateFixture(&fixtureDef);

	//make the userdata point back to this entity
	body->SetUserData(this);

	//PLAYER 2
	bodyDef.position.Set((960+ 194) / PTM_RATIO, 250 / PTM_RATIO);
	body2 = game->world->CreateBody(&bodyDef); //create the body and add it to the world
	body2->CreateFixture(&fixtureDef);
	body2->SetUserData(this);

	//MATTRESS
	bodyDef.position.Set((960 + 98) / PTM_RATIO, 250 / PTM_RATIO);
	mattressBody = game->world->CreateBody(&bodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape mattressShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	mattressShape.SetAsBox(98 / PTM_RATIO, 32 / PTM_RATIO);

	b2FixtureDef fixtureDef2;
	fixtureDef2.shape = &playerShape;
	fixtureDef2.density = 1.0f;
	fixtureDef2.restitution = 0.1;
	fixtureDef2.friction = 0.5f;

	//collison masking
	fixtureDef2.filter.categoryBits = CMASK_MATTRESS;  //this is a player
	fixtureDef2.filter.maskBits = CMASK_ENEMY | CMASK_EDGES | CMASK_WALL;		//it collides wth lotsa shit

	mattressBody->CreateFixture(&fixtureDef2);

	//make the userdata point back to this entity
	mattressBody->SetUserData(this);

	//joints
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = body;
	revoluteJointDef.bodyB = mattressBody;
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(-98/PTM_RATIO, 0);
	joint1 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	//joint 2
	revoluteJointDef.bodyA = body2;
	revoluteJointDef.bodyB = mattressBody;
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(98 / PTM_RATIO, 0);
	joint2 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);
}

void PlayerEntity::Update(float seconds)
{

	if(animatingPlayer1)
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
	b2Vec2 force = currentSpeed * movingPlayer1;
	force.y = -1 * force.y;
	//body->ApplyForceToCenter(force, true);
	body->SetLinearVelocity(force);
	//body->ApplyLinearImpulse(force, body->GetPosition(), true);

	force = currentSpeed * movingPlayer2;
	force.y = -1 * force.y;
	body2->SetLinearVelocity(force);

}

void PlayerEntity::MovePlayer1(float x_input, float y_input)
{
	movingPlayer1.x = x_input;
	movingPlayer1.y = y_input;

	//determine closest facing
	if(movingPlayer1.x == 0.f && movingPlayer1.y == 0.f)
		animatingPlayer1 = false;
	else
	{
		anglePlayer1 = vec2deg(movingPlayer1);
		animatingPlayer1 = true;
	}

	while(anglePlayer1 > 360.f) anglePlayer1 -= 360.f;
	while(anglePlayer1 < 0.f) anglePlayer1 += 360.f;
}

void PlayerEntity::MovePlayer2(float x_input, float y_input)
{
	movingPlayer2.x = x_input;
	movingPlayer2.y = y_input;

	//determine closest facing
	if(movingPlayer2.x == 0.f && movingPlayer2.y == 0.f)
		animatingPlayer2 = false;
	else
	{
		anglePlayer2 = vec2deg(movingPlayer2);
		animatingPlayer2 = true;
	}

	while(anglePlayer2 > 360.f) anglePlayer2 -= 360.f;
	while(anglePlayer2 < 0.f) anglePlayer2 += 360.f;
}

void PlayerEntity::Draw()
{
	//Mattress
	position = mattressBody->GetPosition();
	position = Physics2Pixels(position);

	angleMattress = mattressBody->GetAngle();
	mattressSprite->angle = angleMattress;

	mattressSprite->Blit(position.x, position.y, x_scale, y_scale);

	//Player 1
	position = body->GetPosition();
	position = Physics2Pixels(position);

	anglePlayer1 = body->GetAngle();
	player1Sprites[0]->angle = anglePlayer1;
	
	// Draw the objects
	player1Sprites[0]->Blit(position.x, position.y, x_scale, y_scale);

	//Player 2
	position = body2->GetPosition();
	position = Physics2Pixels(position);

	anglePlayer2 = body2->GetAngle();
	player2Sprites[0]->angle = anglePlayer2;

	// Draw the objects
	player2Sprites[0]->Blit(position.x, position.y, x_scale, y_scale);

	
	
}
