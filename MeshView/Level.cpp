#include "Level.h"


Level::Level(ID3D11Device* device, TextureMgr& texMgr, std::vector<std::string> fileNames)
{
	//This data input should maybe be read in from a file that way we can specify stuff like the posistion of objects once more objects are added.
	for (int i = 0; i < fileNames.size; i++)
	{
		LevelModel tempModel = LevelModel(device, texMgr, fileNames[i], L"Textures\\");
		colModels.push_back(tempModel);
		BasicModelInstance testInstance;

		testInstance.Model = &tempModel;



		XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, -1.0f);
		XMMATRIX modelRot = XMMatrixRotationY(0.0f);
		XMMATRIX modelOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&testInstance.World, modelScale*modelRot*modelOffset);

		colModelInstances.push_back(testInstance);
	}
	
}


Level::~Level()
{
}
