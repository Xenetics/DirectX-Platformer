/**************************      TO DO      ********************************************

		
	see player classes TO DO
	recored any movment bugs in the player class

	menus

	level switching and picking(menus)
		maybe see about not loading the level until it is needed? beucase loading al the levels on start up will take forever
		This part still needs to be done ^
		
		basicly implement on the fly level switching.

		Maybe make and e-num for the difrent levels(not really needed at all)

		Roof and other Collisions

//***************************************************************************************/

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Player.h"
#include "Sky.h"
#include "RenderStates.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "TextureMgr.h"
#include "BasicModel.h"
#include "LevelModel.h"
#include "SoundMgr.h"
#include "Level.h"


struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

struct Vertexes //used to make quad vertices for the menu, do not touch please (Damian)
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
 
class MeshViewApp : public D3DApp 
{
public:
	MeshViewApp(HINSTANCE hInstance);
	~MeshViewApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);



	enum class GAME_STATE
	{
		menuState = 1,
		playingState,
		pauseState
	};

private:
	void DrawSceneToSsaoNormalDepthMap();
	void DrawSceneToShadowMap();
	void DrawScreenQuad(ID3D11ShaderResourceView* srv);
	void BuildShadowTransform();
	void BuildScreenQuadGeometryBuffers();
	void BuildGeometryBuffers();
	void BuildMenuFX();
	void BuildVertexLayout();

	void LoadCurrLevel();


	//state based functions
	void UpdateWhilePlaying(float dt);
	void UpdateMainMenu(float dt);
	void DrawWhilePlaying();
	void DrawMenu();

	//misc functions
	void KeyHandler(float dt);

private:
	ID3D11RasterizerState* mWireframeRS;

	TextureMgr mTexMgr;

	Sky* mSky;

	//BasicModel* testModel;
	int currLevel = 2;

	std::vector<BasicModelInstance> mModelInstances;
	//std::vector<BasicModelInstance> mAlphaClippedModelInstances;

	//stuff bellow is needed for my menus (Damian)
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3D11InputLayout* mInputLayout;
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	//

	ID3D11Buffer* mSkySphereVB;
	ID3D11Buffer* mSkySphereIB;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	BoundingSphere mSceneBounds;

	static const int SMapSize = 2048;
	ShadowMap* mSmap;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	Ssao* mSsao;

	float mLightRotationAngle;
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];

	Player mPlayer;
	SoundMgr* mSound;

	POINT mLastMousePos;

	//level stuff
	std::vector<Level*> mLevels;

	//important menu floats
	float mTheta;
	float mPhi;
	float mRadius;

	//key bools
	bool wKey = false;
	bool aKey = false;
	bool sKey = false;
	bool dKey = false;
	bool space = false;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	MeshViewApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

MeshViewApp::MeshViewApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), //currLevel(0),
mScreenQuadVB(0), mScreenQuadIB(0),
  mSmap(0), mSsao(0),
  mLightRotationAngle(0.0f), mBoxVB(0), mBoxIB(0), mFX(0), mTech(0),
  mfxWorldViewProj(0), mInputLayout(0),
  mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Platformer";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mPlayer.SetPosition(0.0f, 10.0f, 0.0f);
 
	mDirLights[0].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient  = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -1.0, 0.0f);

	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;
}

MeshViewApp::~MeshViewApp()
{
	//this needs work
	//SafeDelete(testModel);

	SafeDelete(mSky);
	SafeDelete(mSmap);
	SafeDelete(mSsao);

	ReleaseCOM(mScreenQuadVB);
	ReleaseCOM(mScreenQuadIB);

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
	RenderStates::DestroyAll();
}

