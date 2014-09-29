#pragma once
#include "Camera.h"
#include "xnacollision.h"
#include "LevelModel.h"

class Player : public Camera
{
public:
	Player();

	XMFLOAT3 vel;
	XMFLOAT3 acc;

	bool isCollidingFloor;
	bool isCollidingWall;
	bool isCollidingOther;
	bool isRunWall;
	bool hasBeenOnWall;
	
	TriData currColFloor;
	TriData currColWall;
	TriData currColOther;

	void Update(float dt);

	virtual void Walk(float d);
	virtual void Strafe(float d);
	void Stop();
	void Jump();

	//accessors
	XNA::Sphere GetBoundingSphere()
	{
		return boundingSphere;
	}
	void setWallDir(char c){ wallDir = c; }
private:
	XNA::Sphere boundingSphere;
	char wallDir;
	float wallTimer;

	bool firstFloorHit = false;

	void DoWallCollisions();

	XMFLOAT3 mPrevPos;

	float height = 1.0;
	float extents = 0.3;
};

