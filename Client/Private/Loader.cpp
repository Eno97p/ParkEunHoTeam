#include "stdafx.h"
#include "..\Public\Loader.h"




#include"Import_Class.h"	// <---여기에 들어가서 헤더 넣으셈

#include "Legionnaire_Gun.h"
#include "Body_LGGun.h"
#include "Weapon_Gun_LGGun.h"
#include "Weapon_Sword_LGGun.h"
#include "Weapon_Arrow_LGGun.h"

#include "Ghost.h"
#include "Body_Ghost.h"
#include "Weapon_Ghost.h"

#include "Homonculus.h"
#include "Body_Homonculus.h"
#include "Weapon_Homonculus.h"

#include "TargetLock.h"

#pragma endregion Monster

#pragma region ITEM
#include "Item.h"
#pragma endregion ITEM

#pragma region DECAL
#include "Decal.h"
#pragma endregion DECAL

#include "Map_Element.h"
#include "Passive_Element.h"
#include "Deco_Element.h"
#include "Active_Element.h"
#include "TutorialMapBridge.h"

#include "FakeWall.h"
#include "Elevator.h"
#include "RotateGate.h"
#include "EventTrigger.h"

#include"CHoverboard.h"



CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_iFinishedThreadCount{ 0 }

{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	for (int i = 0; i < MAX_THREAD; ++i)
		m_bIsFinish[i] = false;

}

_uint APIENTRY Loading_Main(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_MapData(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading_Map()))
		return 1;

	CoUninitialize();

	return 0;
}

_uint APIENTRY Loading_ShaderData(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading_Shader()))
		return 1;

	CoUninitialize();

	return 0;
}







HRESULT CLoader::Initialize(LEVEL eNextLevel)
{

	m_eNextLevel = eNextLevel;

	for (_uint i = 0; i < USED_THREAD_COUNT; ++i)
		InitializeCriticalSection(&m_Critical_Section[i]);

	m_hThread[0] = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread[0])
		return E_FAIL;

	m_hThread[1] = (HANDLE)_beginthreadex(nullptr, 0, Loading_MapData, this, 0, nullptr);
	if (0 == m_hThread[1])
		return E_FAIL;

	m_hThread[2] = (HANDLE)_beginthreadex(nullptr, 0, Loading_ShaderData, this, 0, nullptr);
	if (0 == m_hThread[2])
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section[0]);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;	
	case LEVEL_ACKBAR:
		hr = Loading_For_AckbarLevel();
		break;
	case LEVEL_JUGGLAS:
		hr = Loading_For_JugglasLevel();
		break;
	}
	LeaveCriticalSection(&m_Critical_Section[0]);

	if (FAILED(hr))
		return E_FAIL;

	Finish_Thread(0);

	return S_OK;
}