bool MeshViewApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mTexMgr.Init(md3dDevice);

	mSky = new Sky(md3dDevice, L"Textures/rainy.dds", 5000.0f);
	mSmap = new ShadowMap(md3dDevice, SMapSize, SMapSize);

	mPlayer.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	mSsao = new Ssao(md3dDevice, md3dImmediateContext, mClientWidth, mClientHeight, mPlayer.GetFovY(), mPlayer.GetFarZ());

	BuildScreenQuadGeometryBuffers();

	//set up the levels so they are ready to load when needed
	Level* lvl = new Level(md3dDevice, &mTexMgr, "Models\\testMap.alx");
	mLevels.push_back(lvl);
	lvl = new Level(md3dDevice, &mTexMgr, "Models\\testMap2.alx");
	mLevels.push_back(lvl);
	lvl = new Level(md3dDevice, &mTexMgr, "Models\\testMapSmooth.alx");
	mLevels.push_back(lvl);

	LoadCurrLevel();

	//revers face culling
	D3D11_RASTERIZER_DESC jimJam;
	ZeroMemory(&jimJam, sizeof(D3D11_RASTERIZER_DESC));
	jimJam.FillMode = D3D11_FILL_SOLID;
	jimJam.CullMode = D3D11_CULL_FRONT;
	jimJam.AntialiasedLineEnable = true;
	jimJam.FrontCounterClockwise = false;
	HR(md3dDevice->CreateRasterizerState(&jimJam, &mWireframeRS));
	md3dImmediateContext->RSSetState(mWireframeRS);

	//init sound
	mSound = new SoundMgr();
	srand(time(NULL));

	//build menu stuff
	//BuildGeometryBuffers();
	//BuildMenuFX();
	//BuildVertexLayout();


	return true;
}

void MeshViewApp::LoadCurrLevel()
{
	mLevels[currLevel]->Init();

	//
	// Compute scene bounding box.
	//

	//load this in
	 mModelInstances = mLevels[currLevel]->GetModelInstances();

	XMFLOAT3 minPt(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	for (UINT i = 0; i < mModelInstances.size(); ++i)
	{
		for (UINT j = 0; j < mModelInstances[i].Model->BasicVertices.size(); ++j)
		{
			XMFLOAT3 P = mModelInstances[i].Model->BasicVertices[j].Pos;

			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.x, P.x);
			minPt.z = MathHelper::Min(minPt.x, P.x);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.x, P.x);
			maxPt.z = MathHelper::Max(maxPt.x, P.x);
		}
	}

	//
	// Derive scene bounding sphere from bounding box.
	//
	mSceneBounds.Center = XMFLOAT3(
		0.5f*(minPt.x + maxPt.x),
		0.5f*(minPt.y + maxPt.y),
		0.5f*(minPt.z + maxPt.z));

	XMFLOAT3 extent(
		0.5f*(maxPt.x - minPt.x),
		0.5f*(maxPt.y - minPt.y),
		0.5f*(maxPt.z - minPt.z));

	mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);
}

void MeshViewApp::OnResize()
{
	D3DApp::OnResize();

	mPlayer.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	if( mSsao )
	{
		mSsao->OnSize(mClientWidth, mClientHeight, mPlayer.GetFovY(), mPlayer.GetFarZ());
	}
}

void MeshViewApp::UpdateScene(float dt)
{

	GAME_STATE gameState = GAME_STATE::playingState;

	mSound->UpdateSound();

	switch (gameState)
	{
	case GAME_STATE::menuState:
		UpdateMainMenu(dt);
		break;

	case GAME_STATE::playingState:
		UpdateWhilePlaying(dt);
		break;

	case GAME_STATE::pauseState:
		break;

	}
	

}

