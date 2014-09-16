#include "Player.h"

#define WALK_SPEED 1.0
#define RUN_SPEED  2.0
#define JUMP_POWER 10.0

Player::Player() : Camera(), isCollidingWall(false), isCollidingFloor(false), vel(0.0f, 0.0f, 0.0f), acc(0.0f, -20.0f, 0.0f), wallTimer(0)
{
	wallDir = 'z';
	XMFLOAT3 temp = mPosition;
	temp.y -= extents.y;
	boundingBox.Center = temp;
	boundingBox.Extents = extents;

	mPrevPos = mPosition;
}

void Player::Jump()
{
	if (isCollidingFloor)
	{
		mPosition.y += 0.1;
		vel.y = JUMP_POWER;
		isCollidingFloor = false;
		//now update bounding box to fix collision problems
		XMFLOAT3 temp = mPosition;
		temp.y -= extents.y;
		boundingBox.Center = temp;
	}
	else if (isOnWall)//change so you jump off the wall only
	{
  		isOnWall = false;
		isCollidingWall = false;
		XMFLOAT3 temp = mLook;
		temp.y = (temp.y + 1.0f) * 0.5f;//midpoint? sortof
		
		XMVECTOR tempDir = XMLoadFloat3(&temp);
		//tempDir = XMVector3Normalize(tempDir);
		tempDir *= JUMP_POWER * 2.2f;
		XMStoreFloat3(&vel, XMVectorAdd(tempDir, XMLoadFloat3(&vel)));
		wallTimer == 0;
	}
}
/*
void Player::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&vel);
	XMStoreFloat3(&vel, XMVectorMultiplyAdd(s, r, p));
}*/


void Player::Strafe(float d)
{
	if (!isCollidingWall)
	{
		// mPosition += d*mRight
		XMVECTOR s = XMVectorReplicate(d);
		XMVECTOR r = XMLoadFloat3(&mRight);
		XMVECTOR p = XMLoadFloat3(&mPosition);
		XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
	}
}

void Player::Update(float dt)
{
	//record player pos
	mPrevPos = mPosition;

	if (isCollidingFloor)
	{
		isOnWall = false;
		hasBeenOnWall = false;
		//vel.x = 0;
		//vel.z = 0;
		vel.y = 0;
	}

	if (!isCollidingWall)
	{
		isOnWall = false;
	}

	if (isCollidingWall && !hasBeenOnWall && wallTimer == 0)
	{
		wallTimer = 1.1f;
		isOnWall = true;
		hasBeenOnWall = true;
	}

	if (wallTimer <= 0)
	{
		wallTimer = 0;
		isOnWall = false;
	}
	else
	{
		wallTimer -= dt;
	}

	//velocity and acceleration stuff
	vel.x += acc.x * dt;
	vel.z += acc.z * dt;
	vel.y += acc.y * dt;

	if (vel.x > 2000000)
		vel.x = 2000000;
	if (vel.y > 2000000)
		vel.y = 2000000;
	if (vel.z > 2000000)
		vel.z = 2000000;

	if (!isCollidingFloor && !isOnWall)
	{
		//this is a horrible way to do this.
		XMVECTOR s = XMVectorReplicate(dt);//gravity?
		XMVECTOR l = XMLoadFloat3(&vel);
		XMVECTOR p = XMLoadFloat3(&mPosition);
		XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
	}

	if ((isCollidingFloor && isCollidingWall)) //|| (!isOnWall && isCollidingWall) ) //this last part in not too sure
	{
		mPosition = mPrevPos;
	}

	//update bounding box
	XMFLOAT3 temp = mPosition;
	temp.y -= extents.y;
	boundingBox.Center = temp;
}
/*
void Player::Walk(float d)
{
	if (isOnWall)
	{
		if (wallDir == 'z')
		{
			//make it so you can only move in the xz plane
			// mPosition += d*mLook
			XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
			XMFLOAT3 temp1 = mLook;
			temp1.x = 0.0f;
			if (temp1.y < 0)
				temp1.y = 0;

			XMVECTOR l = XMLoadFloat3(&temp1);
			XMVECTOR p = XMLoadFloat3(&mPosition);
			l = XMVector3Normalize(l);
			XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
		}
		else
		{
			//make it so you can only move in the xz plane
			// mPosition += d*mLook
			XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
			XMFLOAT3 temp1 = mLook;
			temp1.z = 0.0f;
			if (temp1.y < 0)
				temp1.y = 0;

			XMVECTOR l = XMLoadFloat3(&temp1);
			XMVECTOR p = XMLoadFloat3(&mPosition);
			l = XMVector3Normalize(l);
			XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
		}
	}
	else
	{
		//make it so you can only move in the xz plane
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
		XMFLOAT3 temp1 = mLook;
		temp1.y = vel.y;

		XMVECTOR l = XMLoadFloat3(&temp1);
		XMVECTOR p = XMLoadFloat3(&vel);
		l = XMVector3Normalize(l);
		XMStoreFloat3(&vel, XMVectorMultiplyAdd(s, l, p));
	}
}*/


void Player::Walk(float d)
{
	if (isOnWall)
	{
		if (wallDir == 'z')
		{
			//make it so you can only move in the xz plane
			// mPosition += d*mLook
			XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
			XMFLOAT3 temp1 = mLook;
			temp1.x = 0.0f;
			if (temp1.y < 0)
				temp1.y = 0;

			XMVECTOR l = XMLoadFloat3(&temp1);
			XMVECTOR p = XMLoadFloat3(&mPosition);
			l = XMVector3Normalize(l);
			XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
		}
		else
		{
			//make it so you can only move in the xz plane
			// mPosition += d*mLook
			XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
			XMFLOAT3 temp1 = mLook;
			temp1.z = 0.0f;
			if (temp1.y < 0)
				temp1.y = 0;

			XMVECTOR l = XMLoadFloat3(&temp1);
			XMVECTOR p = XMLoadFloat3(&mPosition);
			l = XMVector3Normalize(l);
			XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
		}
	}
	else
	{
		//make it so you can only move in the xz plane
		// mPosition += d*mLook
		XMVECTOR s = XMVectorReplicate(d * WALK_SPEED);
		XMFLOAT3 temp1 = mLook;
		temp1.y = 0.0f;

		XMVECTOR l = XMLoadFloat3(&temp1);
		XMVECTOR p = XMLoadFloat3(&mPosition);
		l = XMVector3Normalize(l);
		XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
	}
}

void Player::Stop()
{
	if (isOnWall)
	{
		XMStoreFloat3(&vel, XMVectorReplicate(0.0f));
	}
	else
	{
		XMVECTOR zeroXZ = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f) );
		XMStoreFloat3(&vel, XMVectorMultiply(zeroXZ, XMLoadFloat3(&vel)));
	}
}