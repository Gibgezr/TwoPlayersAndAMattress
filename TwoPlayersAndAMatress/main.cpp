/*
	Two Players & a Mattress - small game for the Jalloo game jam 2015
*/

//this code sets up memory leak detection
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include "Game.h"

enum GameState { START, PLAYING, LOADLEVEL, LEVELREADY, DEAD, RESTART, RESTART2, GAMEOVER };
GameState gameState = START;

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	if(result != FMOD_OK)
	{
		std::cout << file << ", line " << line << " - FMOD ERROR:" << result << " - " << FMOD_ErrorString(result) << std::endl;
		assert("FMOD error" == 0);
	}
}

#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

#define BUFFER_OFFSET(i) ((char *)NULL + (i))



Game *game;

//______MAKE THE LEVEL______
void MakeLevel()
{
	//remove any old enemies
	for(auto b : game->enemyEntityList)
	{
		game->world->DestroyBody(b->body);
		delete b;
	}
	game->enemyEntityList.clear();

	//delete all extra entities, like powerups
	for(int i = game->entityList.size() - 1; i >= 0; --i)
	{
		if(game->entityList[i]->typeID != ENTITYEDGE)
		{
			game->world->DestroyBody(game->entityList[i]->body);
			delete game->entityList[i];
			game->entityList.erase(game->entityList.begin() + i);
		}
	}
		
	//delete all particles
	for(auto p : game->particleList) delete p;
	game->particleList.clear();

	//make the new level
	
}

//ensures that entities are only added ONCE to the deadEntityList
void AddToDeadList(Entity *e)
{
	bool unique = true;

	for(auto ent : game->deadEntityList)
	{
		if(ent == e)
		{
			unique = false;
			break;
		}
	}

	if(unique) game->deadEntityList.push_back(e);
}

void Init()
{
	glDisable(GL_CULL_FACE); //so we can invert scaling to get characters flipped to walk left	

	//turn off the cursor
	game->blit3D->ShowCursor(false);
	
	//load the sprites
	game->spriteList.push_back(game->blit3D->MakeSprite(0, 0, 64, 64, "media\\player.png"));
	game->spriteList.push_back(game->blit3D->MakeSprite(0, 0, 196, 64, "media\\matress.png"));
	
	//from here on, we are setting up the Box2D physics world model

	// Define the gravity vector.
	game->gravity.x = 0.f;
	game->gravity.y = 0.f;

	// Construct a world object, which will hold and simulate the rigid bodies.
	game->world = new b2World(game->gravity);
	//world->SetGravity(gravity); <-can call this to change gravity at any time
	game->world->SetAllowSleeping(true); //set true to allow the physics engine to 'sleep" objects that stop moving

	//_________GROUND OBJECT_____________
	//make an entity for the edges
	EdgeEntity * edgeEntity = new EdgeEntity();
	//A bodyDef for the ground
	b2BodyDef groundBodyDef;
	// Define the ground body.
	groundBodyDef.position.Set(0, 0);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	edgeEntity->body = game->world->CreateBody(&groundBodyDef);
	
	//an EdgeShape object, for the ground
	b2EdgeShape groundBox;

	// Define the ground as 1 edge shape at the bottom of the screen.
	b2FixtureDef boxShapeDef;

	boxShapeDef.shape = &groundBox;

	//collison masking
	boxShapeDef.filter.categoryBits = CMASK_EDGES;  //this is the ground
	boxShapeDef.filter.maskBits = CMASK_ENEMY | CMASK_MATTRESS | CMASK_PLAYER;		//it collides wth everything

	//bottom
	groundBox.Set(b2Vec2(0, 0), b2Vec2(game->blit3D->screenWidth / PTM_RATIO, 0));
	//Create the fixture
	edgeEntity->body->CreateFixture(&boxShapeDef);
	//add the userdata
	edgeEntity->body->SetUserData(edgeEntity);
	

	//left
	groundBox.Set(b2Vec2(0, game->blit3D->screenHeight / PTM_RATIO), b2Vec2(0, 0));
	edgeEntity->body->CreateFixture(&boxShapeDef);

	//top
	groundBox.Set(b2Vec2(0, (game->blit3D->screenHeight - 40) / PTM_RATIO),
		b2Vec2(game->blit3D->screenWidth / PTM_RATIO, (game->blit3D->screenHeight - 40) / PTM_RATIO));
	edgeEntity->body->CreateFixture(&boxShapeDef);

	//right
	groundBox.Set(b2Vec2(game->blit3D->screenWidth / PTM_RATIO,
		0), b2Vec2(game->blit3D->screenWidth / PTM_RATIO, game->blit3D->screenHeight / PTM_RATIO));
	edgeEntity->body->CreateFixture(&boxShapeDef);

	game->entityList.push_back(edgeEntity);

	
	// Create contact listener and use it to collect info about collisions
	game->contactListener = new MyContactListener();
	game->world->SetContactListener(game->contactListener);

	//AUDIO STUFF
	unsigned int      version;
	void             *extraDriverData = 0;

	/*
	Create a System object and initialize
	*/
	//ERRCHECK(FMOD::Studio::System::create(&FMODStudioSystem));

	//ERRCHECK(FMODStudioSystem->initialize(64, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData));

	////load master bank (even if empty) and strings table
	//
	//ERRCHECK(FMODStudioSystem->loadBankFile("DataFiles\\Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

	//ERRCHECK(FMODStudioSystem->loadBankFile("DataFiles\\Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));

	//now load our events and make instances of them

	//hit
	/*FMOD::Studio::EventDescription* hitDescription = NULL;
	ERRCHECK(FMODStudioSystem->getEvent("event:/SFX/hit", &hitDescription));*/
	
	//ERRCHECK(hitDescription->createInstance(&hitInstance));
	
	//check for joysticks, stop at first joystick found plugged in
	for(game->joystickNumber = 1; game->joystickNumber <= 8; ++game->joystickNumber)
	{
		if(game->blit3D->CheckJoystick(game->joystickNumber))
		{
			game->foundJoystick = true;
			break;
		}
	}

	if(game->foundJoystick)
	{
		//get the state arrays for this joystick
		if(!game->blit3D->PollJoystick(game->joystickNumber, game->joystickState)) game->foundJoystick = false;
	}
	
	game->playerEntity1 = new PlayerEntity();
}

