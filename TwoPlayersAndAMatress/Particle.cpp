#include "Particle.h"

Particle::Particle()
{
	startingAlpha = currentAlpha = 1.f;
	totalTimeToLive = 0.3f;
	timeAliveSoFar = 0.f;
	angle = 0.f;
	direction = b2Vec2(0.f, 1.f);
	currentSpeed = startingSpeed = 100.f;
	targetSpeed = startingSpeed / 2;
	rotationSpeed = 0.f;
	percentDone = 0;;
	coords = b2Vec2(0,0);
	useAlpha = true;
	currentFrameNum = 0;
	timePerFrame = 1.f / 30.f; //default framerate, unused atm
	easeMode = EaseMode::EASEOUT;

	currentXscale = currentYscale = startingXscale = startingYscale = targetXscale = targetYscale = 1.f;
}

void Particle::Draw()
{
	spriteList[currentFrameNum]->angle = angle;
	//TODO: add scaling ease types and calculations
	spriteList[currentFrameNum]->Blit(coords.x, coords.y, currentXscale, currentYscale, currentAlpha);
}

bool Particle::Update(float seconds)
{
	timeAliveSoFar += seconds;
	if(timeAliveSoFar >= totalTimeToLive) return true; //early exit for dead particles

	//still alive, so update our position
	//assert(timeAliveSoFar > 0.f);
	percentDone = timeAliveSoFar / totalTimeToLive  ;

	//apply tween easing to speed and scale
	switch(easeMode)
	{
	case EaseMode::EASEIN:
		currentSpeed = startingSpeed + (targetSpeed - startingSpeed) * (percentDone * percentDone * percentDone);
		currentXscale = startingXscale + (targetXscale - startingXscale) * (percentDone * percentDone * percentDone);
		currentYscale = startingYscale + (targetYscale - startingYscale) * (percentDone * percentDone * percentDone);
		break;

	case EaseMode::EASEOUT:
		currentSpeed = startingSpeed + (targetSpeed - startingSpeed) * (1.f - percentDone * percentDone * percentDone);
		currentXscale = startingXscale + (targetXscale - startingXscale) * (1.f - percentDone * percentDone * percentDone);
		currentYscale = startingYscale + (targetYscale - startingYscale) * (1.f - percentDone * percentDone * percentDone);
		break;

	default:
		currentSpeed = startingSpeed + (targetSpeed - startingSpeed) * percentDone;
		currentXscale = startingXscale + (targetXscale - startingXscale) * percentDone;
		currentYscale = startingYscale + (targetYscale - startingYscale) * percentDone;
		break;
	}

	//update position
	coords += currentSpeed * seconds * direction;

	//update rotation
	angle += rotationSpeed * seconds;

	if(useAlpha)
	{
		currentAlpha = (1.f - percentDone * percentDone * percentDone ) * startingAlpha; //strong ease in
	}

	//update animation frames - skip using timePerFrame for now
	currentFrameNum = spriteList.size() * percentDone;
	if (currentFrameNum > spriteList.size() - 1) currentFrameNum = spriteList.size() - 1;

	return false;
}