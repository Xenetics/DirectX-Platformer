#include "LevelModel.h"


LevelModel::LevelModel(ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath, MeshGeometry::Type type) :
	BasicModel(device, texMgr, modelFilename, texturePath, type)
{
	for (int j = 0; j < BasicVertices.size(); j += 3)
	{
		//make new triData object
		TriData tData;
		//get Points
		XMVECTOR P0 = XMLoadFloat3(&BasicVertices[Indices[j]].Pos);
		XMVECTOR P1 = XMLoadFloat3(&BasicVertices[Indices[j + 1]].Pos);
		XMVECTOR P2 = XMLoadFloat3(&BasicVertices[Indices[j + 2]].Pos);
		
		//calculate Normal
		//XMVECTOR x = XMVectorSubtract(P1, P0);
		//XMVECTOR y = XMVectorSubtract(P2, P0);
		//tData.Normal = XMVector3Cross(x, y);
		tData.Normal = XMVector3Cross(P1 - P0, P2 - P0);

		//calculate the plane
		//set up vars
		XMFLOAT3 v1, v2, v3, temp;
		XMStoreFloat3(&v1, P0);
		XMStoreFloat3(&v2, P1);
		XMStoreFloat3(&v3, P2);
		XMFLOAT4 plane;
		//calculate
		plane.x = v1.y*(v2.z - v3.z) + v2.y*(v3.z - v1.z) + v3.y*(v1.z - v2.z);
		plane.y = v1.z*(v2.x - v3.x) + v2.z*(v3.x - v1.x) + v3.z*(v1.x - v2.x);
		plane.z = v1.x*(v2.y - v3.y) + v2.x*(v3.y - v1.y) + v3.x*(v1.y - v2.y);
		plane.w = -(v1.x*(v2.y*v3.z - v3.y*v2.z) + v2.x*(v3.y*v1.z - v1.y*v3.z) + v3.x*(v1.y*v2.z - v2.y*v1.z));
		//set the plane
		tData.Plane = XMLoadFloat4(&plane);

		//find the center
		temp.x = (v1.x + v2.x + v3.x) * 0.3333;
		temp.y = (v1.y + v2.y + v3.y) * 0.3333;
		temp.z = (v1.z + v2.z + v3.z) * 0.3333;
		//set the center
		tData.Center = XMLoadFloat3(&temp);

		//find radius of bounding sphere
		float L1, L2, L3;
		XMVECTOR vTemp = XMLoadFloat3(&temp);
		XMStoreFloat(&L1, XMVector3Length(vTemp - P0));
		XMStoreFloat(&L2, XMVector3Length(vTemp - P1));
		XMStoreFloat(&L3, XMVector3Length(vTemp - P2));
		if (L1 > L2 && L1 > L3)
			tData.Radius = L1;
		else if (L2 > L1 && L2 > L3)
			tData.Radius = L2;
		else if (L3 > L2 && L3 > L1)
			tData.Radius = L3;

		//set the points 
		tData.P0 = P0;
		tData.P1 = P1;
		tData.P2 = P2;
		
		//add it to yhe vector
		data.push_back(tData);
		
	}
}


LevelModel::~LevelModel()
{
}
