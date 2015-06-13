#pragma once
#include "Blit3D/Blit3D.h"
#include <atomic>

#include "Physics.h"
#include "Entity.h"
#include "EdgeEntity.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"


#include "MyContactListener.h" //for handling collisions
#include "CollisionMask.h"
#include "Particle.h" //particles, yay!
//#include <fmod_studio.hpp> //for events
//#include <fmod.hpp>
//#include <fmod_errors.h>

#include "Camera.h"

class Game
{
public:
	Blit3D *blit3D = NULL;

	//GLOBAL DATA
	//Box2D does not use glm::vec2, but instead provides b2Vec2, a similiar
	//2d vector object
	b2Vec2 gravity; //defines our gravity vector
	b2World *world; //our physics engine

	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and ~10 iterations. This provides a high quality simulation
	// in most game scenarios.
	int32 velocityIterations = 8;
	int32 positionIterations = 3;
	float timeStep = 1.f / 60.f; //one 60th of a second
	float elapsedTime = 0; //used for calculating time passed


	std::vector<Sprite *> spriteList;

	//contact listener to handle collisions between important objects
	MyContactListener *contactListener;

	std::vector<EnemyEntity *> enemyEntityList; //enemies go here
	std::vector<Entity *> entityList; //other entities in our game go here
	std::vector<Entity *> deadEntityList; //dead entities

	PlayerEntity *playerEntity1 = NULL;

	int level = 1;

	std::vector<Particle *> particleList;

	//AUDIO STUFF
	//FMOD::Studio::System*     FMODStudioSystem;
	//FMOD::Channel    *channel = 0;
	//FMOD_RESULT       result;
	//FMOD::Studio::Bank* masterBank = NULL;
	//FMOD::Studio::Bank* stringsBank = NULL;
	//FMOD_STUDIO_PLAYBACK_STATE FMODstate;
	//
	//FMOD::Studio::ParameterInstance* gamePlayMusicIntensityParameter = NULL;
	//FMOD::Studio::ParameterInstance* titleMusicIntensityParameter = NULL;
	//float gamePlayMusicIntensity = 0.f; //parameter variable
	//float titleMusicIntensity = 0.f; //parameter variable
	//
	//double musicTimer = 0;
	//
	////SFX
	//FMOD::Studio::EventInstance* hitInstance = NULL;
	//FMOD::Studio::EventInstance* gameOverInstance = NULL;
	//FMOD::Studio::EventInstance* levelAdvanceInstance = NULL;
	//FMOD::Studio::EventInstance* loseLifeInstance = NULL;
	//FMOD::Studio::EventInstance* powerUpInstance = NULL;
	//FMOD::Studio::EventInstance* boinkInstance = NULL;
	//FMOD::Studio::EventInstance* explosionInstance = NULL;
	//
	////Music
	//FMOD::Studio::EventInstance* gamePlayMusicInstance = NULL;
	//FMOD::Studio::EventInstance* titleMusicInstance = NULL;

	//used for joystick
	std::mutex joystickMutex;
	B3D::JoystickState joystickState1;
	B3D::JoystickState joystickState2;
	int joystickNumber1;
	int joystickNumber2;
	float joystick1PositionAxis1 = 0.f;
	float joystick1PositionAxis2 = 0.f;
	float joystick2PositionAxis1 = 0.f;
	float joystick2PositionAxis2 = 0.f;

	bool foundJoystick1 = false;
	bool foundJoystick2 = false;

	double deadTimer = 0;

	Camera2D camera;

	Sprite *levelSprite = NULL;
	float levelWidth = 3000;
	float levelHeight = 3000;
	Sprite *defaultSprite = NULL;

	Sprite *titleSprite = NULL;

	Sprite *player1Sprite;
	Sprite *player2Sprite;

	Sprite *guardSprite;
	Sprite *mattressSegmentSprite;

	Sprite *guardAlertSprite;
	Sprite *guardQuestionSprite;
	Sprite *guardGuardNeutralSprite;

	Sprite *winScreenSprite;
	Sprite *loseScreenSprite;

};