#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include "TextureMgr.h"
#include "BasicModel.h"
#include "LevelModel.h"

class Level
{
public:
	Level();
	~Level();

	void Init();
private:
	BasicModel* testModel;
	LevelModel* currLevel;

	ID3D11Device* md3dDevice;
	TextureMgr* mTexMgr;

	std::string fileName;

	std::vector<BasicModelInstance> mModelInstances;
};

