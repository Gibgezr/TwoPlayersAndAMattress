#pragma once

#include "Blit3D\Blit3D.h"
extern Blit3D *blit3D;

class Camera2D
{
public:
	glm::vec3 panOffset; //use this to pan the camera around
	int panX, panY, panZ; //use these to do whole pixel panning
	float minX, minY, maxX, maxY, minZ, maxZ; //min and max offsets in each direction
	glm::vec3 moveDir; //direction to pan in
	float panSpeed; //how fast we pan in pixels/seconds

	Camera2D()
	{
		panOffset = glm::vec3(0, 0, 0);
		minX = minY = minZ = 0;
		maxX = maxY = maxZ = 0;
		moveDir = glm::vec3(0, 0, 0);
		panSpeed = 1000;
	}

	void Pan(float x, float y)
	{
		moveDir.x = x;
		moveDir.y = y;
	}

	void PanTo(float x, float y)
	{
		panOffset.x = x;
		panOffset.y = y;
	}

	void Update(float seconds)
	{
		panOffset += moveDir * seconds;

		//bounds check
		if(panOffset.x < minX) panOffset.x = minX;
		if(panOffset.x > maxX) panOffset.x = maxX;
		if(panOffset.y < minY) panOffset.y = minY;
		if(panOffset.y > maxY) panOffset.y = maxY;
		if(panOffset.z < minZ) panOffset.z = minZ;
		if(panOffset.z > maxZ) panOffset.z = maxZ;
	}

	void Draw()
	{
		//convert to whole pixels
		panX = -panOffset.x;
		panY = -panOffset.y; 
		panZ = panOffset.z;

		blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
		blit3D->viewMatrix = glm::translate(blit3D->viewMatrix, glm::vec3(panX, panY, panZ)); //translate the view matrix by the offset
		blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send new matrix to the shader
	}

	void UnDraw()
	{
		blit3D->viewMatrix = glm::mat4(1.f); //identity matrix
		blit3D->shader2d->setUniform("viewMatrix", blit3D->viewMatrix); //send new matrix to the shader
	}
};