void MeshViewApp::UpdateWhilePlaying(float dt)
{
	//deal with keypresses
	KeyHandler(dt);
	// screaming while falling 


	//Do player collisions: loops throiugh the current levels triangls and does a bunch of stuff

	//reset the collsiion status
	mPlayer.isCollidingFloor = false;
	mPlayer.isCollidingWall = false;
	//setup vars
	XNA::Sphere pSphere= mPlayer.GetBoundingSphere();
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0, 1.0, 0.0));
	//make a slightly smaller version of the players sphere 
	XNA::Sphere pSmallSphere;
	pSmallSphere.Center = pSphere.Center;
	pSmallSphere.Radius = pSphere.Radius * 1.1f;//this value may be changed to work better.

	//use this for wall collisions to improve walking and wall running 
	//the slight offset in y makes it so that you dont get stuck in the ground and on walls
	XNA::Sphere pTallSphere;
	XMFLOAT3 adjustment = pSphere.Center;
	adjustment.y += 0.3;//this value may be changed to work better.
	pTallSphere.Center = adjustment;
	pTallSphere.Radius = pSphere.Radius;

	//get the vector of indices and Vertices and store them
	std::vector<TriData> tData = mLevels[currLevel]->GetLevelModel()->data;
	for (int j = 0; j < tData.size(); j++)
	{
		//only do collision checks with spheres you can collide with by doing a radius check first
		if (XNA::IntersectSphereSphere(&tData[j].Bounds, &pSphere))
		{
			//use the smaller player sphere to check and see if the player is above the triangles plane
			int daBug = XNA::IntersectSpherePlane(&pSmallSphere, tData[j].Plane); //this may be compleatly Uneeded and useless But I cant tell
			if (daBug == 0)
			{
				XMVECTOR P0 = tData[j].P0;
				XMVECTOR P1 = tData[j].P1;
				XMVECTOR P2 = tData[j].P2;

				//get the angle of the triangles normal to the worlds up to diecide if 
				//we are checking a wall or a floor or a ceiling(other)
				//this code should be moved to the levelModel class where it can be done once on load 
				//for a fairly large preformance increase
				XMVECTOR vAngleR = XMVector3AngleBetweenVectors(up, tData[j].Normal);
				float angleD = (XMVectorGetX(vAngleR) * 180) / MathHelper::Pi;



				if ((angleD > 0 && angleD < 65.0f) && mPlayer.isCollidingFloor == false) //floor collisions
				{
					//are you collidioing?
					mPlayer.isCollidingFloor = XNA::IntersectTriangleSphere(P2, P1, P0, &pSphere);
					//if you are...
					if (mPlayer.isCollidingFloor)
					{
						mPlayer.currColFloor = tData[j];
						//debug output for the center of the triangle  you colide with
						std::wstringstream debug;
						debug << L"Pos of collsiion ";
						debug << tData[j].Bounds.Center.x;
						debug << L"		 ";
						debug << tData[j].Bounds.Center.y;
						debug << L"		 ";
						debug << tData[j].Bounds.Center.z;
						debug << std::endl;
						//OutputDebugString((LPCTSTR)debug.str().c_str());
					}
				}
				else if ((angleD > 65.0f && angleD < 115.0f) && mPlayer.isCollidingWall == false) //Wall collisions(same sort of thing as floor)
				{
					mPlayer.isCollidingWall = XNA::IntersectTriangleSphere(P2, P1, P0, &pTallSphere);
					if (mPlayer.isCollidingWall)
					{
						OutputDebugString(L"thats a Wall");
						mPlayer.currColWall = tData[j];
					}
				}
				/********* CURRENTLY UNIMPLEMENTED IN THE PLAYER CLASS **************
				else if ((angleD > 115.0f && angleD < 180.0f) && mPlayer.isCollidingOther == false) //Bounce collisions( stuff you cant walk or run on)
				{
					mPlayer.isCollidingOther = XNA::IntersectTriangleSphere(P2, P1, P0, &pSphere);//not sure if I should Use tall or normal Shpere
					if (mPlayer.isCollidingOther)
					{
						OutputDebugString(L"thats a Wall");
						mPlayer.currColOther = tData[j];
					}
				}
				*/
			}
		}
	}

	

	//update player
	mPlayer.Update(dt);

	//
	// Animate the lights (and hence shadows).
	//

	BuildShadowTransform();

	mPlayer.UpdateViewMatrix();
}

