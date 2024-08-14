#include "stdafx.h"
#include "..\Public\Level_GrassLand.h"
#include "GameInstance.h"
#include "UI_Manager.h"

//Cameras
#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "SideViewCamera.h"
#include "TransitionCamera.h"

#include "Map_Element.h"
#include "Monster.h"

#include "Light.h"
#include "UI_FadeInOut.h"
#include "FireEffect.h"

#include "Trap.h"
#include "Item.h"


#include "CHoverBoard.h"

#include "Tree.h"
#include "Grass.h"
#include "BackGround_Card.h"
#include"CInitLoader.h"
CLevel_GrassLand::CLevel_GrassLand(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pUI_Manager(CUI_Manager::GetInstance())

{
	Safe_AddRef(m_pUI_Manager);
}

HRESULT CLevel_GrassLand::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	CRenderer::FOG_DESC fogDesc{};
	fogDesc.vFogColor = { 196.f / 255.f, 233.f / 255.f, 255.f / 255.f, 1.f };
	fogDesc.vFogColor2 = { 94.f / 255.f, 160.f / 255.f, 255.f / 255.f, 1.f };
	fogDesc.fFogRange = 7788.5;
	fogDesc.fFogHeightFalloff = 0.0f;
	fogDesc.fFogGlobalDensity = 1.0f;
	fogDesc.fFogTimeOffset = 1.154f;
	fogDesc.fFogTimeOffset2 = 3.462f;
	fogDesc.fNoiseIntensity = 1.731f;
	fogDesc.fNoiseIntensity2 = 1.923f;
	fogDesc.fNoiseSize = 0.000481f;
	fogDesc.fNoiseSize2 = 0.000481f;
	fogDesc.fFogBlendFactor = 0.284f;
	m_pGameInstance->Set_FogOption(fogDesc);

	m_pGameInstance->Set_ReflectionWave(0.693f, 0.1f, 13.743f, 7.5f, 0);

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;


	if (FAILED(Ready_LandObjects()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	Load_LevelData(TEXT("../Bin/MapData/Stage_GrassLand.bin"));

	//Load_Data_Effects();

	m_pUI_Manager->Render_UIGroup(true, "HUD_State");
	m_pUI_Manager->Render_UIGroup(true, "HUD_WeaponSlot");


	m_iCamSize =  m_pGameInstance->Get_GameObjects_Ref(/*m_pGameInstance->Get_CurrentLevel()*/LEVEL_GRASSLAND, TEXT("Layer_Camera")).size();

	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

	pDesc.isFadeIn = true;
	pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
		return E_FAIL;





	////비동기 저장
	//auto futures = m_pGameInstance->AddWork([this]() {
	//	//초기값 몬스터 저장
	//	vector<_tagMonsterInit_Property> vecMonsterInitProperty;
	//	list<CGameObject*> pList = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GRASSLAND, L"Layer_Monster");
	//	vecMonsterInitProperty.resize(pList.size());
	//	_uint iIndex = 0;
	//	for (auto& pMonster : pList)
	//	{
	//		vecMonsterInitProperty[iIndex].vPos = dynamic_cast<CMonster*>(pMonster)->Get_InitPos();
	//		//wcscpy_s(vecMonsterInitProperty[iIndex].strName, MAX_PATH, pMonster->Get_ProtoTypeTag().c_str());
	//		iIndex++;

	//	}
	//	wstring wstrlevelName = Get_CurLevelName(m_pGameInstance->Get_CurrentLevel());
	//	wstring wstrFilePath = L"../Bin/DataFiles/LevelInit_" + wstrlevelName + L".dat";
	//	//const char* Test = Client::LevelNames[m_pGameInstance->Get_CurrentLevel()];
	//	Engine::Save_Data(wstrFilePath.c_str(), false, vecMonsterInitProperty.size(), vecMonsterInitProperty.data());
	//});



	CInitLoader<LEVEL, wstring>* initLoader = new CInitLoader<LEVEL, wstring>(&initLoader);
	initLoader->Save_Start(LEVEL_GRASSLAND, L"Layer_Monster");

	return S_OK;
}

void CLevel_GrassLand::Tick(_float fTimeDelta)
{
	m_pUI_Manager->Tick(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_O))
	{
		Add_FadeInOut(false);
	}
	else if (m_pGameInstance->Key_Down(DIK_K))
	{
		Add_FadeInOut(true);
	}

//#ifdef _DEBUG
	//카메라 전환 ~ 키
	//카메라 전환 ~ 키
	//카메라 전환 ~ 키
	if (m_pGameInstance->Key_Down(DIK_GRAVE))
	{
		m_iMainCameraIdx++;
		if (m_iCamSize <= m_iMainCameraIdx)
		{
			m_iMainCameraIdx = 0;
		}
		m_pGameInstance->Set_MainCamera(m_iMainCameraIdx);
	}

	//if (m_pGameInstance->Key_Down(DIK_4))
	//{
	//	CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

	//	//pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	//	//pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	//	pTCDesc.fFovy = XMConvertToRadians(60.f);
	//	pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	//	pTCDesc.fNear = 0.1f;
	//	pTCDesc.fFar = 3000.f;

	//	pTCDesc.fSpeedPerSec = 40.f;
	//	pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

	//	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GRASSLAND, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
	//	{
	//		MSG_BOX("FAILED");
	//	}
	//	return;
	//}

	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
//#endif
}

void CLevel_GrassLand::Late_Tick(_float fTimeDelta)
{
	m_pUI_Manager->Late_Tick(fTimeDelta);
}

HRESULT CLevel_GrassLand::Ready_Lights()
{
	m_pGameInstance->Light_Clear();

 	Load_Lights();

	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_Layer_Camera(const wstring & strLayerTag)
{
	m_pGameInstance->Clear_Cameras();
	CFreeCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;

	//CameraDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f); // _float4(71.1f, 542.f, 78.f, 1.f);
	//CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f); // _float4(71.1f, 522.f, 98.f, 1.f);

	CameraDesc.vEye = _float4(140.f, 542.f, 78.f, 1.f);
	CameraDesc.vAt = _float4(140.f, 522.f, 98.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 10000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);	

	if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
		return E_FAIL;

	 CThirdPersonCamera::THIRDPERSONCAMERA_DESC pTPCDesc = {};

	 pTPCDesc.fSensor = 0.1f;

	 pTPCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
	 pTPCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	 pTPCDesc.fFovy = XMConvertToRadians(60.f);
	 pTPCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	 pTPCDesc.fNear = 0.1f;
	 pTPCDesc.fFar = 10000.f;

	 pTPCDesc.fSpeedPerSec = 40.f;
	 pTPCDesc.fRotationPerSec = XMConvertToRadians(90.f);
	 pTPCDesc.pPlayerTrans = dynamic_cast<CTransform*>( m_pGameInstance->Get_Component(LEVEL_GRASSLAND, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
	 if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_ThirdPersonCamera"), &pTPCDesc)))
		 return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_Layer_Effect(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Particle_Rect"))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
	//	return E_FAIL;

	//for (size_t i = 0; i < 60; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Explosion"))))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Cloud"))))
		return E_FAIL;

	CMap_Element::MAP_ELEMENT_DESC desc{};
	desc.mWorldMatrix = {};

	_matrix vMat = {500.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 500.f, 0.f,
	-41.f, 271.f, -80.565f, 1.f };

	XMStoreFloat4x4(&desc.mWorldMatrix, vMat);
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Lagoon"), &desc)))
		return E_FAIL;


	CBackGround_Card::CARD_DESC CardDesc{};
	CardDesc.mWorldMatrix = {};

	vMat = { 1427.475f, 0.f, -2009.57, 0.f,
	0.f, 1000.f, 0.f, 0.f,
	815.468f, 0.f, 579.258f, 0.f,
	-3342.f, 695.f, -1722.f, 1.f };

	XMStoreFloat4x4(&CardDesc.mWorldMatrix, vMat);
	CardDesc.iTexNum = 0;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &CardDesc)))
		return E_FAIL;

	ZeroMemory(&CardDesc, sizeof(CardDesc));
	vMat = { -1977.f, 0.f, -3478.f, 0.f,
   0.f, 2000.f, 0.f, 0.f,
   1739.f, 0.f, -988.f, 0.f,
  -3304.f, 1157.f, 2678.f, 1.f };
	XMStoreFloat4x4(&CardDesc.mWorldMatrix, vMat);
	CardDesc.iTexNum = 1;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &CardDesc)))
		return E_FAIL;

	CardDesc.mWorldMatrix = {};
	vMat = { 1143.863f, 0.f, 2773.897f, 0.f,
	   0.f, 1500.f, 0.f, 0.f,
	   -1386.976f, 0.f, 571.943f, 0.f,
	   3609.f, 783.378f, -706.441f, 1.f };
	XMStoreFloat4x4(&CardDesc.mWorldMatrix, vMat);
	CardDesc.iTexNum = 2;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &CardDesc)))
		return E_FAIL;

	ZeroMemory(&CardDesc, sizeof(CardDesc));
	vMat = { 3495.291f, 0.f, -188.991f, 0.f,
		0.f, 2000.f, 0.f, 0.f,
		161.997f, 0.f, 2996.f, 0.f,
		1349.754f, 1881.764f, 4170.497f, 1.f };
	XMStoreFloat4x4(&CardDesc.mWorldMatrix, vMat);
	CardDesc.iTexNum = 0;
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_BackGround_Card"), &CardDesc)))
		return E_FAIL;
	//CMap_Element::MAP_ELEMENT_DESC desc{};
	//XMStoreFloat4x4(&desc.mWorldMatrix,  XMMatrixIdentity());
	//desc.mWorldMatrix.m[3][1] = 100.f;
	//desc.mWorldMatrix.m[0][0] = 300.f;
	//desc.mWorldMatrix.m[1][1] = 300.f;
	//desc.mWorldMatrix.m[2][2] = 300.f;


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Lagoon"), &desc)))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_LandObjects()
{
	CLandObject::LANDOBJ_DESC		LandObjDesc{};
	
	LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GRASSLAND, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_GRASSLAND, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjDesc)))
		return E_FAIL;



	//_float3 fPosArray[] = {
	//_float3(-73.8f, 6.7f, -7.2f),
	//_float3(-107.2f, 3.3f, -15.9f),
	//_float3(-201.5f, 3.5f, -15.4f),
	//};

	//_uint arraySize = sizeof(fPosArray) / sizeof(_float3);

	//CItem::ITEM_DESC desc;
	//for (int i = 0; i < arraySize; i++)
	//{
	//	desc.vPosition = fPosArray[i];
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &desc)))
	//		return E_FAIL;
	//}



	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_Layer_Player(const wstring & strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
	/*pLandObjDesc->mWorldMatrix._41 = -8.3f;
	pLandObjDesc->mWorldMatrix._42 = 3.5f;
	pLandObjDesc->mWorldMatrix._43 = -2.4f;
	pLandObjDesc->mWorldMatrix._44 = 1.f;*/

	pLandObjDesc->mWorldMatrix._41 = 500.f;
	pLandObjDesc->mWorldMatrix._42 = 346.995f;
	pLandObjDesc->mWorldMatrix._43 = 500.f;
	pLandObjDesc->mWorldMatrix._44 = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Player"), pLandObjDesc)))
		return E_FAIL;


	////Blast Wall Test Code
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_BlastWall"), TEXT("Prototype_GameObject_BlastWall"))))
	//	return E_FAIL;
	


	return S_OK;
}

