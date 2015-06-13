#pragma once

#include "Entity.h"

class PlayerEntity : public Entity
{
private:
	b2Vec2 movingPlayer1;
	b2Vec2 movingPlayer2;
	float anglePlayer1;
	float anglePlayer2;
	int currentFrame;	
	float frameTime;
	int pingpongDir;
	bool animatingPlayer1;
	bool animatingPlayer2;
	float x_scale;
	float y_scale;
	float currentSpeed;
	std::vector<Sprite *>player1Sprites;
	std::vector<Sprite *>player2Sprites;
	Sprite *mattressSprite;
	b2Body *body2;
	b2Body *mattressBody;
	b2RevoluteJoint *joint1;
	b2RevoluteJoint *joint2;

public:
	float timePerFrame;
	PlayerEntity();
	void Update(float seconds);
	void MovePlayer1(float x_input, float y_input);
	void MovePlayer2(float x_input, float y_input);
	void Draw();
};