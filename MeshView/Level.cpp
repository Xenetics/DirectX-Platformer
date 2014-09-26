#include "Level.h"


Level::Level()
{
}


Level::~Level()
{
}


void Level::Init()
{
	currLevel = new LevelModel(md3dDevice, *mTexMgr, fileName, L"Textures\\");

	BasicModelInstance testInstance;

	testInstance.Model = currLevel;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&testInstance.World, modelScale*modelRot*modelOffset);

	mModelInstances.push_back(testInstance);
}