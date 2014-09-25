/********  TO DO  ************
Fix bugs People find

//bugs should be well explained so that I can fix them
+++++++ LIST OF FOUND BUGS ++++++++
	When you are on a wall and fall off do to staying on it too long you go flying(direction?)

	Jumping off a wall makes you jump super far and fast.

+++++++++++++++++++++++++++++++++++
*///////////////////////////////
#include "Player.h"

#define WALK_SPEED 5.0
#define RUN_SPEED  2.0
#define JUMP_POWER 8.0
#define GRAVITY    -20.0

Player::Player() : Camera(), isCollidingWall(false), isCollidingFloor(false), isRunWall(false), vel(0.0f, 0.0f, 0.0f), acc(0.0f, GRAVITY, 0.0f), wallTimer(0)
{
	wallDir = 'z';
	XMFLOAT3 temp = mPosition;
	temp.y -= height;
	boundingSphere.Center = temp;
	boundingSphere.Radius = extents;

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
		temp.y -= height;
		boundingSphere.Center = temp;
	}
	else if (isRunWall)//change so you jump off the wall only
	{
		isRunWall = false;
		isCollidingWall = false;
		XMFLOAT3 temp = mLook;
		temp.y = (temp.y + 1.0f) * 0.5f;//midpoint? sortof

		XMVECTOR tempDir = XMLoadFloat3(&temp);
		tempDir = XMVector3Normalize(tempDir);//hardly chnages it
		tempDir *= JUMP_POWER * 2.2f;
		XMStoreFloat3(&vel, XMVectorAdd(tempDir, XMLoadFloat3(&vel)));
		wallTimer == 0;
	}
}

void Player::Walk(float d)
{
	if (isCollidingFloor)
	{
		//Project the look vector on to the plane you are colliding with.
		XMVECTOR U, V, N;
		XMFLOAT3 direction;
		V = XMLoadFloat3(&mLook);
		N = XMVector3Normalize(currColFloor.Normal);

		U = V - XMVectorScale(N, XMVectorGetX(XMVector3Dot(V, N)));

		//using projection
		XMStoreFloat3(&direction, XMVector3Normalize(U)); //normilizing sucks?

		direction.y *= d * WALK_SPEED;
		direction.z *= d * WALK_SPEED;
		direction.x *= d * WALK_SPEED;

		XMStoreFloat3(&vel, XMLoadFloat3(&direction));
	}
	else
	{
		//moving in the air. maybe we dont want this
		XMFLOAT3 temp1 = mLook;
		temp1.y = vel.y;
		temp1.z *= d * WALK_SPEED;
		temp1.x *= d * WALK_SPEED;

		XMStoreFloat3(&vel, XMLoadFloat3(&temp1));
	}
}

void Player::Strafe(float d)
{
	// mPosition += d*mRight
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&vel);
	XMStoreFloat3(&vel, XMVectorMultiplyAdd(s, r, p));
}

void Player::Update(float dt)
{
	//combine if's and general cleanup needed. most of what is here is self explaintory

	//record player pos
	mPrevPos = mPosition;

	if (isCollidingFloor)
	{
		isRunWall = false;
		hasBeenOnWall = false;
		acc.y = 0;
		if (firstFloorHit)
		{
			vel.y = 0;
			firstFloorHit = false;
		}
	}
	else if (isRunWall)
	{
		acc.y = 0;
		firstFloorHit = true;
	}
	else
	{
		acc.y = GRAVITY;
		firstFloorHit = true;
	}

	if (isCollidingWall && !hasBeenOnWall && wallTimer == 0)
	{
		wallTimer = 1.1f;
		isRunWall = true;
		hasBeenOnWall = true;
	}

	if (wallTimer <= 0)
	{
		wallTimer = 0;
		isRunWall = false;
	}
	else
	{
		wallTimer -= dt;
	}

	if (isCollidingWall)
		DoWallCollisions();

	//this is a horrible way to do this.
	XMVECTOR s = XMVectorReplicate(dt);//gravity?
	XMVECTOR l = XMLoadFloat3(&vel);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	if (!isCollidingFloor)
	{
		//velocity and acceleration stuff
		vel.x += acc.x * dt;
		vel.z += acc.z * dt;
		vel.y += acc.y * dt;
	}

	if (vel.x > 2000000)
		vel.x = 2000000;
	if (vel.y > 2000000)
		vel.y = 2000000;
	if (vel.z > 2000000)
		vel.z = 2000000;

	//after movment
	//update bounding box
	XMFLOAT3 temp = mPosition;
	temp.y -= height;
	boundingSphere.Center = temp;


	//this is not right?
	if (isCollidingWall) //|| (!isRunWall && isCollidingWall) ) //this last part in not too sure
	{
		//mPosition = mPrevPos;
	}

	
}

