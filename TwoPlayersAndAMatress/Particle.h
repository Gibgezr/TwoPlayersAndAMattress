#pragma once

/*
Generic Particle class. Although I am using b2Vec2s form Box2D here,
all values such as speed and coords are pixel-relative.
*/

#include "Blit3D/Blit3D.h"
#include "Physics.h"

enum class EaseMode {LINEAR, EASEIN, EASEOUT};

class Particle
{
private:
	float percentDone;
	float currentSpeed;
	float currentXscale;
	float currentYscale;
	float currentAlpha;

public:
	std::vector<Sprite *> spriteList;
	float startingAlpha;
	float totalTimeToLive;
	float timeAliveSoFar;
	float angle;
	b2Vec2 direction;
	float startingSpeed;
	float targetSpeed;
	float rotationSpeed;
	b2Vec2 coords;
	bool useAlpha;
	int currentFrameNum;
	float timePerFrame;
	EaseMode easeMode;
	float startingXscale;
	float startingYscale;
	float targetXscale;
	float targetYscale;
	
	Particle();
	void Draw();
	bool Update(float seconds); //returns true if this particle should be removed
};