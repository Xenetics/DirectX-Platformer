#include "Box.h"

Box::Box()
{
	XMFLOAT3 size = XMFLOAT3(1.0, 1.0, 1.0);
	XMFLOAT3 pos = XMFLOAT3(0.0, 0.0, 0.0);

	GeometryGenerator gen;
	gen.CreateBox(1.0, 1.0, 1.0, mData);
	mIndexCount = mData.Indices.size();
	mVertexCount = mData.Vertices.size();

	XMMATRIX boxScale = XMMatrixScaling(size.x, size.y, size.z);
	XMMATRIX boxOffset = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	//update bounding box  
	boundingBox.Center = mPosition;
	boundingBox.Extents = XMFLOAT3(size.x * 0.5, size.y * 0.5, size.z * 0.5);
}

Box::Box(XMFLOAT3 size, XMFLOAT3 pos) : mPosition(pos)
{
	GeometryGenerator gen;
	gen.CreateBox(1.0, 1.0, 1.0, mData);
	mIndexCount = mData.Indices.size(); 
	mVertexCount = mData.Vertices.size();

	XMMATRIX boxScale = XMMatrixScaling(size.x, size.y, size.z);
	XMMATRIX boxOffset = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	//update bounding box  
	boundingBox.Center = mPosition;
	boundingBox.Extents = XMFLOAT3(size.x * 0.5, size.y * 0.5, size.z * 0.5);
}

Box::~Box()
{
}

XNA::AxisAlignedBox Box::GetBoundingBox() const
{
	return boundingBox;
}
XMVECTOR Box::GetPositionXM()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Box::GetPosition()const
{
	return mPosition;
}

void Box::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	//update bounding box
	boundingBox.Center = mPosition;
}

void Box::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	//update bounding box
	boundingBox.Center = mPosition;
}
