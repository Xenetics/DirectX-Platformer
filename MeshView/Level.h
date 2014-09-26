#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include "TextureMgr.h"
#include "LevelModel.h"


//needs some sort of delete function that does not destry the object but resets the loaded in model information.

class Level
{
public:
	Level(ID3D11Device* device, TextureMgr* tex, std::string name);
	~Level();

	void Init();

	LevelModel* GetLevelModel()
	{
		return level;
	}
	std::vector<BasicModelInstance> GetModelInstances()
	{
		return mModelInstances;
	}
private:
	LevelModel* level;
	std::vector<BasicModelInstance> mModelInstances;
	
	ID3D11Device* md3dDevice;
	TextureMgr* mTexMgr;
	std::string fileName;

	
};