void MeshViewApp::UpdateMainMenu(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void MeshViewApp::DrawScene()
{

	GAME_STATE gameState = GAME_STATE::playingState;

	switch (gameState)
	{
		case GAME_STATE::menuState:
			DrawMenu();
			break;

		case GAME_STATE::playingState:
			DrawWhilePlaying();
			break;

		case GAME_STATE::pauseState:
			break;

	}

}

void MeshViewApp::DrawWhilePlaying()
{
	//
	// Render the scene to the shadow map.
	//

	mSmap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);

	//DrawSceneToShadowMap();//his may not work

	md3dImmediateContext->RSSetState(0);


	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	mSsao->SetNormalDepthRenderTarget(mDepthStencilView);

	DrawSceneToSsaoNormalDepthMap();//could be broken

	//
	// Now compute the ambient occlusion.
	// may not work

	mSsao->ComputeSsao(mPlayer);
	mSsao->BlurAmbientMap(2);

	//
	// Restore the back and depth buffer and viewport to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { mRenderTargetView };
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);//maybe does not work?

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));

	// We already laid down scene depth to the depth buffer in the Normal/Depth map pass,
	// so we can set the depth comparison test to “EQUALS.”  This prevents any overdraw
	// in this rendering pass, as only the nearest visible pixels will pass this depth
	// comparison test.

	//may notdo anything or just break it
	md3dImmediateContext->OMSetDepthStencilState(RenderStates::EqualsDSS, 0);

	XMMATRIX view = mPlayer.View();
	XMMATRIX proj = mPlayer.Proj();
	XMMATRIX viewProj = mPlayer.ViewProj();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mPlayer.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());
	Effects::BasicFX->SetSsaoMap(mSsao->AmbientSRV());

	ID3DX11EffectTechnique* tech = Effects::BasicFX->Light3TexTech;

	//may not work/do anything
	ID3DX11EffectTechnique* alphaClippedTech = Effects::NormalMapFX->Light3TexAlphaClipTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
     
	//if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(mWireframeRS);

	//
	// Draw opaque objects.
	//
	
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		for (UINT modelIndex = 0; modelIndex < mModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::BasicFX->SetShadowTransform(world*shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for (UINT subset = 0; subset < mModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::BasicFX->SetMaterial(mModelInstances[modelIndex].Model->Mat[subset]);
				Effects::BasicFX->SetDiffuseMap(mModelInstances[modelIndex].Model->DiffuseMapSRV[subset]);
				//Effects::BasicFX->SetNormalMap(mModelInstances[modelIndex].Model->NormalMapSRV[subset]);

				tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				mModelInstances[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
	}
	// Turn off wireframe.
	md3dImmediateContext->RSSetState(0);

	// Restore from RenderStates::EqualsDSS
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Debug view SSAO map.
	//DrawScreenQuad(mSsao->AmbientSRV());

	mSky->Draw(md3dImmediateContext, mPlayer);

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	md3dImmediateContext->PSSetShaderResources(0, 16, nullSRV);

	HR(mSwapChain->Present(0, 0));
}

void MeshViewApp::DrawMenu()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertexes);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		// 36 indices for the box.
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	HR(mSwapChain->Present(0, 0));
}

void MeshViewApp::KeyHandler(float dt)
{
	//
	// Control the  Player
	//
	if (GetAsyncKeyState('W') & 0x8000)
	{
		if (wKey == false)
		{
			//key down

		}
		//key pressed
		mPlayer.Walk(1.0f);

		if (mPlayer.isCollidingFloor || mPlayer.isRunWall)
		{
			mSound->playing[mSound->STEP_1] = true;
			mSound->ChangeVolume(1, 0.1);
			float random = 0.981 + (rand() * 0.000001);
			mSound->ChangeFrequency(1,random);
			//mSound->playing[2] = true;
			//mSound->ChangeVolume(2, 0.1);
		}

		wKey = true;
	}
	else
	{
		if (wKey == true)
		{
			//on key up
			mPlayer.Stop();
		}
		wKey = false;
	}


	if (GetAsyncKeyState('S') & 0x8000)
	{
		if (sKey == false)
		{
			//key down
		}
		//key pressed

		if (mPlayer.isCollidingFloor || mPlayer.isRunWall)
		{
			mSound->playing[2] = true;
			mSound->ChangeVolume(1, 0.1);
			float random = 0.981 + (rand() * 0.000001);
			mSound->ChangeFrequency(1, random);
			mSound->playing[mSound->STEP_2] = true;
			mSound->ChangeVolume(2, 0.1);
		}
		//mSound->playing[2] = true;

		mPlayer.Walk(-1.0f);

		sKey = true;
	}
	else
	{
		if (sKey == true)
		{
			//on key up
			mPlayer.Stop();
		}
		sKey = false;
	}


	if (GetAsyncKeyState('A') & 0x8000)
	{
		if (aKey == false)
		{
			//key down
		}
		//key pressed
		//mPlayer.Strafe(-1.0f);
		aKey = true;
	}
	else
	{
		if (aKey == true)
		{
			//on key up
			mPlayer.Stop();
		}
		aKey = false;
	}


	if (GetAsyncKeyState('D') & 0x8000)
	{
		if (dKey == false)
		{
			//key down
		}
		//key pressed
		//mPlayer.Strafe(1.0f);
		dKey = true;
	}
	else
	{
		if (dKey == true)
		{
			//on key up
			mPlayer.Stop();
		}
		dKey = false;
	}



	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (space == false)
		{
			//key down
			mPlayer.Jump();
			mSound->playing[mSound->GRUNT] = true;
		}
		//key pressed

		space = true;
	}
	else
	{
		if (space == true)
		{
			//on key up
		}
		space = false;
	}

	//for debugging
	//reseting the player
	if ((GetAsyncKeyState('R') & 0x8000))
	{
		mPlayer.SetPosition(0.0f, 8.0f, 0.0f);
		mPlayer.vel.y = 0;
		mPlayer.vel.x = 0;
		mPlayer.vel.z = 0;
	}
}

void MeshViewApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MeshViewApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MeshViewApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mPlayer.Pitch(dy);
		mPlayer.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MeshViewApp::DrawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view     = mPlayer.View();
	XMMATRIX proj     = mPlayer.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormalDepthFX->NormalDepthTech;
	ID3DX11EffectTechnique* alphaClippedTech = Effects::SsaoNormalDepthFX->NormalDepthAlphaClipTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;


	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	//if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(mWireframeRS);

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			for(UINT subset = 0; subset < mModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				mModelInstances[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }

	//this can be reimplemented if we ever want to have objects that are not culled and have alpha in the texture?(or whatever it does)
	/*
	// The alpha tested triangles are leaves, so render them double sided.
	md3dImmediateContext->RSSetState(RenderStates::NoCullRS);
	alphaClippedTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mAlphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mAlphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			
			for(UINT subset = 0; subset < mAlphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::SsaoNormalDepthFX->SetDiffuseMap(mAlphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset]);
				alphaClippedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				mAlphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }
	*/
	md3dImmediateContext->RSSetState(0);
}

void MeshViewApp::DrawSceneToShadowMap()
{
	XMMATRIX view     = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj     = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mPlayer.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	ID3DX11EffectTechnique* tech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* alphaClippedTech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
     
	//if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(mWireframeRS);

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			for(UINT subset = 0; subset < mModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				mModelInstances[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }

	//this can be reimplemented if we ever want to have objects that are not culled and have alpha in the texture?(or whatever it does)
	/*
	alphaClippedTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for(UINT modelIndex = 0; modelIndex < mAlphaClippedModelInstances.size(); ++modelIndex)
		{
			world = XMLoadFloat4x4(&mAlphaClippedModelInstances[modelIndex].World);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

			for(UINT subset = 0; subset < mAlphaClippedModelInstances[modelIndex].Model->SubsetCount; ++subset)
			{
				Effects::BuildShadowMapFX->SetDiffuseMap(mAlphaClippedModelInstances[modelIndex].Model->DiffuseMapSRV[subset]);
				alphaClippedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				mAlphaClippedModelInstances[modelIndex].Model->ModelMesh.Draw(md3dImmediateContext, subset);
			}
		}
    }
	*/
	md3dImmediateContext->RSSetState(0);
}

void MeshViewApp::DrawScreenQuad(ID3D11ShaderResourceView* srv)
{
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
 
	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ID3DX11EffectTechnique* tech = Effects::DebugTexFX->ViewRedTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::DebugTexFX->SetWorldViewProj(world);
		Effects::DebugTexFX->SetTexture(srv);

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
    }
}

void MeshViewApp::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void MeshViewApp::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}

void MeshViewApp::BuildGeometryBuffers()
{
	// Create vertex buffer
	Vertexes vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Red },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::Yellow },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Cyan },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Magenta }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertexes)* 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));


	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}

void MeshViewApp::BuildMenuFX()
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(L"FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	// compilationMsgs can store errors or warnings.
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationMsgs);
	}

	// Even if there are no compilationMsgs, check to make sure there were no other errors.
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, md3dDevice, &mFX));

	// Done with compiled shader.
	ReleaseCOM(compiledShader);

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
}

void MeshViewApp::BuildVertexLayout()
{
	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &mInputLayout));
}