HRESULT CLoader::Loading_Map()
{
	EnterCriticalSection(&m_Critical_Section[1]);
	HRESULT			hr{};


	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		break;
	case LEVEL_GAMEPLAY:
	{

		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_static"),
		//	CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/TutorialMap/TutorialMap.fbx", TEXT("../Bin/MapData/Stage_Tutorial.bin")))))
		//	return hr = E_FAIL;
		

		//PHYSX TUTORIAL MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Tutorial_PhysX.bin";
		HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte(0);

		// 먼저 map의 크기(고유한 모델의 수)를 읽습니다.
		size_t mapSize = 0;
		ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);
		if (0 == dwByte)
		{
			CloseHandle(hFile);
			return S_OK; // 파일이 비어있는 경우
		}

		// 각 모델에 대한 정보를 읽습니다.
		for (size_t i = 0; i < mapSize; ++i)
		{
			char szModelName[MAX_PATH] = "";
			char szModelPath[MAX_PATH] = "";

			ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;

			ReadFile(hFile, szModelPath, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
				return hr = E_FAIL;

		}

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PhysX_BasicDonut"),
			CPhysXComponent_static::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
			return hr = E_FAIL;

		CloseHandle(hFile);

		MSG_BOX("Tutorial PhysX Data Loaded");
	}
	break;
	case LEVEL_ACKBAR: //JUGGLAS CASTLE
	{

		//PHYSX JUGGLAS MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Ackbar_PhysX.bin";
		HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte(0);

		// 먼저 map의 크기(고유한 모델의 수)를 읽습니다.
		size_t mapSize = 0;
		ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);
		if (0 == dwByte)
		{
			CloseHandle(hFile);
			return S_OK; // 파일이 비어있는 경우
		}

		// 각 모델에 대한 정보를 읽습니다.
		for (size_t i = 0; i < mapSize; ++i)
		{
			char szModelName[MAX_PATH] = "";
			char szModelPath[MAX_PATH] = "";

			ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;

			ReadFile(hFile, szModelPath, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Ackbar.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);

		MSG_BOX("Ackbar PhysX Data Loaded");
	}
	break;
	case LEVEL_JUGGLAS: //JUGGLAS CASTLE
	{

		//PHYSX JUGGLAS MAP
		const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Jugglas_PhysX.bin";
		HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD dwByte(0);

		// 먼저 map의 크기(고유한 모델의 수)를 읽습니다.
		size_t mapSize = 0;
		ReadFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);
		if (0 == dwByte)
		{
			CloseHandle(hFile);
			return S_OK; // 파일이 비어있는 경우
		}

		// 각 모델에 대한 정보를 읽습니다.
		for (size_t i = 0; i < mapSize; ++i)
		{
			char szModelName[MAX_PATH] = "";
			char szModelPath[MAX_PATH] = "";

			ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;

			ReadFile(hFile, szModelPath, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			if (0 == dwByte)
				break;


			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, const_char_to_wstring(szModelName),
				CPhysXComponent_static::Create(m_pDevice, m_pContext, szModelPath, TEXT("../Bin/MapData/Stage_Jugglas.bin")))))
				return hr = E_FAIL;

		}

		CloseHandle(hFile);

		MSG_BOX("Jugglas PhysX Data Loaded");
	}
	break;
	}

	LeaveCriticalSection(&m_Critical_Section[1]);


	if (FAILED(hr))
		return E_FAIL;



	Finish_Thread(1);
	return S_OK;
}

HRESULT CLoader::Loading_Shader()
{
	EnterCriticalSection(&m_Critical_Section[2]);
	HRESULT			hr{};


	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel_For_Shader();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel_For_Shader();
		break;
	case LEVEL_ACKBAR:
		hr = Loading_For_AckbarLevel_For_Shader();
		break;
	case LEVEL_JUGGLAS:
		hr = Loading_For_JugglasLevel_For_Shader();
		break;
	}
	//예시:case LEVEL_BOSS:
	//	
	//	
	//	
	//	
	//	
	//	
	//
	//	break;







	LeaveCriticalSection(&m_Critical_Section[2]);


	if (FAILED(hr))
		return E_FAIL;

	Finish_Thread(2);
	return S_OK;

}




HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	/* For.Prototype_Component_Shader_VtxUITex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxUITex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));


	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel_For_Shader()
{
	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;


#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	//// Prototype_Component_Model_TronesT02
	PreTransformMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TronesT02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT02/TronesT02.fbx", PreTransformMatrix))))
		return E_FAIL;

	//// Prototype_Component_Model_TronesT03
	PreTransformMatrix =/* XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TronesT03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT03/TronesT03.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ANDRAS ARENA
	//ANDRAS ARENA
	//ANDRAS ARENA
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AndrasTEST"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/AndrasTEST/AndrasTEST.fbx", PreTransformMatrix))))
	//	return E_FAIL;


	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
	//TUTORIAL MAP ELEMENTS @@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMap.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMapBridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMapBridge.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoStructure"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoStructure.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoCubes"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoCubes.fbx", PreTransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialDecoMaze"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoMaze.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;


	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   DECO ELEMENTS

#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Elevator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Elevator/Elevator.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f)); 

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Juggulus
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Juggulus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Juggulus.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHammer
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Hammer.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandOne
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandOne"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_1.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandTwo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandTwo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_2.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandThree
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandThree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_3.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Aspiration
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Aspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Sphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	// CircleSphere
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CircleSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL

	//Trap
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hachoir"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Traps/Hachoir/Hachoir.fbx", PreTransformMatrix))))
		return E_FAIL;
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SmashingPillar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Traps/SmashingPillar/SmashingPillar.fbx", PreTransformMatrix))))
		return E_FAIL;




	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));

	/* For.Prototype_GameObject_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FreeCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ThirdPersonCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThirdPersonCamera"),
		CThirdPersonCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SideViewCamera*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SideViewCamera"),
		CSideViewCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster ::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;


	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Distortion"),
		CDistortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Clone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Clone"),
		CClone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WhisperSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword"),
		CWhisperSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_WhisperSword_Anim */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WhisperSword_Anim"),
		CWhisperSword_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cendres */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cendres"),
		CCendres::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CorruptedSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CorruptedSword"),
		CCorruptedSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Catharsis */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Catharsis"),
		CCatharsis::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DurgaSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DurgaSword"),
		CDurgaSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_IceBlade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBlade"),
		CIceBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_NaruehSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NaruehSword"),
		CNaruehSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PretorianSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PretorianSword"),
		CPretorianSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RadamantheSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RadamantheSword"),
		CRadamantheSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SitraSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SitraSword"),
		CSitraSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ValnirSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ValnirSword"),
		CValnirSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_VeilleurSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VeilleurSword"),
		CVeilleurSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Passive_Element */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Passive_Element"),
		CPassive_Element::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Deco_Element */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Deco_Element"),
		CDeco_Element::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Active_Element */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Active_Element"),
		CActive_Element::Create(m_pDevice, m_pContext))))
		return E_FAIL;



