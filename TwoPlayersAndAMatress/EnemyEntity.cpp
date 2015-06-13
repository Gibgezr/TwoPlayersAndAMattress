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
	x_scale = y_scale = 1.f;

	guardState = GuardState::Patrol;

	pathPointCounter = 0;
	pathList.push_back(b2Vec2(pixelX / PTM_RATIO, pixelY/PTM_RATIO));

	sightRange = 500.f/PTM_RATIO;
	inRangeOfPoint = 25.f/PTM_RATIO;
	playerInView = false;

	movementSpeed = 85.f / PTM_RATIO;
	runSpeed = 250.f / PTM_RATIO;
	rotationSpeed = 90.f; // In Degrees

	totalRotation = 0.f;
	direction = b2Vec2(1,0);
	currentPosition = b2Vec2(pixelX/PTM_RATIO, pixelY/PTM_RATIO);

	totalNoticeTime = 0.5f;
	noticeTime = totalNoticeTime;
		
	enemyType = type;
	switch (enemyType)
	{
	case EnemyType::SHOOTER:
		//currentSpeed = 50.f;
		break;

	default:
		//currentSpeed = 40.f;
		break;
	}


	sprite = game->spriteList[2]; //fix this

	//physics body
	b2BodyDef bodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	bodyDef.position.Set(pixelX / PTM_RATIO, pixelY / PTM_RATIO);
	bodyDef.type = b2_dynamicBody; //make it a dynamic body i.e. one moved by Box2D

	body = game->world->CreateBody(&bodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2CircleShape circleShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	circleShape.m_radius = 64.f / PTM_RATIO;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 1.0f;
	fixtureDef.restitution = 0.1;
	fixtureDef.friction = 0.5f;

	//collison masking
	fixtureDef.filter.categoryBits = CMASK_ENEMY ;  //this is an enemy
	fixtureDef.filter.maskBits = CMASK_PLAYER | CMASK_EDGES | CMASK_MATTRESS | CMASK_WALL; //it collides wth a few things

	body->CreateFixture(&fixtureDef);

	//make the userdata point back to this entity
	body->SetUserData(this);

}

void EnemyEntity::Update(float seconds)
{
	playerPosition = game->playerEntity1->mattressBody->GetPosition();
	currentPosition = body->GetPosition();
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

	switch(guardState) {
	case GuardState::Patrol:
		Patrol(seconds);
		break;
	case GuardState::Check:
		Check(seconds);
		break;
	case GuardState::Look:
		Look(seconds);
		break;
	case GuardState::Chase:
		Chase(seconds);
		break;
	}

	CheckVision(seconds);

	// Rotate the sprite to face direction
	float desiredAngle = atan2f(-direction.x, direction.y);
	body->SetTransform(currentPosition, desiredAngle);

}

void EnemyEntity::Draw()
{
	position = body->GetPosition();
	position = Physics2Pixels(position);

	sprite->angle = body->GetAngle();

	float scaleLeft = x_scale;

	// Draw the objects
	sprite->Blit(position.x, position.y, scaleLeft, y_scale);
}

void EnemyEntity::Patrol(float seconds) {
	// Calculate Movement Target
	if((pathList[pathPointCounter] - currentPosition).Length() <= inRangeOfPoint) {
		if(pathPointCounter >= pathList.size() - 1) {
			if(isPathReversable) {
				reversePath = true;
			}
			else {
				pathPointCounter = -1;
			}
		}
		else if(pathPointCounter <= 0) {
			reversePath = false;
		}

		if(reversePath) {
			pathPointCounter--;
		}
		else {
			pathPointCounter++;
		}
	}

	moveTarget = pathList[pathPointCounter];
	Move(seconds);
}

void EnemyEntity::Check(float seconds) {
	// The location is set in the check vision
	if((lastSeenPlayerLocation - currentPosition).Length() <= inRangeOfPoint) {
		guardState = GuardState::Look;
		noticeTime = 0.f;
		totalRotation = 0.f;
	}

	moveTarget = lastSeenPlayerLocation;
	Run(seconds);
}

void EnemyEntity::Look(float seconds) {
	// Rotate around in a circle
	RotateLook(seconds);

	body->SetLinearVelocity(b2Vec2(0.f, 0.f));

	if(totalRotation > 360) {
		// Did not see the player return to the patrol
		guardState = GuardState::Patrol;
	}
}

void EnemyEntity::Chase(float seconds) {
	// Run towards the player target
	moveTarget = lastSeenPlayerLocation;
	Run(seconds);

	if((lastSeenPlayerLocation - currentPosition).Length() <= inRangeOfPoint) {
		guardState = GuardState::Look;
		noticeTime = 0.f;
		totalRotation = 0.f;
	}
}