void DeInit(void)
{
	
	//clean up audio
	//hitInstance->release();
	

	//unload all banks
	/*ERRCHECK(stringsBank->unload());
	ERRCHECK(masterBank->unload());*/


	//release the sound system memory
	/*ERRCHECK(FMODStudioSystem->release());*/


	//delete all particles
	for(auto p : game->particleList) delete p;
	game->particleList.clear();

	//delete all the entities
	for(auto e : game->entityList) delete e;
	game->entityList.clear();

	for(auto e : game->enemyEntityList) delete e;
	game->enemyEntityList.clear();

	if(game->playerEntity1 != NULL) delete game->playerEntity1;

	//delete the contact listener
	delete game->contactListener;

	//Free all physics game data we allocated
	delete game->world;
	
	//any sprites still allocated are freed automatcally by the Blit3D object when we destroy it
	if(game->blit3D) delete game->blit3D;
}

void Update(double seconds)
{
	
	//FMODStudioSystem->update();

	switch(gameState)
	{
	case PLAYING:
	{
					bool loseLife = false;
					game->elapsedTime += seconds;

					//did we finish the level?
					
					//don't apply physics unless at least a timestep worth of time has passed
					while(game->elapsedTime >= game->timeStep)
					{
						//update the physics world
						game->world->Step(game->timeStep, game->velocityIterations, game->positionIterations);

						// Clear applied body forces. 
						game->world->ClearForces();

						game->elapsedTime -= game->timeStep;

						//update game logic/animation
						for(auto e : game->entityList) e->Update(game->timeStep);
						b2Vec2 playerPosition = game->playerEntity1->body->GetPosition();
						for(auto e : game->enemyEntityList) e->AI(game->timeStep);
						for(auto e : game->enemyEntityList) e->Update(game->timeStep);
					
					
						game->joystickMutex.lock();
						game->playerEntity1->MovePlayer1(game->joystickPositionAxis1, game->joystickPositionAxis2);
						game->playerEntity1->Update(game->timeStep);
						game->joystickMutex.unlock();

						//update the particle list and remove dead particles
						for(int i = game->particleList.size() - 1; i >= 0; --i)
						{
							if(game->particleList[i]->Update(game->timeStep))
							{
								//time to die!
								delete game->particleList[i];
								game->particleList.erase(game->particleList.begin() + i);
							}
						}

						//loop over contacts
						for(int pos = 0; pos < game->contactListener->contacts.size(); ++pos)
						{
							MyContact contact = game->contactListener->contacts[pos];

							//fetch the entities from the body userdata
							Entity *A = (Entity *)contact.fixtureA->GetBody()->GetUserData();
							Entity *B = (Entity *)contact.fixtureB->GetBody()->GetUserData();

							if (A != NULL && B != NULL) //if there is an entity for these objects...
							{


								//if (A->typeID == ENTITY_MY_SHOT)
								//{
								//	//swap A and B
								//	Entity *C = A;
								//	A = B;
								//	B = C;
								//}


								
							}//end of checking if they are both NULL userdata
						}//end of collison handling

						//clean up dead entities
						for(auto e : game->deadEntityList)
						{
							//remove body from the physics world and free the body data
							game->world->DestroyBody(e->body);
							//remove the entity from the appropriate entityList
							if (e->typeID == ENTITYENEMY)
							{
								for(int i = 0; i < game->enemyEntityList.size(); ++i)
								{
									if(e == game->enemyEntityList[i])
									{
										delete game->enemyEntityList[i];
										game->enemyEntityList.erase(game->enemyEntityList.begin() + i);
										break;
									}
								}
							}
							else
							{
								for(int i = 0; i < game->entityList.size(); ++i)
								{
									if (e == game->entityList[i])
									{
										delete game->entityList[i];
										game->entityList.erase(game->entityList.begin() + i);
										break;
									}
								}
							}
						}

						game->deadEntityList.clear();
					}

					if (loseLife)
					{
						gameState = GAMEOVER;
						
					}
	}
		break;

	case START:
		
		break;
	case LOADLEVEL:
	{
		game->level++;
		MakeLevel();	

		gameState = LEVELREADY;
	}
		break;
	
	case LEVELREADY:
		//update the particle list and remove dead particles
		for(int i = game->particleList.size() - 1; i >= 0; --i)
		{
			if(game->particleList[i]->Update(game->timeStep))
			{
				//time to die!
				delete game->particleList[i];
				game->particleList.erase(game->particleList.begin() + i);
			}
		}
		game->deadTimer += seconds;
		if(game->deadTimer > 1)
		{
			game->deadTimer = 0;
			gameState = PLAYING;
		}
		break;
	case DEAD:
		
		break;	
	default:
		//do nada here
		break;
	}//end switch(gameState)
	
}

