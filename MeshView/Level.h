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
private:
	BasicModel* testModel;
	LevelModel* currLevel;

	std::vector<BasicModelInstance> mModelInstances;
};

