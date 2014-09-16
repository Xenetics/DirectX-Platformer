#pragma once
#include "Camera.h"
#include "xnacollision.h"

class Player : public Camera
{
public:
	Player();

	XMFLOAT3 vel;
	XMFLOAT3 acc;

	bool isCollidingFloor;
	bool isCollidingWall;
	bool isOnWall;
	bool hasBeenOnWall;
	

	void Update(float dt);

	virtual void Walk(float d);
	virtual void Strafe(float d);
	void Stop();
	void Jump();

	//accessors
	XNA::AxisAlignedBox GetBoundingBox()
	{
		return boundingBox;
	}
	void setWallDir(char c){ wallDir = c; }
private:
	XNA::AxisAlignedBox boundingBox;
	char wallDir;
	float wallTimer;


	XMFLOAT3 mPrevPos;
	XMFLOAT3 extents = XMFLOAT3(0.9f, 1.3f, 0.9f);
};

