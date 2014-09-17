#pragma once
#include "d3dUtil.h"
#include "xnacollision.h"
#include "GeometryGenerator.h"
#include "Effects.h"

class Box
{
public:
	Box();
	Box(XMFLOAT3 size, XMFLOAT3 pos);

	~Box();

	void Draw();

	//accessors
	XNA::AxisAlignedBox Box::GetBoundingBox() const;
	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	GeometryGenerator::MeshData GetMeshData(){return mData;}
	UINT GetIndexCount(){ return mIndexCount; }
	UINT GetVertexCount(){ return mVertexCount; }
	XMFLOAT4X4 GetWorldMatrix(){ return mBoxWorld; }
private:
	XMFLOAT3 mPosition;
	UINT mIndexCount;
	UINT mVertexCount;
	XMFLOAT4X4 mBoxWorld;
	GeometryGenerator::MeshData mData;

	XNA::AxisAlignedBox boundingBox;

	XMFLOAT3 extents = XMFLOAT3(0.6f, 1.3f, 0.3f);
};