void EnemyEntity::CheckVision(float seconds) {
	b2Vec2 playerDirection = (playerPosition - currentPosition);
	playerDirection.Normalize();

	// Check Vision Cone
	if(DotProd2D(playerDirection, direction) > 0.5f) {
		if((playerPosition - currentPosition).Length() <= sightRange) {
			//RaycastHit2D hit = Physics2D.Raycast(currentPosition, (playerPosition - currentPosition));

			//set up input
			b2RayCastInput input;
			input.p1 = currentPosition;
			input.p2 = playerPosition;
			input.maxFraction = 1;

			//check every fixture of every body to find closest
			float closestFraction = 1; //start with end of line as p2
			b2Vec2 intersectionNormal(0, 0);
			b2Fixture* collisionFixture = NULL;
			for(b2Body* b = game->world->GetBodyList(); b; b = b->GetNext()) {
				for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext()) {

					b2RayCastOutput output;
					if(!f->RayCast(&output, input, 0))
						continue;
					if(output.fraction < closestFraction) {
						closestFraction = output.fraction;
						intersectionNormal = output.normal;
						collisionFixture = f;
					}
				}
			}

			if(collisionFixture == NULL)
			{
				noticeTime -= seconds;

				if(noticeTime <= 0.f) {
					noticeTime = 0.f;
				}
				return;
			}
			//get entity from fixture
			b2Body *collisionBody = collisionFixture->GetBody();
			Entity * collisionEntity = (Entity *)collisionBody->GetUserData();
			if(collisionEntity == NULL)
			{
				noticeTime -= seconds;

				if(noticeTime <= 0.f) {
					noticeTime = 0.f;
				}
				return;
			}

			if(collisionEntity->typeID == ENTITYPLAYER) {
				// enemy can see the player!
				noticeTime += seconds;
				// TODO Show a question mark over the guards head to show that he noticed something

				if(noticeTime > totalNoticeTime) {
					//Debug.Log ("Guard noticed the player");
					// TODO Show an exclaimation mark over the guards head to show that he noticed something

					switch(guardState) {
					case GuardState::Patrol:
						// TODO Delay and then set the location if correct
						lastSeenPlayerLocation = playerPosition;
						guardState = GuardState::Check;
						noticeTime = 0.f;
						break;
					case GuardState::Check:
						break;
					case GuardState::Look:
						lastSeenPlayerLocation = playerPosition;
						guardState = GuardState::Chase;
						// set a chase var and start a countdown to do a check if you still see the player
						break;
					case GuardState::Chase:
						// set the player position
						lastSeenPlayerLocation = playerPosition;
						noticeTime = totalNoticeTime * 0.5f;
						break;
					}

				}
			}
			else
			{
				noticeTime -= seconds;

				if(noticeTime <= 0.f) {
					noticeTime = 0.f;
				}
				return;
			}
		}
		else
		{
			noticeTime -= seconds;

			if(noticeTime <= 0.f) {
				noticeTime = 0.f;
			}
			return;
		}
	}
	else {
		noticeTime -= seconds;

		if(noticeTime <= 0.f) {
			noticeTime = 0.f;
		}
	}
}

void EnemyEntity::Move(float seconds) {
	direction = (moveTarget - currentPosition);
	direction.Normalize();
	//currentPosition += direction * movementSpeed * seconds;
	//transform.position = currentPosition;

	//move us
	b2Vec2 force = movementSpeed * direction;
	//body->ApplyForceToCenter(force, true);
	body->SetLinearVelocity(force);
	//body->ApplyLinearImpulse(force, body->GetPosition(), true);

}

void EnemyEntity::Run(float seconds) {
	if((moveTarget - currentPosition).Length() > 0.1f) {
		direction = (moveTarget - currentPosition);
	}
	direction.Normalize();
	//currentPosition += direction * runSpeed * seconds;
	//transform.position = currentPosition;

	//move us
	b2Vec2 force = runSpeed * direction;
	//body->ApplyForceToCenter(force, true);
	body->SetLinearVelocity(force);
	//body->ApplyLinearImpulse(force, body->GetPosition(), true);
}

void EnemyEntity::RotateLook(float seconds) {
	totalRotation += rotationSpeed * seconds;
	direction = RotateVector2d(direction, rotationSpeed * seconds);
	// check for a full rotation, if complete and no player as target then return to the path
}

b2Vec2 EnemyEntity::RotateVector2d(b2Vec2 dir, float degree) {
	float radian = degree * M_PI / 180;
	b2Vec2 result;
	result.x = cos(radian);
	result.y = sin(radian);

	return b2Vec2(result.x*dir.x - result.y*dir.y, result.y*dir.x + result.x*dir.y);
}

float EnemyEntity::DotProd2D(b2Vec2 v1, b2Vec2 v2) {
	return v1.x * v2.x + v1.y * v2.y;
}