#pragma region Active Element

	// Prototype_GameObject_TutorialMapBridge
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TutorialMapBridge"),
		CTutorialMapBridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// Prototype_GameObject_FakeWall
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FakeWall"),
		CFakeWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	// Prototype_GameObject_Elevator
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elevator"),
		CElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Prototype_GameObject_RotateGate
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotateGate"),
		CRotateGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Prototype_GameObject_EventTrigger
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EventTrigger"),
		CEventTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion Active Element

#pragma region Monster
	/* For.Prototype_GameObject_Boss_Juggulus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Juggulus"),
		CBoss_Juggulus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Juggulus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Juggulus"),
		CBody_Juggulus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_Hammer"),
		CJuggulus_Hammer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandOne */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandOne"),
		CJuggulus_HandOne::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandTwo */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandTwo"),
		CJuggulus_HandTwo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Juggulus_HandThree */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Juggulus_HandThree"),
		CJuggulus_HandThree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Aspiration */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aspiration"),
		CAspiration::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sphere"),
		CSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CircleSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CircleSphere"),
		CCircleSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Mantari */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mantari"),
		CMantari::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Mantari */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Mantari"),
		CBody_Mantari::Create(m_pDevice, m_pContext)))) // 조심해라 김보민 가만안둬 (괴도루팡)
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Mantari */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Mantari"),
		CWeapon_Mantari::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Legionnaire */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Legionnaire"),
		CLegionnaire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Legionnaire */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Legionnaire"),
		CBody_Legionnaire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Legionnaire */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Legionnaire"),
		CWeapon_Legionnaire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Legionnaire_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Legionnaire_Gun"),
		CLegionnaire_Gun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_LGGun"),
		CBody_LGGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Gun_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Gun_LGGun"),
		CWeapon_Gun_LGGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Sword_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Sword_LGGun"),
		CWeapon_Sword_LGGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Arrow_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Arrow_LGGun"),
		CWeapon_Arrow_LGGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ghost */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ghost"),
		CGhost::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Ghost */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Ghost"),
		CBody_Ghost::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Ghost */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Ghost"),
		CWeapon_Ghost::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Homonculus"),
		CHomonculus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Homonculus"),
		CBody_Homonculus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Homonculus"),
		CWeapon_Homonculus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TargetLock */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TargetLock"),
		CTargetLock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	/* For.Prototype_GameObject_Item*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion ITEM

#pragma region DECAL
	/* For.Prototype_GameObject_Decal*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion DECAL



	/* For.Prototype_GameObject_HoverBoard */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HoverBoard"),
		CHoverboard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Trap */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trap"),
		CTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel_For_Shader()
{

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));
	
	return S_OK;
}

HRESULT CLoader::Loading_For_AckbarLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;


#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;





#pragma region  STAGE 1 PASSIVE ELEMENTS

	//STAGE 1 PASSIVE ELEMENTS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MergedSmallHouses23"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/MergedSmallHouses23/MergedSmallHouses23.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_MergedSmallHouses65"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/MergedSmallHouses65/MergedSmallHouses65.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarCastle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarCastle/AckbarCastle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AqueducTower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTower/AqueducTower.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigStairSides"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigStairSides/BigStairSides.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodStair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodStair.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodPlatform"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WoodStairs/WoodPlatform.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WoodPlank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/WoodenPlank.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigRocks"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BigRocks4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/BigRocks4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock1.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock2.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock3.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Rock4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BigRocks/Rock4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RuinsPilar"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/RuinsPilar/RuinsPilar.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseLarge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarHouseLarge.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarHouseSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarSwampHouse"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouse/AckbarSwampHouse.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofL"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofL.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofM"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofM.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarHouseRoofS"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AckbarHouseRoof/AckbarHouseRoofS.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AqueducTowerRoof"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/AqueducTowerRoof/AqueducTowerRoof.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_UnderRoofFilling"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/UnderRoofFilling/UnderRoofFilling.fbx", PreTransformMatrix))))
		return E_FAIL;



