#include "Level.h"


Level::Level(ID3D11Device* device, TextureMgr* tex, std::string name) : md3dDevice(device), mTexMgr(tex), fileName(name)
{

}


Level::~Level()
{
	//needs more
	//this should also b in another function that just clears out what has been initilized in the init function
	SafeDelete(level);
}


void Level::Init()
{
	level = new LevelModel(md3dDevice, *mTexMgr, fileName, L"Textures\\");

	BasicModelInstance testInstance;

	testInstance.Model = level;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&testInstance.World, modelScale*modelRot*modelOffset);

	mModelInstances.push_back(testInstance);
}