void Player::Stop()
{
	if (isRunWall)
	{
		XMStoreFloat3(&vel, XMVectorReplicate(0.0f));
	}
	else if (isCollidingFloor)
	{
		XMStoreFloat3(&vel, XMVectorReplicate(0.0f));
	}
	else
	{
		XMVECTOR zeroXZ = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
		XMStoreFloat3(&vel, XMVectorMultiply(zeroXZ, XMLoadFloat3(&vel)));
	}
}

void Player::DoWallCollisions()
{
	float d = 1.0;

	//add the look vector to players position to get a point aproximet to were you are going
	XMVECTOR dest = XMLoadFloat3(&mPosition) + XMLoadFloat3(&vel);
	//do a check with this point and th wall you hit
	//make a sphere with 0 radius to act like a point
	XNA::Sphere point; point.Radius = 0.0001;
	XMStoreFloat3(&point.Center, dest);
	int out = XNA::IntersectSpherePlane(&point, XMVector4Normalize(currColWall.Plane));


	if (out == 0)//positive side of plane
	{		
		if (isCollidingFloor)
		{
			//Project the look vector on to the plane you are colliding with.
			XMVECTOR U, V, N;
			XMFLOAT3 direction;
			V = XMLoadFloat3(&vel);
			N = XMVector3Normalize(currColFloor.Normal);

			U = V - XMVectorScale(N, XMVectorGetX(XMVector3Dot(V, N)));

			//using projection
			XMStoreFloat3(&direction, XMVector3Normalize(U)); //normilizing sucks?

			direction.y *= d * WALK_SPEED;
			direction.z *= d * WALK_SPEED;
			direction.x *= d * WALK_SPEED;

			XMStoreFloat3(&vel, XMLoadFloat3(&direction));
		}
		//this else never seems to happen
		else 
		{
			//moving in the air and on the wall
			XMFLOAT3 temp1 = vel;

			//if you are running on the wall move up or down the wall acording to were you look
			if (isRunWall)
			{
				if (mLook.y > 0)
					temp1.y = mLook.y * WALK_SPEED;
				else
					temp1.y = -0.05;
			}
			else
			{
				temp1.y = vel.y;
			}

			temp1.z *= d * WALK_SPEED;
			temp1.x *= d * WALK_SPEED;

			XMStoreFloat3(&vel, XMLoadFloat3(&temp1));
		}
	}
	else//negitive side of plane
	{
		//sill move but only along wall
		//Project the look vector on to the plane you are colliding with.
		XMVECTOR U, V, N;
		XMFLOAT3 direction;
		V = XMLoadFloat3(&vel);

		N = XMVector3Normalize(currColWall.Normal);

		U = V - XMVectorScale(N, XMVectorGetX(XMVector3Dot(V, N)));

		//using projection
		XMStoreFloat3(&direction, XMVector3Normalize(U)); //normilizing sucks?

		//if you are running on the wall move up or down the wall acording to were you look
		if (isRunWall)
		{
			if (mLook.y > 0)
				direction.y = mLook.y * WALK_SPEED;
			else
				direction.y = -0.05;
		}
		else
		{
			direction.y = vel.y;
		}

		direction.z *= d * WALK_SPEED;
		direction.x *= d * WALK_SPEED;

		XMStoreFloat3(&vel, XMLoadFloat3(&direction));
	}
}


//old stuff
/*
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
	else if (isRunWall)//change so you jump off the wall only
	{
  		isRunWall = false;
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
		isRunWall = false;
		hasBeenOnWall = false;
		//vel.x = 0;
		//vel.z = 0;
		vel.y = 0;
	}

	if (!isCollidingWall)
	{
		isRunWall = false;
	}

	if (isCollidingWall && !hasBeenOnWall && wallTimer == 0)
	{
		wallTimer = 1.1f;
		isRunWall = true;
		hasBeenOnWall = true;
	}

	if (wallTimer <= 0)
	{
		wallTimer = 0;
		isRunWall = false;
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

	if (!isCollidingFloor && !isRunWall)
	{
		//this is a horrible way to do this.
		XMVECTOR s = XMVectorReplicate(dt);//gravity?
		XMVECTOR l = XMLoadFloat3(&vel);
		XMVECTOR p = XMLoadFloat3(&mPosition);
		XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
	}

	if ((isCollidingFloor && isCollidingWall)) //|| (!isRunWall && isCollidingWall) ) //this last part in not too sure
	{
		mPosition = mPrevPos;
	}

	//update bounding box
	XMFLOAT3 temp = mPosition;
	temp.y -= extents.y;
	boundingBox.Center = temp;
}

void Player::Walk(float d)
{
	if (isRunWall)
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
}*/