void Draw(void)
{
	switch(gameState)
	{
	case START:
		glClearColor(0.1f, 0.1f, 0.2f, 0.0f);	//clear colour: r,g,b,a 
		break;
	default:
		glClearColor(0.2f, 0.3f, 0.3f, 0.0f);	//clear colour: r,g,b,a 
		break;
	}

	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	switch(gameState)
	{
	case START:
	{
		/*std::string title = "SHOOTASTIC!";
		float widthTitle = fontXeno150->WidthText(title);
		fontXeno150->BlitText(1920 / 2 - widthTitle / 2, 900, title);
		title = "a small game by Darren Reid for the mini Ludum Dare #59";
		widthTitle = fontXeno30->WidthText(title);
		fontXeno30->BlitText(1920 / 2 - widthTitle / 2, 800, title);

		if(!foundJoystick) fontComputer50->BlitText(100, 500, "Plug in a game controller to play.");
		fontComputer50->BlitText(100, 400, "Left stick : move");
		fontComputer50->BlitText(100, 300, "Right stick : fire");
		fontComputer50->BlitText(100, 200, "'A' button : continue");
		fontComputer50->BlitText(100, 100, "'Back' button or escape on the keyboard : stop the madness");*/
	}
		break;

			
	case DEAD:
	{
		/*std::string title = "Press 'A' to continue";
		float widthTitle = fontComputer50->WidthText(title);
		fontComputer50->BlitText(1920 / 2 - widthTitle / 2, 100, title);*/
	}	
	case LOADLEVEL:
	case LEVELREADY:
	case PLAYING:		
	{
		//loop over all entities and draw them
		for(auto e : game->entityList) e->Draw();
		for(auto e : game->enemyEntityList) e->Draw();
		for(auto p : game->particleList) p->Draw();
		if(game->playerEntity1 != NULL) game->playerEntity1->Draw();
					
	}	
	break;

	case GAMEOVER:
	{
		/*std::string scoreCounter = "Score: " + std::to_string(score);
		fontXeno150->BlitText(50, 800, scoreCounter);

		std::string levelCounter = "Level: " + std::to_string(level);
		fontXeno150->BlitText(50, 400, levelCounter);*/
	}
		break;
	}//end switch
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		game->blit3D->Quit(); //start the shutdown sequence	
}

