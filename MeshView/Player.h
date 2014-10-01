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
	void SetPlayerRotation(float angle) 
	{ 
		XMMATRIX R = XMMatrixIdentity();

		XMStoreFloat3(&mRight, XMVector3TransformNormal(XMVectorSet(-4.37f,0,1,1), R));
		XMStoreFloat3(&mUp, XMVector3TransformNormal(XMVectorSet(0,1,0,1), R));
		XMStoreFloat3(&mLook, XMVector3TransformNormal(XMVectorSet(-1,0,-4.37f,1), R));

		R = XMMatrixRotationY(angle);

		XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
		XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
		XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
	}
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

