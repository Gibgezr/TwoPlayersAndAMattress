#pragma once

#include "PlayerEntity.h"

enum class EnemyType { GRUNT = 1, SHOOTER };

enum class GuardState {
	Patrol,
	Check,
	Look,
	Chase
};

class EnemyEntity : public Entity
{
private:
	b2Vec2 moving;
	float angle;
	int currentFrame;
	float frameTime;
	int pingpongDir;
	bool animating;
	float x_scale;
	float y_scale;
	float currentSpeed;
	EnemyType enemyType;
	GuardState guardState;

	//ai vars
	// Position and Movement variables
	
	float movementSpeed;
	float runSpeed;
	b2Vec2 direction;
	b2Vec2 moveTarget;
	b2Vec2 lastSeenPlayerLocation;

	// Patrol Path Variables
	
	int pathPointCounter;
	float inRangeOfPoint;
	bool isPathReversable = true;
	bool reversePath = false;

	// Catch player variables
	bool playerInView;
	float sightRange;
	float noticeTime;
	float totalNoticeTime;
	b2Vec2 playerPosition;

	// Rotation Variables
	float rotationSpeed;
	float totalRotation;

public:
	b2Vec2 currentPosition;
	std::vector<b2Vec2> pathList;
	float timePerFrame;
	
	EnemyEntity(float pixelX, float pixelY, EnemyType type);
	void Update(float seconds);
	
	void Draw();

	void Move(float seconds);
	void Patrol(float seconds);
	void Check(float seconds);
	void Look(float seconds);
	void Chase(float seconds);
	void CheckVision(float seconds);
	void Run(float seconds);
	void RotateLook(float seconds);
	static b2Vec2 RotateVector2d(b2Vec2 dir, float degree);
	static float DotProd2D(b2Vec2 v1, b2Vec2 v2);
};