HRESULT CLevel_GrassLand::Ready_Layer_Monster(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
	//for (size_t i = 0; i < 10; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Monster"), pLandObjDesc)))
	//		return E_FAIL;
	//}0

	// 테스트 위해 임의로 생성
	/*CMonster::MST_DESC* pDesc = static_cast<CMonster::MST_DESC*>(pLandObjDesc);

	pDesc->eLevel = LEVEL_GRASSLAND;*/

	// Prototype_GameObject_Boss_Juggulus   Prototype_GameObject_Ghost    Prototype_GameObject_Legionnaire_Gun

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Legionnaire_Gun"), pLandObjDesc)))
	//	return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Ghost"), pLandObjDesc)))
	//	return E_FAIL;
	//

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Homonculus"), pLandObjDesc)))
	//	return E_FAIL;



	////for (size_t i = 0; i < 5; i++)
	//{

		//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, strLayerTag, TEXT("Prototype_GameObject_Mantari"), pLandObjDesc)))
		//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GrassLand::Load_LevelData(const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
		return E_FAIL;

	char szName[MAX_PATH] = "";
	char szLayer[MAX_PATH] = "";
	char szModelName[MAX_PATH] = "";
	_float4x4 WorldMatrix;
	CModel::MODELTYPE eModelType = CModel::TYPE_END;
	DWORD dwByte(0);
	_tchar wszName[MAX_PATH] = TEXT("");
	_tchar wszLayer[MAX_PATH] = TEXT("");
	_tchar wszModelName[MAX_PATH] = TEXT("");
	_uint   iTriggerType = 0;
	_double dStartTime = 0;

	// 모델 종류별로 월드 매트릭스를 저장할 맵
	map<wstring, vector<_float4x4*>> modelMatrices;


	// 생성된 객체 로드
	while (true)
	{
		ZeroMemory(wszName, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszLayer, sizeof(_tchar) * MAX_PATH);
		ZeroMemory(wszModelName, sizeof(_tchar) * MAX_PATH);

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
		{
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CMap_Element::MAP_ELEMENT_DESC pDesc{};

			pDesc.mWorldMatrix = WorldMatrix;
			pDesc.TriggerType = iTriggerType;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
				return E_FAIL;
		}
		else if (strcmp(szName, "Prototype_GameObject_Trap") == 0)
		{
			ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
			ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &dStartTime, sizeof(_double), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			CTrap::TRAP_DESC pTrapDesc{};

			MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

			pTrapDesc.wstrLayer = wszLayer;
			pTrapDesc.wstrModelName = wszModelName;
			pTrapDesc.mWorldMatrix = WorldMatrix;
			pTrapDesc.TriggerType = iTriggerType;
			pTrapDesc.dStartTimeOffset = dStartTime;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Trap"), TEXT("Prototype_GameObject_Trap"), &pTrapDesc)))
				return E_FAIL;
		}
		else
		{
			ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
			ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
			ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

			MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName), wszName, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szLayer, strlen(szLayer), wszLayer, MAX_PATH);
			MultiByteToWideChar(CP_ACP, 0, szModelName, strlen(szModelName), wszModelName, MAX_PATH);

			if (0 == dwByte)
				break;


			if (wstring(wszName) == TEXT("Prototype_GameObject_Passive_Element"))
			{
				// 모델 이름별로 월드 매트릭스 저장
				_float4x4* pWorldMatrix = new _float4x4(WorldMatrix);
				modelMatrices[wszModelName].push_back(pWorldMatrix);
			}
			else
			{
				//for (int i = 0; i < 40; i++)
				{
					// 다른 객체들은 개별적으로 생성
					CMap_Element::MAP_ELEMENT_DESC pDesc{};


					pDesc.mWorldMatrix = WorldMatrix;
					pDesc.wstrModelName = wszModelName;
					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, wszLayer, wszName, &pDesc)))
						return E_FAIL;
				}


			}
		}



	}

	CloseHandle(hFile);

	// Passive_Element 객체들을 인스턴싱하여 생성
	for (const auto& pair : modelMatrices)
	{
		CMap_Element::MAP_ELEMENT_DESC pDesc{};
		pDesc.WorldMats = pair.second;
		pDesc.iInstanceCount = pair.second.size();
		pDesc.wstrModelName = pair.first;

		pDesc.mWorldMatrix = WorldMatrix;
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Passive_Element"),
			TEXT("Prototype_GameObject_Passive_Element"), &pDesc)))
			return E_FAIL;
	}

	// 동적 할당된 메모리 해제
	for (auto& pair : modelMatrices)
	{
		for (auto pWorldMatrix : pair.second)
		{
			Safe_Delete(pWorldMatrix);
		}
	}

	return S_OK;
}

