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
	player1Sprites.push_back(game->player1Sprite);
	player2Sprites.push_back(game->player2Sprite);
	mattressSprite = game->mattressSegmentSprite;

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

	for(int i = 0; i < 4; ++i)
	{

		bodyDef.position.Set((960 + 40) / PTM_RATIO, 250 / PTM_RATIO);
		mattressBody[i] = game->world->CreateBody(&bodyDef); //create the body and add it to the world

		// Define a box shape for our dynamic body.
		b2PolygonShape mattressShape;
		//SetAsBox() takes as arguments the half-width and half-height of the box
		mattressShape.SetAsBox(40 / PTM_RATIO, 32 / PTM_RATIO);

		b2FixtureDef fixtureDef2;
		fixtureDef2.shape = &mattressShape;
		fixtureDef2.density = 1.0f;
		fixtureDef2.restitution = 0.1;
		fixtureDef2.friction = 0.5f;

		//collison masking
		fixtureDef2.filter.categoryBits = CMASK_MATTRESS;  //this is a player
		fixtureDef2.filter.maskBits = CMASK_ENEMY | CMASK_EDGES | CMASK_WALL | CMASK_LEVELWIN;		//it collides wth lotsa shit

		mattressBody[i]->CreateFixture(&fixtureDef2);

		//make the userdata point back to this entity
		mattressBody[i]->SetUserData(this);
	}
	//joints
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = body;
	revoluteJointDef.bodyB = mattressBody[0];
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(-34/PTM_RATIO, 0);
	joint1 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	//joint 5
	revoluteJointDef.bodyA = body2;
	revoluteJointDef.bodyB = mattressBody[3];
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(34 / PTM_RATIO, 0);
	joint5 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	//joint 2
	revoluteJointDef.bodyA = mattressBody[0];
	revoluteJointDef.bodyB = mattressBody[1];
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.lowerAngle = -0.25f * b2_pi; // -45 degrees
	revoluteJointDef.upperAngle = 0.25f * b2_pi; // 45 degrees
	revoluteJointDef.enableLimit = true;
	revoluteJointDef.maxMotorTorque = 10000.0f;
	revoluteJointDef.motorSpeed = 0.0f;
	revoluteJointDef.enableMotor = true;

	revoluteJointDef.localAnchorA.Set(34 / PTM_RATIO, 0);
	revoluteJointDef.localAnchorB.Set(-34 / PTM_RATIO, 0);
	joint2 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	//joint 3
	revoluteJointDef.bodyA = mattressBody[1];
	revoluteJointDef.bodyB = mattressBody[2];
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(34 / PTM_RATIO, 0);
	revoluteJointDef.localAnchorB.Set(-34 / PTM_RATIO, 0);
	joint3 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	//joint 4
	revoluteJointDef.bodyA = mattressBody[2];
	revoluteJointDef.bodyB = mattressBody[3];
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(34 / PTM_RATIO, 0);
	revoluteJointDef.localAnchorB.Set(-34 / PTM_RATIO, 0);
	joint4 = (b2RevoluteJoint*)game->world->CreateJoint(&revoluteJointDef);

	
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

	//mattress
	float32 gain = 10.5f;

	float32 angleError = joint1->GetJointAngle();	
	joint1->SetMotorSpeed(-gain * angleError);

	angleError = joint2->GetJointAngle();
	joint2->SetMotorSpeed(-gain * angleError);

	angleError = joint3->GetJointAngle();
	joint3->SetMotorSpeed(-gain * angleError);

	angleError = joint4->GetJointAngle();
	joint4->SetMotorSpeed(-gain * angleError);

	angleError = joint5->GetJointAngle();
	joint5->SetMotorSpeed(-gain * angleError);
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
		b2Vec2 moving = movingPlayer1;
		moving.Normalize();
		moving.y *= -1;
		anglePlayer1 = vec2rad(moving);
		body->SetTransform(body->GetPosition(), anglePlayer1);
	}
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
		b2Vec2 moving = movingPlayer2;
		moving.Normalize();
		moving.y *= -1;
		anglePlayer2 = vec2rad(moving);
		body2->SetTransform(body2->GetPosition(), anglePlayer2);
	}
}

void PlayerEntity::Draw()
{
	//Mattress
	for(int i = 0; i < 4; ++i)
	{
		position = mattressBody[i]->GetPosition();
		position = Physics2Pixels(position);

		angleMattress = mattressBody[i]->GetAngle();
		mattressSprite->angle = angleMattress;

		mattressSprite->Blit(position.x, position.y, x_scale, y_scale);
	}

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