#pragma endregion  STAGE 1 PASSIVE ELEMENTS


#pragma region  DECO ELEMENTS
	//STAGE 1 DECO ELEMENTS
	//Box, Crate
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BoxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BoxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateB.fbx", PreTransformMatrix))))
		return E_FAIL;

	//YantraStatue
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_YantraStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/YantraStatue/YantraStatue.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Banners
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_BannerPole5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole5.fbx", PreTransformMatrix))))
		return E_FAIL;

	//DECO PROPS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_AckbarWell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/AckbarWell.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_CrenauxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_CrenauxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_HourdA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_HourdA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_WallMotifA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallMotifA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Remparts_WallRenfortA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallRenfortA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Charette
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CharetteNew"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteNew.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CharetteBroke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteBroke.fbx", PreTransformMatrix))))
		return E_FAIL;

	//FACADE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Facade10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade10.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion   DECO ELEMENTS

#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL

	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("LEVEL ACKBAR"));


	return S_OK;
}


HRESULT CLoader::Loading_For_AckbarLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ACKBAR, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

HRESULT CLoader::Loading_For_JugglasLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	/* Prototype_Component_Texture_Distortion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Distortion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Distortion/Distortion%d.png"), 5))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Texture_Desolve16"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effects/Desolve/Noise%d.png"), 16))))
		return E_FAIL;


#pragma region  Environmental Element Model Load

	lstrcpy(m_szLoadingText, TEXT("환경 Element 로딩 중"));
	/* For.Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
	//	return E_FAIL;

		/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ShatteredSkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;


	//// Prototype_Component_Model_TronesT02
	PreTransformMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TronesT02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT02/TronesT02.fbx", PreTransformMatrix))))
		return E_FAIL;

	//// Prototype_Component_Model_TronesT03
	PreTransformMatrix =/* XMMatrixScaling(0.01f, 0.01f, 0.01f) * */XMMatrixRotationY(XMConvertToRadians(0.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TronesT03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TronesT03/TronesT03.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//BASIC MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicCube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Cube/BasicCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicDonut"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Donut/BasicDonut.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BasicGround"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Basic/Ground/BasicGround.fbx", PreTransformMatrix))))
		return E_FAIL;


	//JUGGLAS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMap.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialMapBridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialMapBridge.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialDecoStructure"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoStructure.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_TutorialDecoCubes"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/TutorialMap/TutorialDecoCubes.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_MetalGrid"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/MetalGrid/MetalGrid.fbx", PreTransformMatrix))))
		return E_FAIL;





#pragma region  JUGGLAS PASSIVE ELEMENTS

	//JUGGLAS PASSIVE ELEMENTS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle.fbx", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RasSamrahCastle3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahCastle3.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion  JUGGLAS PASSIVE ELEMENTS

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Well"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/Well/Well.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WellArea"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/WellArea/WellArea.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region  DECO ELEMENTS
	//STAGE 1 DECO ELEMENTS
	//Box, Crate
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BoxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BoxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Crates_CrateB.fbx", PreTransformMatrix))))
		return E_FAIL;

	//YantraStatue
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_YantraStatue"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/YantraStatue/YantraStatue.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Banners
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_BannerPole5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/BannerPole/BannerPole5.fbx", PreTransformMatrix))))
		return E_FAIL;

	//DECO PROPS
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_AckbarWell"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/AckbarWell.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_CrenauxA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_CrenauxB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_CrenauxB.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_HourdA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_HourdA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_WallMotifA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallMotifA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Remparts_WallRenfortA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/Remparts_WallRenfortA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Stage_1/DecoObjects/RichStairs_Rambarde.fbx", PreTransformMatrix))))
		return E_FAIL;

	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	//LIGHT PROPS@@@@@@@@@@
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Brasero"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Brasero.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_BraseroSmall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_BraseroSmall.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Candle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Candle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_CandleGroup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_CandleGroup.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_Crystal.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_TorchA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchA.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Light_TorchB"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/LightProps/Light_TorchB.fbx", PreTransformMatrix))))
		return E_FAIL;
	//Charette
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CharetteNew"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteNew.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CharetteBroke"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Charette/CharetteBroke.fbx", PreTransformMatrix))))
		return E_FAIL;

	//FACADE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade1.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade2.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade3.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade4"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade4.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade5"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade5.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade6"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade6.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade7"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade7.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade8"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade8.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade9"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade9.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Facade10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Facade/Facade10.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion   DECO ELEMENTS