void DoJoystick(void)
{
	if(game->foundJoystick)
	{
		if(game->blit3D->PollJoystick(game->joystickNumber, game->joystickState))
		{
			//joystick at joystickNumber is still plugged in, so we can work with it's states
			game->joystickMutex.lock();
			//axis states are between -1.f and 1.f
			if(game->joystickState.axisCount > 0) game->joystickPositionAxis1 = game->joystickState.axisStates[0];
			if(game->joystickState.axisCount > 1) game->joystickPositionAxis2 = game->joystickState.axisStates[1];
			if(game->joystickState.axisCount > 3) game->joystickPositionAxis3 = game->joystickState.axisStates[3];
			if(game->joystickState.axisCount > 4) game->joystickPositionAxis4 = game->joystickState.axisStates[4];
			
			//deadzone
			if(game->joystickPositionAxis1 < 0.25f && (game->joystickPositionAxis1 > -0.25f)) game->joystickPositionAxis1 = 0.f;
			if(game->joystickPositionAxis2 < 0.25f && (game->joystickPositionAxis2 > -0.25f)) game->joystickPositionAxis2 = 0.f;
			if(game->joystickPositionAxis3 < 0.25f && (game->joystickPositionAxis3 > -0.25f)) game->joystickPositionAxis3 = 0.f;
			if(game->joystickPositionAxis4 < 0.25f && (game->joystickPositionAxis4 > -0.25f)) game->joystickPositionAxis4 = 0.f;

			if(game->joystickState.buttonCount > 6)
			{
				//pressing BACK button will exit the program
				if(game->joystickState.buttonStates[6] == GLFW_PRESS) game->blit3D->Quit(); //start the shutdown sequence
			}
			if(game->joystickState.buttonCount > 0)
			{
				//pressing A button will start the game playing
				if(game->joystickState.buttonStates[0] == GLFW_PRESS)
				{
					switch(gameState)
					{
					case START:
						gameState = PLAYING;
						
						game->level = 1;
						MakeLevel();						
						break;

					case PLAYING:
						
						break;
					
					case LEVELREADY:
						
						break;

					case DEAD:
						gameState = START;
						break;

					case GAMEOVER:
						gameState = START;
						break;
					}
				}
			}
			game->joystickMutex.unlock();
		}
		else game->foundJoystick = false;
	}
	else
	{
		//search for a joystick again
		game->joystickMutex.lock();
		//check for joysticks, stop at first joystick found plugged in
		for(game->joystickNumber = 1; game->joystickNumber <= 8; ++game->joystickNumber)
		{
			if(game->blit3D->CheckJoystick(game->joystickNumber))
			{
				game->foundJoystick = true;
				break;
			}
		}

		if(game->foundJoystick)
		{
			//get the state arrays for this joystick
			if(!game->blit3D->PollJoystick(game->joystickNumber, game->joystickState)) game->foundJoystick = false;
		}

		game->joystickMutex.unlock();
	}
}

int main(int argc, char *argv[])
{
	//the line below turns on memory leak detection in debug mode
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	game = new Game();
	game->blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW_1080P, "Two Players & a Matress", 1600, 900);

	//set our callback funcs
	game->blit3D->SetInit(Init);
	game->blit3D->SetDeInit(DeInit);
	game->blit3D->SetUpdate(Update);
	game->blit3D->SetDraw(Draw);
	game->blit3D->SetDoInput(DoInput);
	game->blit3D->SetDoJoystick(DoJoystick);

	//Run() blocks until the window is closed
	game->blit3D->Run(Blit3DThreadModel::SINGLETHREADED);

	delete game;
}