HRESULT CLevel_GrassLand::Load_Data_Effects()
{
	const wchar_t* wszFileName = L"../Bin/MapData/EffectsData/Stage_GrassLand_Effects.bin";
	HANDLE hFile = CreateFile(wszFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
		return E_FAIL;

	DWORD dwByte(0);
	_uint iEffectCount = 0;

	// 이펙트 개수 읽기
	ReadFile(hFile, &iEffectCount, sizeof(_uint), &dwByte, nullptr);

	// Tree 객체들을 그룹화하기 위한 맵
	auto treeCmp = [](const tuple<wstring, float, float, float, bool>& a, const tuple<wstring, float, float, float, bool>& b) {
		return a < b;
		};
	map<tuple<wstring, float, float, float, bool>, vector<_float4x4*>, decltype(treeCmp)> treeGroups(treeCmp);

	// Grass 객체들을 그룹화하기 위한 맵
	auto grassCmp = [](const tuple<wstring, wstring, _float3, _float3>& a, const tuple<wstring, wstring, _float3, _float3>& b) {
		if (get<0>(a) != get<0>(b)) return get<0>(a) < get<0>(b);
		if (get<1>(a) != get<1>(b)) return get<1>(a) < get<1>(b);

		const _float3& topColA = get<2>(a);
		const _float3& topColB = get<2>(b);
		if (topColA.x != topColB.x) return topColA.x < topColB.x;
		if (topColA.y != topColB.y) return topColA.y < topColB.y;
		if (topColA.z != topColB.z) return topColA.z < topColB.z;

		const _float3& botColA = get<3>(a);
		const _float3& botColB = get<3>(b);
		if (botColA.x != botColB.x) return botColA.x < botColB.x;
		if (botColA.y != botColB.y) return botColA.y < botColB.y;
		return botColA.z < botColB.z;
		};
	map<tuple<wstring, wstring, _float3, _float3>, vector<vector<_float4x4*>>, decltype(grassCmp)> grassGroups(grassCmp);

	for (_uint i = 0; i < iEffectCount; ++i)
	{
		char szName[MAX_PATH] = "";
		char szModelName[MAX_PATH] = "";
		char szLayer[MAX_PATH] = "";
		_float4x4 WorldMatrix;

		ReadFile(hFile, szName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, szLayer, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		wstring wsName, wsModelName, wsLayer;
		int nNameLen = MultiByteToWideChar(CP_ACP, 0, szName, -1, NULL, 0);
		int nModelNameLen = MultiByteToWideChar(CP_ACP, 0, szModelName, -1, NULL, 0);
		int nLayerLen = MultiByteToWideChar(CP_ACP, 0, szLayer, -1, NULL, 0);
		wsName.resize(nNameLen);
		wsModelName.resize(nModelNameLen);
		wsLayer.resize(nLayerLen);
		MultiByteToWideChar(CP_ACP, 0, szName, -1, &wsName[0], nNameLen);
		MultiByteToWideChar(CP_ACP, 0, szModelName, -1, &wsModelName[0], nModelNameLen);
		MultiByteToWideChar(CP_ACP, 0, szLayer, -1, &wsLayer[0], nLayerLen);

		if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Fire_Effect") == 0)
		{
			CFireEffect::FIREEFFECTDESC FireDesc{};
			FireDesc.vStartPos = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };
			FireDesc.vStartScale = { 1.f, 1.f };
			FireDesc.mWorldMatrix = WorldMatrix;

			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, wsLayer.c_str(), wsName.c_str(), &FireDesc)))
				return E_FAIL;
		}
		else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Grass") == 0)
		{
			_float3 TopCol, BotCol;
			ReadFile(hFile, &TopCol, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &BotCol, sizeof(_float3), &dwByte, nullptr);

			_uint iVtxMatrixCount = 0;
			ReadFile(hFile, &iVtxMatrixCount, sizeof(_uint), &dwByte, nullptr);

			vector<_float4x4*> vtxMatrices;
			for (_uint j = 0; j < iVtxMatrixCount; ++j)
			{
				_float4x4* pMatrix = new _float4x4();
				ReadFile(hFile, pMatrix, sizeof(_float4x4), &dwByte, nullptr);
				vtxMatrices.push_back(pMatrix);
			}

			// Grass 그룹화
			auto key = make_tuple(wsModelName, wsName, TopCol, BotCol);
			grassGroups[key].emplace_back(move(vtxMatrices));
		}
		else if (wcscmp(wsName.c_str(), L"Prototype_GameObject_Tree") == 0)
		{
			_float3 LeafCol;
			bool isBloom;
			ReadFile(hFile, &LeafCol, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &isBloom, sizeof(bool), &dwByte, nullptr);

			// 트리 그룹화
			auto key = make_tuple(wsModelName, LeafCol.x, LeafCol.y, LeafCol.z, isBloom);
			_float4x4* pWorldMatrix = new _float4x4(WorldMatrix);
			treeGroups[key].emplace_back(pWorldMatrix);
		}
	}

	CloseHandle(hFile);

	// Tree 객체들을 인스턴싱하여 생성
	for (const auto& group : treeGroups)
	{
		CTree::TREE_DESC TreeDesc{};
		TreeDesc.wstrModelName = get<0>(group.first);
		TreeDesc.vLeafCol = _float3(get<1>(group.first), get<2>(group.first), get<3>(group.first));
		TreeDesc.isBloom = get<4>(group.first);
		TreeDesc.WorldMats = group.second;
		TreeDesc.iInstanceCount = group.second.size();

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Vegetation"), TEXT("Prototype_GameObject_Tree"), &TreeDesc)))
			return E_FAIL;
	}

	// Grass 객체들을 인스턴싱하여 생성
	for (const auto& group : grassGroups)
	{
		CGrass::GRASS_DESC GrassDesc{};
		GrassDesc.wstrModelName = get<0>(group.first);
		GrassDesc.vTopCol = get<2>(group.first);
		GrassDesc.vBotCol = get<3>(group.first);

		// 모든 인스턴스의 정점 매트릭스를 하나의 벡터로 합칩니다.
		vector<_float4x4*> allVtxMatrices;
		for (const auto& instanceMatrices : group.second)
		{
			allVtxMatrices.insert(allVtxMatrices.end(), instanceMatrices.begin(), instanceMatrices.end());
		}

		GrassDesc.WorldMats = move(allVtxMatrices);
		GrassDesc.iInstanceCount = GrassDesc.WorldMats.size();

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_Vegetation"), TEXT("Prototype_GameObject_Grass"), &GrassDesc)))
			return E_FAIL;
	}

	// Tree 메모리 해제
	for (auto& group : treeGroups)
	{
		for (auto& pMatrix : group.second)
		{
			delete pMatrix;
		}
	}

	// Grass 메모리 해제
	for (auto& group : grassGroups)
	{
		for (auto& instanceMatrices : group.second)
		{
			for (auto& pMatrix : instanceMatrices)
			{
				delete pMatrix;
			}
		}
	}

