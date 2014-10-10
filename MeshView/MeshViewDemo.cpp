/**************************      TO DO      ********************************************
	Once you have a model with real textures you may have to mess with the draw function to get it top draw properly
	as none of our models have had textures yet so we dont know if it wirks right
		
	see player classes TO DO
	recored any movment bugs in the player class

	menus - LIKE HOLLY FUCK WE NEED MENU	
		this entire file and a bunch of shit in it is going to have to be changed around 
		Becuase Damain Can't get his dam menu done on time. ARRG!

	level switching and picking(menus)
		
		basicly implement on the fly level switching.
		This is mostly done. a function needs to be implemented that deletes all the data the level is storing
		that way you will not run out of memery after playing many differetn maps withough restarting the game.

	Maybe make and e-num for the difrent levels(not really needed at all)

	Draw a sphere where the win sphere is.
	The code is done but it does not draw. Ithink this has something to do with the way the draw is different in respect to the mesh draw

	Roof and other Collisions

	Unless the player will spawn at the same place(cordanite) in every level it needs to ba asigened to each level

		
	The header part of this could use some orginizing
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
#include "FileWriter.h"

#define EPSILON 0.00001

struct Cube
{
	XMVECTOR pos;
	XMVECTOR originPos;
	XMVECTOR scale;
	XMVECTOR halfSize;
	enum menuButtons { LOGOb, PLAYb, EXITb, SOUNDb, SOUNDbOff, MUSICb, MUSICbOff, BESTRUNS, RETURN, BESTRUNSB, LEVEL01, LEVEL02 };
	menuButtons button;
	XNA::AxisAlignedBox mMeshBox;
	XMFLOAT4X4 localWorld;
	float distanceFromCam;
};

struct GUICube
{
	XMVECTOR pos;
	XMVECTOR scale;
	XMFLOAT4X4 localWorld;
	enum hudTextures{ ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, HASH, COLON };
	hudTextures currentTex;
	XMVECTOR displacement = XMVectorZero();
};

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
	void DrawScreenQuad(ID3D11ShaderResourceView* srv);
	void BuildShadowTransform();
	void BuildScreenQuadGeometryBuffers();
	void BuildGeometryBuffers();
	void BuildMenuFX();
	void BuildVertexLayout();
	void Pick(int sx, int sy);
	bool AreSameVec(XMVECTOR a, XMVECTOR b);

	//Menu creation and draw stuff
	std::vector<Cube*> cubes;
	ID3D11ShaderResourceView* mDiffuseMapSRVMenuButtons[12];
	ID3D11ShaderResourceView* mDiffuseMapSRVGUITex[12];
	void CreateMenu();
	Material mBoxMat;
	XMFLOAT4X4 mTexTransform;
	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;


	void LoadCurrLevel();


	//state based functions
	void UpdateWhilePlaying(float dt);
	void UpdateMainMenu(float dt);
	void DrawWhilePlaying();
	void DrawMenu();

	//misc functions
	void KeyHandler(float dt);

private:
	ID3D11RasterizerState* mCullCorrectionRS;

	TextureMgr mTexMgr;

	Sky* mSky;

	//BasicModel* testModel;
	int currLevel = 0;

	std::vector<BasicModelInstance> mModelInstances;
	//std::vector<BasicModelInstance> mAlphaClippedModelInstances;

	//stuff bellow is needed for my menus
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3D11InputLayout* mInputLayout;
	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	float angleTo;
	float angleCur;
	bool toRuns = false;
	bool toMain = false;
	void ScreenTransition(float);
	std::vector<GUICube*> HSCubes;
	GUICube::hudTextures numberChar(std::vector<std::string> vector, int score, int number);
	
	// Picking stuff
	UINT mPickedTriangle;
	Material mPickedTriangleMat;
	static bool SortByVector(const Cube* lhs, const Cube* rhs) { return lhs->distanceFromCam < rhs->distanceFromCam; }

	//winSphere draw stuff
	ID3D11Buffer* mWinSphereVB;
	ID3D11Buffer* mWinSphereIB;

	UINT mWinSphereIndexCount;
	Material mWinSphereMat;
	ID3D11ShaderResourceView* mWinSphereTexSRV;
	XMFLOAT4X4 mWinSphereWorld;
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
	bool SFXOn = true;

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

	//reset level
	void ResetLevel();

	GAME_STATE gameState;

	//in-game gui functions
	std::vector<GUICube*> guiCubes;
	void InitGUI();
	void UpdateGUI(float dt);
	void DrawGUI();
	bool timerOn = true;
	float seconds = 0;
	float minutes = 0;
	Camera HUDcam;
	void SaveScore();
	FileWriter fileManager;
	//this is a cheap hack bool
	bool newLevelHasStarted = false;
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
 
//there is probably more stuff that can be pre-initilzed
MeshViewApp::MeshViewApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), //currLevel(0),
mScreenQuadVB(0), mScreenQuadIB(0),
  mSmap(0), mSsao(0),
  mLightRotationAngle(0.0f), mBoxVB(0), mBoxIB(0), mFX(0), mTech(0),
  mfxWorldViewProj(0), mInputLayout(0),
  mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f), gameState(GAME_STATE::menuState), fileManager("ParkcoulHighScores.hst")
{
	mMainWndCaption = L"Platformer";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mTexTransform, I);

	mPlayer.SetPosition(0.0f, 0.0f, 0.0f);
	HUDcam.SetPosition(0.0f, 0.0f, 0.0f);

	// Mat for the general blocks
	mBoxMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mBoxMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mDirLights[0].Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient  = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -1.0, 0.0f);

	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;
}

MeshViewApp::~MeshViewApp()
{
	//this needs work
	//LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOTS of work....
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
	
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/LogoBanner.png", 0, 0, &mDiffuseMapSRVMenuButtons[0], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/PlayButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[1], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/ExitButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[2], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/SoundOnButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[3], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/SoundOffButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[4], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/MusicOnButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[5], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/MusicOffButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[6], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/BestRuns.png", 0, 0, &mDiffuseMapSRVMenuButtons[7], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/ReturnButton.png", 0, 0, &mDiffuseMapSRVMenuButtons[8], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/BestRuns.png", 0, 0, &mDiffuseMapSRVMenuButtons[9], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/lvl01.png", 0, 0, &mDiffuseMapSRVMenuButtons[10], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/lvl02.png", 0, 0, &mDiffuseMapSRVMenuButtons[11], 0));

	//GUI textures
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/0Texture.png", 0, 0, &mDiffuseMapSRVGUITex[0], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/1Texture.png", 0, 0, &mDiffuseMapSRVGUITex[1], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/2Texture.png", 0, 0, &mDiffuseMapSRVGUITex[2], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/3Texture.png", 0, 0, &mDiffuseMapSRVGUITex[3], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/4Texture.png", 0, 0, &mDiffuseMapSRVGUITex[4], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/5Texture.png", 0, 0, &mDiffuseMapSRVGUITex[5], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/6Texture.png", 0, 0, &mDiffuseMapSRVGUITex[6], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/7Texture.png", 0, 0, &mDiffuseMapSRVGUITex[7], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/8Texture.png", 0, 0, &mDiffuseMapSRVGUITex[8], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/9Texture.png", 0, 0, &mDiffuseMapSRVGUITex[9], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/#Texture.png", 0, 0, &mDiffuseMapSRVGUITex[10], 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/ColonTexture.png", 0, 0, &mDiffuseMapSRVGUITex[11], 0));

	mSmap = new ShadowMap(md3dDevice, SMapSize, SMapSize);

	mPlayer.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	HUDcam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	mSsao = new Ssao(md3dDevice, md3dImmediateContext, mClientWidth, mClientHeight, mPlayer.GetFovY(), mPlayer.GetFarZ());

	BuildScreenQuadGeometryBuffers();

	//set up the levels so they are ready to load when needed
	Level* lvl = new Level(md3dDevice, &mTexMgr, "Models\\level2.alx", XMFLOAT3(-126.7, -2.3, -85.6), 10.0f); 
	lvl->SetSpawnPoint(XMFLOAT3(0.0f, 10.0f, 0.0f));
	mLevels.push_back(lvl);

	lvl = new Level(md3dDevice, &mTexMgr, "Models\\testMap.alx", XMFLOAT3(-126.7, -2.3, -85.6), 10.0f);
	lvl->SetSpawnPoint(XMFLOAT3(0.0f, 10.0f, 0.0f));
	mLevels.push_back(lvl);

	lvl = new Level(md3dDevice, &mTexMgr, "Models\\level1.alx", XMFLOAT3(-126.7, -2.3, -85.6), 10.0f);
	lvl->SetSpawnPoint(XMFLOAT3(0.0f, 10.0f, 0.0f));
	mLevels.push_back(lvl);

	
	
	
	//revers face culling
	D3D11_RASTERIZER_DESC jimJam;
	ZeroMemory(&jimJam, sizeof(D3D11_RASTERIZER_DESC));
	jimJam.FillMode = D3D11_FILL_SOLID;
	jimJam.CullMode = D3D11_CULL_FRONT;
	jimJam.AntialiasedLineEnable = true;
	jimJam.FrontCounterClockwise = false;
	HR(md3dDevice->CreateRasterizerState(&jimJam, &mCullCorrectionRS));
	md3dImmediateContext->RSSetState(mCullCorrectionRS);
	

	//make the model for the winSphere
	GeometryGenerator::MeshData winSphereData;
	GeometryGenerator gen;
	gen.CreateGeosphere(1.0f, 3, winSphereData);

	mWinSphereIndexCount = winSphereData.Indices.size();

	//buffers
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &winSphereData.Vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWinSphereVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &winSphereData.Indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWinSphereIB));
	//WHERE IT IS 
	float temp = mLevels[currLevel]->GetWinSphere().Radius * 2;
	XMFLOAT3 tempF3 = mLevels[currLevel]->GetWinSphere().Center;
	XMMATRIX scale = XMMatrixScaling(temp, temp, temp);
	XMMATRIX offset = XMMatrixTranslation(tempF3.x, tempF3.y, tempF3.z);
	XMStoreFloat4x4(&mWinSphereWorld, XMMatrixMultiply(scale, offset));
	//texture time BB (This really needs a better texture!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	mWinSphereTexSRV = mTexMgr.CreateTexture(L"Textures/farm_silo_D.png");
	//material? do i even need this?
	mWinSphereMat.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mWinSphereMat.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mWinSphereMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mWinSphereMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);


	//init sound
	mSound = new SoundMgr();
	srand(time(NULL));

	//build menu stuff
	BuildGeometryBuffers();
	//BuildMenuFX();
	
	//BuildVertexLayout();

	CreateMenu();
	

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
	mPlayer.SetPosition(mLevels[currLevel]->GetSpawnPoint());
}

void MeshViewApp::OnResize()
{
	D3DApp::OnResize();

	mPlayer.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	HUDcam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	if( mSsao )
	{
		mSsao->OnSize(mClientWidth, mClientHeight, mPlayer.GetFovY(), mPlayer.GetFarZ());
	}
}

void MeshViewApp::UpdateScene(float dt)
{
	mSound->UpdateSound();

	switch (gameState)
	{
	case GAME_STATE::menuState:
		UpdateMainMenu(dt);
		break;

	case GAME_STATE::playingState:
		UpdateGUI(dt);
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
	
	//get eh player sphere
	XNA::Sphere pSphere= mPlayer.GetBoundingSphere();
	//see if you win
	if (XNA::IntersectSphereSphere(&pSphere, &mLevels[currLevel]->GetWinSphere()) && !newLevelHasStarted)
	{
		if (currLevel + 1 < mLevels.size())
		{
			currLevel++;
		}

		newLevelHasStarted = true;
		SaveScore();
		LoadCurrLevel();
		

		return;
	}

	//check if player has fallen off level
	if (mPlayer.GetPosition().y < -10)
	{
		ResetLevel();
	}

	//Do player collisions: loops throiugh the current levels triangls and does a bunch of stuff

	//reset the collsiion status
	mPlayer.isCollidingFloor = false;
	mPlayer.isCollidingWall = false;
	//setup vars
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0, 1.0, 0.0));
	//make a slightly smaller version of the players sphere 
	XNA::Sphere pSmallSphere;
	pSmallSphere.Center = pSphere.Center;
	pSmallSphere.Radius = pSphere.Radius * 0.9f;//this value may be changed to work better.

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
			//Im pretty sure this does not work because I had to comment out line 3682 of the XNACollision.cpp for it to even compile
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
				//this is for collision that are like ceilings
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

	// SHADOWS DONT WORK RIGHT NOW (you need normal maps or something? look in to it
	// Animate the lights (and hence shadows).
	//

	BuildShadowTransform();

	//this can probably be moved in to the players update
	mPlayer.UpdateViewMatrix();
}

void MeshViewApp::UpdateMainMenu(float dt)
{
	ScreenTransition(dt);
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
	switch (gameState)
	{
		case GAME_STATE::menuState:
			DrawMenu();
			break;

		case GAME_STATE::playingState:
			DrawWhilePlaying();
			DrawGUI();
			break;

		case GAME_STATE::pauseState:

			break;
	}
	HR(mSwapChain->Present(0, 0)); //only call this ONCE!
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
		md3dImmediateContext->RSSetState(mCullCorrectionRS);

	//
	// Draw opaque objects.
	//
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//draw the mesh
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
	
	//HR(mSwapChain->Present(0, 0));
}

void MeshViewApp::DrawMenu()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	// Set constants
	mPlayer.UpdateViewMatrix();

	XMMATRIX view = mPlayer.View();
	XMMATRIX proj = mPlayer.Proj();
	XMMATRIX viewProj = mPlayer.ViewProj();

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mPlayer.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());

	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTexTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		for (int i = 0; i < cubes.size(); i++)
		{
			if (cubes[i] != NULL)
			{
				XMMATRIX world = XMLoadFloat4x4(&cubes[i]->localWorld);
				XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
				XMMATRIX worldViewProj = world*view*proj;

				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
				Effects::BasicFX->SetMaterial(mBoxMat);
				switch (cubes[i]->button) //show texture of cube
				{
				case Cube::LOGOb:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[0]);
					break;
				case Cube::PLAYb:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[1]);
					break;
				case Cube::EXITb:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[2]);
					break;
				case Cube::SOUNDb:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[3]);
					break;
				case Cube::SOUNDbOff:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[4]);
					break;
				case Cube::MUSICb:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[5]);
					break;
				case Cube::MUSICbOff:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[6]);
					break;
				case Cube::BESTRUNS:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[7]);
					break;
				case Cube::RETURN:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[8]);
					break;
				case Cube::LEVEL01:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[10]);
					break;
				case Cube::LEVEL02:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVMenuButtons[11]);
					break;
				}
				activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
				// Restore default
				md3dImmediateContext->RSSetState(0);
				
			}
		}

		for (int i = 0; i < HSCubes.size(); i++)
		{
			if (HSCubes[i] != NULL)
			{
				XMMATRIX world = XMLoadFloat4x4(&HSCubes[i]->localWorld);
				XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
				XMMATRIX worldViewProj = world*view*proj;

				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
				Effects::BasicFX->SetMaterial(mBoxMat);
				// ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, COLON, HASH
				switch (HSCubes[i]->currentTex) //show texture of cube
				{
				case GUICube::ZERO:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[0]);
					break;
				case GUICube::ONE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[1]);
					break;
				case GUICube::TWO:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[2]);
					break;
				case GUICube::THREE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[3]);
					break;
				case GUICube::FOUR:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[4]);
					break;
				case GUICube::FIVE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[5]);
					break;
				case GUICube::SIX:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[6]);
					break;
				case GUICube::SEVEN:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[7]);
					break;
				case GUICube::EIGHT:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[8]);
					break;
				case GUICube::NINE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[9]);
					break;
				case GUICube::HASH:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[10]);
					break;
				case GUICube::COLON:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[11]);
					break;
				}
				activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
				// Restore default
				md3dImmediateContext->RSSetState(0);

			}
		}
		mSky->Draw(md3dImmediateContext, mPlayer); // draw sky
	}
	//HR(mSwapChain->Present(0, 0));
}

void MeshViewApp::KeyHandler(float dt)
{
	if (gameState != GAME_STATE::menuState)
	{
		//
		// Control the  Player
		//
		if (GetAsyncKeyState('W') & 0x8000)
		{
			if (wKey == false)
			{
				//on key down
			}
			//key pressed
			mPlayer.Walk(1.0f);

			if ((mPlayer.isCollidingFloor || mPlayer.isRunWall) && SFXOn)
			{
				mSound->playing[mSound->STEP_1] = true;
				mSound->ChangeVolume(1, 0.1);
				float random = 0.981 + (rand() * 0.000001);
				mSound->ChangeFrequency(1, random);
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
				//on key down
			}
			//key pressed

			if ((mPlayer.isCollidingFloor || mPlayer.isRunWall) && SFXOn)
			{
				mSound->playing[2] = true;
				mSound->ChangeVolume(1, 0.1);
				float random = 0.981 + (rand() * 0.000001);
				mSound->ChangeFrequency(1, random);
				mSound->playing[mSound->STEP_2] = true;
				mSound->ChangeVolume(2, 0.1);
			}

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

			//key pressed
			mPlayer.Strafe(-0.4f);
			if ((mPlayer.isCollidingFloor || mPlayer.isRunWall) && SFXOn)
			{
				mSound->playing[2] = true;
				mSound->ChangeVolume(1, 0.1);
				float random = 0.981 + (rand() * 0.000001);
				mSound->ChangeFrequency(1, random);
				mSound->playing[mSound->STEP_2] = true;
				mSound->ChangeVolume(2, 0.1);
			}
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

			if (GetAsyncKeyState('D') & 0x8000)
			{
				if (dKey == false)
				{
					//key down
				}
				//key pressed
				//mPlayer.Strafe(1.0f);
				dKey = true;
				//key pressed
				mPlayer.Strafe(0.4f);
				if ((mPlayer.isCollidingFloor || mPlayer.isRunWall) && SFXOn)
				{
					mSound->playing[2] = true;
					mSound->ChangeVolume(1, 0.1);
					float random = 0.981 + (rand() * 0.000001);
					mSound->ChangeFrequency(1, random);
					mSound->playing[mSound->STEP_2] = true;
					mSound->ChangeVolume(2, 0.1);
				}
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
				mPlayer.Jump();
				if (space == false && SFXOn)
				{
					//key down
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
				ResetLevel();
			}
		}
	}
}

void MeshViewApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0 && gameState == GAME_STATE::menuState)
	{
		Pick(x, y);
	}

	if (gameState != GAME_STATE::menuState)
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;

		SetCapture(mhMainWnd);
	}
}

void MeshViewApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MeshViewApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (gameState != GAME_STATE::menuState)
	{
		if ((btnState & MK_LBUTTON) != 0)
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
		md3dImmediateContext->RSSetState(mCullCorrectionRS);

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
	GeometryGenerator::MeshData box;
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	UINT totalVertexCount = box.Vertices.size();
	UINT totalIndexCount = mBoxIndexCount;
	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//
	std::vector<Vertex::Basic32> vertices(totalVertexCount);
	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));
	//
	// Pack the indices of all the meshes into one index buffer.
	//
	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
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

	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),0, md3dDevice, &mFX));

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

void MeshViewApp::CreateMenu()
{
	// LOGO
	Cube * logoButton = new Cube; //creates new block
	logoButton->pos = XMVectorSet(0, 1, 3, 1); //set the position in world space for the cube
	logoButton->originPos = logoButton->pos; //set its origin pos for button presses
	logoButton->scale = XMVectorSet(3.0f, 0.4f, 0.00001f, 1.0f); //set the scale of the button
	XMStoreFloat4x4(&logoButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(logoButton->scale), XMMatrixTranslationFromVector(logoButton->pos)));
	XMStoreFloat3(&logoButton->mMeshBox.Center, logoButton->pos); //sets the center of the mesh box for click detection
	logoButton->halfSize = XMVectorSet(1.5f, 0.2f, 0.000005f, 1.0f); // sets the size of the bounding box from the center of the object
	XMStoreFloat3(&logoButton->mMeshBox.Extents, logoButton->halfSize);
	logoButton->button = Cube::LOGOb; //sets the texture of button; 
	MeshViewApp::cubes.push_back(logoButton); //adds the play button to the array of cubes to draw

	// Play button
	Cube * playButton = new Cube; 
	playButton->pos = XMVectorSet(0, 0.5, 3, 1); 
	playButton->originPos = playButton->pos; 
	playButton->scale = XMVectorSet(1.0f, 0.28f, 0.00001f, 1.0f); 
	XMStoreFloat4x4(&playButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(playButton->scale), XMMatrixTranslationFromVector(playButton->pos)));
	XMStoreFloat3(&playButton->mMeshBox.Center, playButton->pos); 
	playButton->halfSize = XMVectorSet(0.5f, 0.14f, 0.000005f, 1.0f);
	XMStoreFloat3(&playButton->mMeshBox.Extents, playButton->halfSize);
	playButton->button = Cube::PLAYb; 
	MeshViewApp::cubes.push_back(playButton);

	// Exit button
	Cube * exitButton = new Cube;
	exitButton->pos = XMVectorSet(1.2, -1, 3, 1);
	exitButton->originPos = exitButton->pos;
	exitButton->scale = XMVectorSet(0.6f, 0.2f, 0.00001f, 1.0f);
	XMStoreFloat4x4(&exitButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(exitButton->scale), XMMatrixTranslationFromVector(exitButton->pos)));
	XMStoreFloat3(&exitButton->mMeshBox.Center, exitButton->pos);
	exitButton->halfSize = XMVectorSet(0.3f, 0.1f, 0.000005f, 1.0f);
	XMStoreFloat3(&exitButton->mMeshBox.Extents, exitButton->halfSize);
	exitButton->button = Cube::EXITb;
	MeshViewApp::cubes.push_back(exitButton);

	// Sound button
	Cube * soundButton = new Cube;
	soundButton->pos = XMVectorSet(-1.2, -0.7, 3, 1);
	soundButton->originPos = soundButton->pos;
	soundButton->scale = XMVectorSet(0.6f, 0.2f, 0.00001f, 1.0f);
	XMStoreFloat4x4(&soundButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(soundButton->scale), XMMatrixTranslationFromVector(soundButton->pos)));
	XMStoreFloat3(&soundButton->mMeshBox.Center, soundButton->pos);
	soundButton->halfSize = XMVectorSet(0.3f, 0.1f, 0.000005f, 1.0f);
	XMStoreFloat3(&soundButton->mMeshBox.Extents, soundButton->halfSize);
	soundButton->button = Cube::SOUNDb;
	MeshViewApp::cubes.push_back(soundButton);

	// Music button
	Cube * musicButton = new Cube;
	musicButton->pos = XMVectorSet(-1.2, -1, 3, 1);
	musicButton->originPos = musicButton->pos;
	musicButton->scale = XMVectorSet(0.6f, 0.2f, 0.00001f, 1.0f);
	XMStoreFloat4x4(&musicButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(musicButton->scale), XMMatrixTranslationFromVector(musicButton->pos)));
	XMStoreFloat3(&musicButton->mMeshBox.Center, musicButton->pos);
	musicButton->halfSize = XMVectorSet(0.3f, 0.1f, 0.000005f, 1.0f);
	XMStoreFloat3(&musicButton->mMeshBox.Extents, musicButton->halfSize);
	musicButton->button = Cube::MUSICb;
	MeshViewApp::cubes.push_back(musicButton);

	// Highscores button
	Cube * scoresButton = new Cube;
	scoresButton->pos = XMVectorSet(1.2, -0.7, 3, 1);
	scoresButton->originPos = scoresButton->pos;
	scoresButton->scale = XMVectorSet(0.6f, 0.2f, 0.00001f, 1.0f);
	XMStoreFloat4x4(&scoresButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(scoresButton->scale), XMMatrixTranslationFromVector(scoresButton->pos)));
	XMStoreFloat3(&scoresButton->mMeshBox.Center, scoresButton->pos);
	scoresButton->halfSize = XMVectorSet(0.3f, 0.1f, 0.000005f, 1.0f);
	XMStoreFloat3(&scoresButton->mMeshBox.Extents, scoresButton->halfSize);
	scoresButton->button = Cube::BESTRUNS;
	MeshViewApp::cubes.push_back(scoresButton);

	// which level to play block
	Cube * levelBlock = new Cube;
	levelBlock->pos = XMVectorSet(0, 0, 3, 1);
	levelBlock->originPos = levelBlock->pos;
	levelBlock->scale = XMVectorSet(0.8f, 0.28f, 0.00001f, 1.0f);
	XMStoreFloat4x4(&levelBlock->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(levelBlock->scale), XMMatrixTranslationFromVector(levelBlock->pos)));
	XMStoreFloat3(&levelBlock->mMeshBox.Center, levelBlock->pos);
	levelBlock->halfSize = XMVectorSet(0.4f, 0.14f, 0.000005f, 1.0f);
	XMStoreFloat3(&levelBlock->mMeshBox.Extents, levelBlock->halfSize);
	levelBlock->button = Cube::LEVEL01;
	MeshViewApp::cubes.push_back(levelBlock);

	//HIGHSCORE AREA------------------------------------------------------------
	// Highscores Banner
	Cube * scoresBanner = new Cube;
	scoresBanner->pos = XMVectorSet(3, 1, 0, 1);
	scoresBanner->originPos = scoresBanner->pos;
	scoresBanner->scale = XMVectorSet(0.00001f, 0.4f, 3.0f, 1.0f);
	XMStoreFloat4x4(&scoresBanner->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(scoresBanner->scale), XMMatrixTranslationFromVector(scoresBanner->pos)));
	XMStoreFloat3(&scoresBanner->mMeshBox.Center, scoresBanner->pos);
	scoresBanner->halfSize = XMVectorSet(0.000005f, 0.2f, 1.5f, 1.0f);
	XMStoreFloat3(&scoresBanner->mMeshBox.Extents, scoresBanner->halfSize);
	scoresBanner->button = Cube::BESTRUNSB;
	MeshViewApp::cubes.push_back(scoresBanner);

	// Return button highscore screen
	Cube * returnButton = new Cube;
	returnButton->pos = XMVectorSet(3, -1, 1.2, 1);
	returnButton->originPos = returnButton->pos;
	returnButton->scale = XMVectorSet(0.00001f, 0.2f, 0.6f, 1.0f);
	XMStoreFloat4x4(&returnButton->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(returnButton->scale), XMMatrixTranslationFromVector(returnButton->pos)));
	XMStoreFloat3(&returnButton->mMeshBox.Center, returnButton->pos);
	returnButton->halfSize = XMVectorSet(0.000005f, 0.1f, 0.3f, 1.0f);
	XMStoreFloat3(&returnButton->mMeshBox.Extents, returnButton->halfSize);
	returnButton->button = Cube::RETURN;
	MeshViewApp::cubes.push_back(returnButton);

	// Pounds and Numbers
	GUICube::hudTextures numbers[5] = { GUICube::ONE, GUICube::TWO, GUICube::THREE, GUICube::FOUR, GUICube::FIVE };
	std::vector<std::string> scoresFromFile;
	scoresFromFile.push_back(fileManager.ReadData("first")[0]);
	scoresFromFile.push_back(fileManager.ReadData("second")[0]);
	scoresFromFile.push_back(fileManager.ReadData("third")[0]);
	scoresFromFile.push_back(fileManager.ReadData("fourth")[0]);
	scoresFromFile.push_back(fileManager.ReadData("fifth")[0]);

	for (int i = 1; i < 6; ++i)
	{
		GUICube* hash = new GUICube; //creates new block
		hash->pos = XMVectorSet(3, 1 - (0.35 * i), 0.8, 1); //set the position in world space for the cube
		hash->scale = XMVectorSet(0.00001f, 0.2f, 0.2f, 1.0f); //set the scale of the button
		XMStoreFloat4x4(&hash->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(hash->scale), XMMatrixTranslationFromVector(hash->pos)));
		hash->currentTex = GUICube::HASH; //sets the texture of button; 
		MeshViewApp::HSCubes.push_back(hash); //adds the play button to the array of cubes to draw

		GUICube* number = new GUICube;
		number->pos = XMVectorSet(3, 1 - (0.35 * i), 0.6, 1);
		number->scale = XMVectorSet(0.00001f, 0.2f, 0.2f, 1.0f);
		XMStoreFloat4x4(&number->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(number->scale), XMMatrixTranslationFromVector(number->pos)));
		number->currentTex = numbers[i - 1];
		MeshViewApp::HSCubes.push_back(number);

		GUICube* colon = new GUICube;
		colon->pos = XMVectorSet(3, 1 - (0.35 * i), -0.4, 1);
		colon->scale = XMVectorSet(0.00001f, 0.2f, 0.2f, 1.0f);
		XMStoreFloat4x4(&colon->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(colon->scale), XMMatrixTranslationFromVector(colon->pos)));
		colon->currentTex = GUICube::COLON;
		MeshViewApp::HSCubes.push_back(colon);

		for (int j = 1; j < 5; j++)
		{
			GUICube* score = new GUICube;
			if (j > 2)
			{
				score->pos = XMVectorSet(3, 1 - (0.35 * i), 0.0 - (0.2 * j), 1);
			}
			else
			{
				score->pos = XMVectorSet(3, 1 - (0.35 * i), 0.2 - (0.2 * j), 1);
			}
			score->scale = XMVectorSet(0.00001f, 0.2f, 0.2f, 1.0f);
			XMStoreFloat4x4(&score->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(score->scale), XMMatrixTranslationFromVector(score->pos)));
			score->currentTex = numberChar(scoresFromFile, i - 1, j - 1);
			MeshViewApp::HSCubes.push_back(score);
		}
	}
}

GUICube::hudTextures MeshViewApp::numberChar(std::vector<std::string> vector, int score, int number)
{
	switch (vector[score][number])
	{
	case '0':
		return GUICube::ZERO;
	case '1':
		return GUICube::ONE;
	case '2':
		return GUICube::TWO;
	case '3':
		return GUICube::THREE;
	case '4':
		return GUICube::FOUR;
	case '5':
		return GUICube::FIVE;
	case '6':
		return GUICube::SIX;
	case '7':
		return GUICube::SEVEN;
	case '8':
		return GUICube::EIGHT;
	case '9':
		return GUICube::NINE;
	}
	return GUICube::COLON;
}

void MeshViewApp::Pick(int sx, int sy)
{
	XMMATRIX P = mPlayer.Proj();

	// Compute picking ray in view space.
	float vx = (+2.0f*sx / mClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f*sy / mClientHeight + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = mPlayer.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMMATRIX toLocal = invView;

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	// so do the ray/triangle tests.
	//
	// If we did not hit the bounding box, then it is impossible that we hit 
	// the Mesh, so do not waste effort doing ray/triangle tests.

	mPickedTriangle = -1;// Assume we have not picked anything yet, so init to -1.
	float tmin = 0.0f;
	std::vector<Cube*> cubesTouched;

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);
	for (int i = 0; i < cubes.size(); i++)
	{
		if (cubes[i] != NULL)
		{
			// Make the ray direction unit length for the intersection tests.
			rayDir = XMVector3Normalize(rayDir);
			if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &MeshViewApp::cubes[i]->mMeshBox, &tmin))
			{
				XMVECTOR temp = XMVector3Length(mPlayer.GetPositionXM() - cubes[i]->pos);
				cubes[i]->distanceFromCam = XMVectorGetIntX(temp);
				cubesTouched.push_back(cubes[i]);
			}
		}
	}

	if (gameState == GAME_STATE::menuState)
	{
		for (int i = 0; i < cubesTouched.size(); i++)
		{
			if (!cubesTouched.empty())
			{
				std::sort(cubesTouched.begin(), cubesTouched.end(), SortByVector);
				int size = cubesTouched.size() - 1;
				switch (cubesTouched[size]->button)
				{
				case Cube::PLAYb:
					gameState = GAME_STATE::playingState;
					if (cubes[6]->button == Cube::LEVEL02)
					{
						currLevel++;
					}
					LoadCurrLevel();
					ResetLevel();
					InitGUI();
					break;
				case Cube::EXITb:
					PostQuitMessage(0);
					break;
				case Cube::SOUNDb:
					SFXOn = false;
					cubes[3]->button = Cube::SOUNDbOff;
					break;
				case Cube::MUSICb:
					mSound->musicChannel->setMute(true);
					cubes[4]->button = Cube::MUSICbOff;
					break;
				case Cube::SOUNDbOff:
					SFXOn = true;
					cubes[3]->button = Cube::SOUNDb;
					break;
				case Cube::MUSICbOff:
					mSound->musicChannel->setMute(false);
					cubes[4]->button = Cube::MUSICb;
					break;
				case Cube::BESTRUNS:
					angleTo = 1.57;
					angleCur = 0;
					toRuns = true;
					break;
				case Cube::RETURN:
					angleTo = 0;
					angleCur = 1.57;
					toMain = true;
					break;
				case Cube::LEVEL01:
					cubes[6]->button = Cube::LEVEL02;
					break;
				case Cube::LEVEL02:
					cubes[6]->button = Cube::LEVEL01;
					break;
				default:
					break;
				}
			}
		}
	}
}

bool MeshViewApp::AreSameVec(XMVECTOR a, XMVECTOR b)
{
	if (XMVectorGetIntX(a) - XMVectorGetIntX(b) < EPSILON &&
		XMVectorGetIntY(a) - XMVectorGetIntY(b) < EPSILON &&
		XMVectorGetIntZ(a) - XMVectorGetIntZ(b) < EPSILON)
	{
		return true;
	}
	else
		return false;
}

void MeshViewApp::ResetLevel()
{
	mPlayer.SetPosition(mLevels[currLevel]->GetSpawnPoint());
	mPlayer.SetPlayerRotation(XMConvertToRadians(75));
	//mPlayer.RotateY(XMConvertToRadians(-90));
	//TODO set rotation to specific float3x3
	mPlayer.vel.y = 0;
	mPlayer.vel.x = 0;
	mPlayer.vel.z = 0;
}

void MeshViewApp::ScreenTransition(float dt)
{
	if (toRuns && angleCur < angleTo)
	{
		angleCur += 1.5 * dt;
		mPlayer.RotateY(1.5 * dt);
	}

	if (toMain && angleCur > angleTo)
	{
		angleCur -= 1.5 * dt;
		mPlayer.RotateY(1.5 * -dt);
	}
}

void MeshViewApp::InitGUI()
{
	// first minute digit
	GUICube* MinuteOne = new GUICube; //creates new block
	MinuteOne->pos = XMVectorSet(0, 1, 3, 1); //set the position in world space for the cube
	MinuteOne->scale = XMVectorSet(0.1f, 0.1f, 0.001f, 1.0f); //set the scale of the button
	XMStoreFloat4x4(&MinuteOne->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(MinuteOne->scale), XMMatrixTranslationFromVector(MinuteOne->pos)));
	MinuteOne->currentTex = GUICube::ZERO; //sets the texture of button; 
	MinuteOne->displacement = XMVectorSet(-0.75f, 0.55f, 1.5f, 0);
	MeshViewApp::guiCubes.push_back(MinuteOne); //adds the play button to the array of cubes to draw

	// second minute digit
	GUICube * MinuteTWO = new GUICube;
	MinuteTWO->pos = XMVectorSet(0, 1, 3, 1);
	MinuteTWO->scale = XMVectorSet(0.1f, 0.1f, 0.001f, 1.0f);
	XMStoreFloat4x4(&MinuteTWO->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(MinuteTWO->scale), XMMatrixTranslationFromVector(MinuteTWO->pos)));
	MinuteTWO->currentTex = GUICube::ZERO;
	MinuteTWO->displacement = XMVectorSet(-0.65f, 0.55f, 1.5f, 0);
	MeshViewApp::guiCubes.push_back(MinuteTWO);

	// colon inbetween minutes and second
	GUICube * colon = new GUICube;
	colon->pos = XMVectorSet(0, 1, 3, 1);
	colon->scale = XMVectorSet(0.1f, 0.1f, 0.001f, 1.0f);
	XMStoreFloat4x4(&colon->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(colon->scale), XMMatrixTranslationFromVector(colon->pos)));
	colon->currentTex = GUICube::COLON;
	colon->displacement = XMVectorSet(-0.55f, 0.55f, 1.5f, 0);
	MeshViewApp::guiCubes.push_back(colon);

	// first second digit
	GUICube * secondOne = new GUICube;
	secondOne->pos = XMVectorSet(0, 1, 3, 1);
	secondOne->scale = XMVectorSet(0.1f, 0.1f, 0.001f, 1.0f);
	XMStoreFloat4x4(&secondOne->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(secondOne->scale), XMMatrixTranslationFromVector(secondOne->pos)));
	secondOne->currentTex = GUICube::ZERO;
	secondOne->displacement = XMVectorSet(-0.45f, 0.55f, 1.5f, 0);
	MeshViewApp::guiCubes.push_back(secondOne);

	// second second digit
	GUICube * secondTwo = new GUICube;
	secondTwo->pos = XMVectorSet(0, 1, 3, 1);
	secondTwo->scale = XMVectorSet(0.1f, 0.1f, 0.001f, 1);
	XMStoreFloat4x4(&secondTwo->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(secondTwo->scale), XMMatrixTranslationFromVector(secondTwo->pos)));
	secondTwo->currentTex = GUICube::ZERO;
	secondTwo->displacement = XMVectorSet(-0.35f, 0.55f, 1.5f, 0);
	MeshViewApp::guiCubes.push_back(secondTwo);
}

void MeshViewApp::UpdateGUI(float dt)
{
	for (int i = 0; i < guiCubes.size(); i++)
	{
		guiCubes[i]->pos = (XMLoadFloat3(&HUDcam.GetPosition()) + guiCubes[i]->displacement);
		XMStoreFloat4x4(&guiCubes[i]->localWorld, XMMatrixMultiply(XMMatrixScalingFromVector(guiCubes[i]->scale), XMMatrixTranslationFromVector(guiCubes[i]->pos)));
	}
	if (timerOn)
	{
		seconds += dt;
		if (seconds >= 1 && newLevelHasStarted)
		{
			newLevelHasStarted = false;
		}

		if (seconds > 10)
		{
			seconds = 0;
			guiCubes[3]->currentTex = (GUICube::hudTextures)(1 + (int)guiCubes[3]->currentTex);
			if (guiCubes[3]->currentTex == GUICube::SIX)
			{
				seconds = 0;
				guiCubes[3]->currentTex = GUICube::ZERO;
				guiCubes[1]->currentTex = (GUICube::hudTextures)(1 + (int)guiCubes[1]->currentTex);
				if (guiCubes[1]->currentTex == GUICube::HASH)
				{
					guiCubes[1]->currentTex = GUICube::ZERO;
					guiCubes[0]->currentTex = (GUICube::hudTextures)(1 + (int)guiCubes[0]->currentTex);

				}
			}
			if (guiCubes[0]->currentTex == GUICube::HASH)
			{
				guiCubes[0]->currentTex = GUICube::NINE;
				guiCubes[1]->currentTex = GUICube::NINE;
				guiCubes[3]->currentTex = GUICube::FIVE;
				seconds = 9;
				timerOn = false;
			}
		}
		int temp = seconds;
		switch (temp)
		{
		case 0:
			guiCubes[4]->currentTex = GUICube::ZERO;
			break;
		case 1:
			guiCubes[4]->currentTex = GUICube::ONE;
			break;
		case 2:
			guiCubes[4]->currentTex = GUICube::TWO;
			break;
		case 3:
			guiCubes[4]->currentTex = GUICube::THREE;
			break;
		case 4:
			guiCubes[4]->currentTex = GUICube::FOUR;
			break;
		case 5:
			guiCubes[4]->currentTex = GUICube::FIVE;
			break;
		case 6:
			guiCubes[4]->currentTex = GUICube::SIX;
			break;
		case 7:
			guiCubes[4]->currentTex = GUICube::SEVEN;
			break;
		case 8:
			guiCubes[4]->currentTex = GUICube::EIGHT;
			break;
		case 9:
			guiCubes[4]->currentTex = GUICube::NINE;
			break;
		}
	}
}

void MeshViewApp::DrawGUI()
{
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	// Set constants
	HUDcam.UpdateViewMatrix();

	XMMATRIX view = HUDcam.View();
	XMMATRIX proj = HUDcam.Proj();
	XMMATRIX viewProj = HUDcam.ViewProj();

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(HUDcam.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());

	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTexTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

		for (int i = 0; i < guiCubes.size(); i++)
		{
			if (guiCubes[i] != NULL)
			{
				XMMATRIX world = XMLoadFloat4x4(&guiCubes[i]->localWorld);
				XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
				XMMATRIX worldViewProj = world*view*proj;
				//mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

				Effects::BasicFX->SetWorld(world);
				Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::BasicFX->SetWorldViewProj(worldViewProj);
				Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
				Effects::BasicFX->SetMaterial(mBoxMat);
				switch (guiCubes[i]->currentTex) //show texture of cube
				{
				case GUICube::ZERO:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[0]);
					break;
				case GUICube::ONE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[1]);
					break;
				case GUICube::TWO:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[2]);
					break;
				case GUICube::THREE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[3]);
					break;
				case GUICube::FOUR:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[4]);
					break;
				case GUICube::FIVE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[5]);
					break;
				case GUICube::SIX:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[6]);
					break;
				case GUICube::SEVEN:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[7]);
					break;
				case GUICube::EIGHT:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[8]);
					break;
				case GUICube::NINE:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[9]);
					break;
				case GUICube::HASH:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[10]);
					break;
				case GUICube::COLON:
					Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRVGUITex[11]);
					break;
				}
				activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

				// Restore default
				md3dImmediateContext->RSSetState(0);
			}
		}
	}
	//HR(mSwapChain->Present(0, 0));
}

void MeshViewApp::SaveScore()
{
	std::string score = std::to_string(guiCubes[0]->currentTex) +
						std::to_string(guiCubes[1]->currentTex) +
						std::to_string(guiCubes[3]->currentTex) +
						std::to_string(guiCubes[4]->currentTex);
	//std::stringstream temp;
	//temp << score;

	//create a temp vector and push the saved highscores into it.
	std::vector<std::string> scoresFromFile;
	scoresFromFile.push_back(fileManager.ReadData("first")[0]);
	scoresFromFile.push_back(fileManager.ReadData("second")[0]);
	scoresFromFile.push_back(fileManager.ReadData("third")[0]);
	scoresFromFile.push_back(fileManager.ReadData("fourth")[0]);
	scoresFromFile.push_back(fileManager.ReadData("fifth")[0]);
	//add the new score to the vector list.
	scoresFromFile.push_back(score);
	//sort the vector list from smallest to biggest
	std::sort(scoresFromFile.begin(), scoresFromFile.end());

	//write the top 5 scores into the top five places in the save file.
	fileManager.WriteData("first", scoresFromFile[0]);
	fileManager.WriteData("second", scoresFromFile[1]);
	fileManager.WriteData("third", scoresFromFile[2]);
	fileManager.WriteData("fourth", scoresFromFile[3]);
	fileManager.WriteData("fifth", scoresFromFile[4]);

	//set timer vars back
	seconds = 0;
	guiCubes[0]->currentTex = GUICube::ZERO;
	guiCubes[1]->currentTex = GUICube::ZERO;
	guiCubes[3]->currentTex = GUICube::ZERO;
	guiCubes[4]->currentTex = GUICube::ZERO;
	seconds = 0;
	timerOn = true;

}
