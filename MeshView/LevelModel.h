#pragma once
#include "BasicModel.h"
#include "xnaCollision.h"

//struc aligned to 32 bit, maybe change to 16? ask Bryan
__declspec(align(32)) struct TriData
{
	XMVECTOR Center;            // Center of the Triangle.
	FLOAT Radius;               // Radius of the bounding sphere around the triangle.
	XMVECTOR P0;				//poitns...
	XMVECTOR P1;
	XMVECTOR P2;
	XMVECTOR Plane;
	XMVECTOR Normal;
	
};

class LevelModel : public BasicModel
{
public:
	LevelModel(ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath, MeshGeometry::Type type = MeshGeometry::NORMAL_DIFFUSE);
	~LevelModel();

	std::vector<TriData> data;
};