#ifdef _DEBUG
	  MSG_BOX("Effect Data Load");
#endif

	return S_OK;
}

void CLevel_GrassLand::Load_Lights()
{
	list<CLight*> lights = m_pGameInstance->Get_Lights();
	for (auto& iter : lights)
	{
		iter->LightOff();
	}

	wstring LightsDataPath = L"../Bin/MapData/LightsData/GrassLand_Lights.dat";

	HANDLE hFile = CreateFile(LightsDataPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (nullptr == hFile)
	{
		MSG_BOX("hFile is nullptr");
		return;
	}

	DWORD dwByte(0);
	_uint iLightCount = 0;
	ReadFile(hFile, &iLightCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iLightCount; ++i)
	{
		LIGHT_DESC desc{};
		ReadFile(hFile, &desc.eType, sizeof(LIGHT_DESC::TYPE), &dwByte, nullptr);
		ReadFile(hFile, &desc.vDiffuse, sizeof(XMFLOAT4), &dwByte, nullptr);
		ReadFile(hFile, &desc.vAmbient, sizeof(XMFLOAT4), &dwByte, nullptr);
		ReadFile(hFile, &desc.vSpecular, sizeof(XMFLOAT4), &dwByte, nullptr);

		switch (desc.eType)
		{
		case LIGHT_DESC::TYPE_DIRECTIONAL:
			ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
			break;
		case LIGHT_DESC::TYPE_POINT:
			ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
			break;
		case LIGHT_DESC::TYPE_SPOTLIGHT:
			ReadFile(hFile, &desc.vDirection, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.vPosition, sizeof(XMFLOAT4), &dwByte, nullptr);
			ReadFile(hFile, &desc.fRange, sizeof(float), &dwByte, nullptr);
			ReadFile(hFile, &desc.innerAngle, sizeof(float), &dwByte, nullptr);
			ReadFile(hFile, &desc.outerAngle, sizeof(float), &dwByte, nullptr);
			break;
		}

		if (0 == dwByte)
			break;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
		{
			MSG_BOX("Failed to Add Light");
			return;
		}
	}

	CloseHandle(hFile);
#ifdef _DEBUG
	MSG_BOX("Lights Data Load");
#endif
	return;
}

HRESULT CLevel_GrassLand::Add_FadeInOut(_bool isDissolve)
{
	CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

	pDesc.isFadeIn = false;
	if(isDissolve)
		pDesc.eFadeType = CUI_FadeInOut::TYPE_DISSOLVE;
	else
		pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GRASSLAND, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_GrassLand * CLevel_GrassLand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GrassLand*		pInstance = new CLevel_GrassLand(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_GrassLand");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GrassLand::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Manager);
	

}