#pragma region  Active Element Model Load
	lstrcpy(m_szLoadingText, TEXT("Active Element 모델 로딩 중"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Elevator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Elevator/Elevator.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RotateGate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RasSamrahCastle/RasSamrahGate/RasSamrahGate.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion   Active Element Model Load




#pragma endregion

	/* For.Prototype_Component_Model_ForkLift */
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Wander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_HoverBoard*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Hoverboard"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Hoverboard/Hoverboard.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wander */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Catharsis"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Catharsis/Catharsis.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Cendres"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Cendres/Cendres.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CorruptedSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/CorruptedSword/CorruptedSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_DurgaSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DurgaSword/DurgaSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_IceBlade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/IceBlade/IceBlade.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Lughan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Lughan/Lughan.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_NaruehSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/NaruehSword/NaruehSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_PretorianSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/PretorianSword/PretorianSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_RadamantheSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/RadamantheSword/RadamantheSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_SitraSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SitraSword/SitraSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_ValnirSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/ValnirSword/ValnirSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_VeilleurSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/VeilleurSword/VeilleurSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WhisperSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapons/WhisperSword/WhisperSword_Anim.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma region Monster

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Juggulus
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Juggulus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Juggulus.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHammer
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Hammer.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandOne
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandOne"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_1.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandTwo
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandTwo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_2.fbx", PreTransformMatrix))))
		return E_FAIL;

	// JuggulusHandThree
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_JuggulusHandThree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Juggulus/Hand_3.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Aspiration
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Aspiration"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Aspiration.fbx", PreTransformMatrix))))
		return E_FAIL;

	// Sphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	// CircleSphere
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_CircleSphere"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Juggulus/Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire_Gun */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Legionnaire_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow_Jobmob.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Gun_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Gun.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Sword_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_LGGun */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Arrow_LGGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Arrow_Jobmob/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Ghost */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Ghost"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Ghost/Ghost.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Homonculus */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Homonculus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Homonculus/Homonculus.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* Mantari - 박은호 작업 */
	/* For.Prototype_Component_Model_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Mantari/Mantari.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Weapon_Mantari */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Weapon_Mantari"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Mantari/MantariSword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Legionnaire */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Legionnaire"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Legionnaire/Legionnaire.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Mst_TargetLock */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Mst_TargetLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Bone_Sphere/Bone_Sphere.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion Monster

#pragma region ITEM
	//Item
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Item"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/Item.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion ITEM
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Model_Decal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Decal/Decal.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma region DECAL


#pragma endregion DECAL

	//lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Navigation"),
	//	CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("피직스(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx"),
		CPhysXComponent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx_Charater"),
		CPhysXComponent_Character::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Physx_Vehicle"),
		CPhysXComponent_Vehicle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("LEVEL JUGGLAS"));

	return S_OK;
}

HRESULT CLoader::Loading_For_JugglasLevel_For_Shader()
{
	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_MapElement.hlsl"), VTXINSTANCE_MESH::Elements, VTXINSTANCE_MESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPassiveElement */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_VtxMapElement"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMapElement.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Calculate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_ComputeShader_Calculate"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Calculate.hlsl"), "main"))))
		return E_FAIL;

	/* For.Prototype_Component_ComputeShader_Float4 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_JUGGLAS, TEXT("Prototype_Component_ComputeShader_Float4"),
		CComputeShader_Buffer::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/ComputeShader_Float4.hlsl"), "main"))))
		return E_FAIL;


	//lstrcpy(m_szLoadingText, TEXT("쉐이더 로드 되었습니다."));

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	for (int i = 0; i < MAX_THREAD; ++i)
	{
		if (m_hThread[i] != nullptr)
		{
			WaitForSingleObject(m_hThread[i], INFINITE);
			CloseHandle(m_hThread[i]);
			m_hThread[i] = nullptr;
		}
	}

	for (_uint i = 0; i < MAX_THREAD; ++i)
		DeleteCriticalSection(&m_Critical_Section[i]);


	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
