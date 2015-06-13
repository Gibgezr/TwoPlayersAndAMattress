#pragma once

#include "PlayerEntity.h"

enum class EnemyType { GRUNT = 1, SHOOTER };

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
	

public:
	float timePerFrame;
	
	EnemyEntity(float pixelX, float pixelY, EnemyType type);
	void Update(float seconds);
	void Move(float x_input, float y_input);
	void Draw();
	void AI(float seconds);
};