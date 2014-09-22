#pragma once
#include "LevelModel.h"

class Level
{
public:
	Level(ID3D11Device* device, TextureMgr& texMgr, std::vector<std::string> fileNames);
	~Level();
	std::vector<LevelModel> GetColModels()
	{
		return colModels;
	}

	std::vector<BasicModelInstance> GetColModelInstances()
	{
		return colModelInstances;
	}

private:
	std::vector<LevelModel> colModels;
	std::vector<BasicModelInstance> colModelInstances;
};

