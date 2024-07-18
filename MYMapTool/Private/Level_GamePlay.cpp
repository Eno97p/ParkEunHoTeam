#include "Level_GamePlay.h"

#include "GameInstance.h"
#include "Default_Camera.h"
#include "ThirdPersonCamera.h"
#include "SideViewCamera.h"

#include "Imgui_Manager.h"
#include "ToolObj_Manager.h"
#include "Terrain.h"

#include "NaviMgr.h"

#include "Light.h"
#include "Light_Manager.h"

#include "VIBuffer_Instance_Point.h"

#include "Grass.h"
CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{pDevice, pContext}
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    if (FAILED(Ready_Light()))
        return E_FAIL;


    
    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if(FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
        return E_FAIL;

    if (FAILED(Ready_LandObjects()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
    //// Imgui의 Save 버튼이 눌린 경우 Save_Data( ) 함수 호출
    if (CImgui_Manager::GetInstance()->Get_IsSave())
    {
        Save_Data();
       // CNaviMgr::GetInstance()->Save_NaviData(); // Navi Save( ) 함수 호출
    }     



    if (CImgui_Manager::GetInstance()->Get_IsLoad())
        Load_Data();

    if (CImgui_Manager::GetInstance()->Get_IsPhysXSave())
    {
        Save_Data_PhysX();
    }

    if (CImgui_Manager::GetInstance()->Get_IsPhysXLoad())
    {
        Load_Data_PhysX();
    }

    if (m_pGameInstance->Key_Down(DIK_1))
    {
        m_pGameInstance->Set_MainCamera(1);
    }
    if (m_pGameInstance->Key_Down(DIK_2))
    {
        m_pGameInstance->Set_MainCamera(2);
    }

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Level : GamePlay"));
#endif
}

HRESULT CLevel_GamePlay::Ready_Light()
{
    LIGHT_DESC			LightDesc{};

    //LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    //LightDesc.vDirection = _float4{ -0.6f, -1.f, 0.8f, 0.f };
    //LightDesc.vDiffuse = _float4(0.95f, 0.97f, 0.97f, 1.f);
    ////LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    //LightDesc.vAmbient = _float4(0.93f * 0.8f, 0.98f * 0.8f, 0.97f * 0.8f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);


    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(20.f, 5.f, 20.f, 1.f);
    //LightDesc.fRange = 20.f;
    //LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    ////LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
    //LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    ////LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);

    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(100.f, 50.f, 100.f, 1.f);
    //LightDesc.fRange = 10.f;
    //LightDesc.vDiffuse = _float4(0.8f, 1.f, 0.8f, 1.f);
    //LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);


    //ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
    //LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    //LightDesc.vPosition = _float4(259.85f, 53.45f, 186.49f, 1.0f);
    //LightDesc.fRange = 20.f;
    //LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
    //LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    //LightDesc.vSpecular = LightDesc.vDiffuse;

    //m_pGameInstance->Add_Light(LightDesc);

    CImgui_Manager::GetInstance()->Load_Lights();

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const wstring& strLayerTag)
{
    if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain")))) // , &pDesc
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;



    //CGrass::GRASS_DESC desc{};
    //if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Grass"), &desc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
    CDefault_Camera::DEFAULTCAM_DESC pDesc = {};

    pDesc.fSensor = 0.1f;

    pDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    pDesc.fFovy = XMConvertToRadians(60.f);
    pDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pDesc.fNear = 0.1f;
    pDesc.fFar = 3000.f;

    pDesc.fSpeedPerSec = 40.f;
    pDesc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_DefaultCamera"), &pDesc)))
        return E_FAIL;

    //pDesc.fSensor = 0.1f;

    //pDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    //pDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    //pDesc.fFovy = XMConvertToRadians(60.f);
    //pDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    //pDesc.fNear = 0.1f;
    //pDesc.fFar = 3000.f;

    //pDesc.fSpeedPerSec = 40.f;
    //pDesc.fRotationPerSec = XMConvertToRadians(90.f);

    //if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_DefaultCamera"), &pDesc)))
    //    return E_FAIL;


    CThirdPersonCamera::THIRDPERSONCAMERA_DESC pTPCDesc = {};

    pTPCDesc.fSensor = 0.1f;

    pTPCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pTPCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    pTPCDesc.fFovy = XMConvertToRadians(60.f);
    pTPCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pTPCDesc.fNear = 0.1f;
    pTPCDesc.fFar = 3000.f;

    pTPCDesc.fSpeedPerSec = 40.f;
    pTPCDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pTPCDesc.pPlayerTrans = dynamic_cast<CTransform*>( m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
    if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ThirdPersonCamera"), &pTPCDesc)))
        return E_FAIL;




    CSideViewCamera::SIDEVIEWCAMERA_DESC pSVDesc = {};

    pSVDesc.fSensor = 0.1f;

    pSVDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
    pSVDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    pSVDesc.fFovy = XMConvertToRadians(60.f);
    pSVDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
    pSVDesc.fNear = 0.1f;
    pSVDesc.fFar = 3000.f;

    pSVDesc.fSpeedPerSec = 40.f;
    pSVDesc.fRotationPerSec = XMConvertToRadians(90.f);
    pSVDesc.pPlayerTrans = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"), 0));
    if (FAILED(m_pGameInstance->Add_Camera(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_SideViewCamera"), &pSVDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
 /*   if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
        return E_FAIL;*/




    //if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(0, 0, XMVectorSet(0.f,0.f,0.f, 1.f))))
    //    return E_FAIL;



    //if (FAILED(CToolObj_Manager::GetInstance()->Add_CloneObj(0, 0, XMVectorSet(5.f, 3.f, 5.f, 1.f))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag, CLandObject::LANDOBJ_DESC* pLandObjDesc)
{
    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), pLandObjDesc)))
        return E_FAIL;


    return S_OK;
}


HRESULT CLevel_GamePlay::Ready_LandObjects()
{
    CLandObject::LANDOBJ_DESC		LandObjDesc{};

    LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(CGameInstance::GetInstance()->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
    LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(CGameInstance::GetInstance()->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), &LandObjDesc)))
        return E_FAIL;

    //if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), &LandObjDesc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Save_Data()
{
    const wchar_t* wszFileName[MAX_PATH] = { Setting_FileName() };
    HANDLE	hFile = CreateFile(*wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    _char   szName[MAX_PATH] = "";
    _char   szLayer[MAX_PATH] = "";
    _char   szModelName[MAX_PATH] = "";
    _float4x4 WorldMatrix;
    CModel::MODELTYPE eModelType;
    //_uint   iVerticesX = { 0 };
    //_uint   iVerticesZ = { 0 };
    DWORD   dwByte(0);
    _uint iTriggerType;

    // Terrain 정보 저장
    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object_InLayer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"))->back());
    // 현재 object list의 size가 0으로 나옴

    //iVerticesX = pTerrain->Get_VerticesX();
    //iVerticesZ = pTerrain->Get_VerticesZ();

    //WriteFile(hFile, &iVerticesX, sizeof(_uint), &dwByte, nullptr);
    //WriteFile(hFile, &iVerticesZ, sizeof(_uint), &dwByte, nullptr);

    // 생성된 Tool Obj들 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        strcpy_s(szName, iter->Get_Name());
        XMStoreFloat4x4(&WorldMatrix, iter->Get_WorldMatrix());
       

        WriteFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr); // sizeof(_char) * MAX_PATH

        if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
        {
            iTriggerType =iter->Get_TriggerType();
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);
        }
        else
        {
            strcpy_s(szModelName, iter->Get_ModelName());
            eModelType = iter->Get_ModelType();
            strcpy_s(szLayer, iter->Get_Layer());

            WriteFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            WriteFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);
        }
      
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontSave();

    MSG_BOX("Data Save");

    return S_OK;
}


HRESULT CLevel_GamePlay::Save_Data_PhysX()
{
    const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_Ackbar_PhysX.bin";
    HANDLE hFile = CreateFile(wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (nullptr == hFile)
        return E_FAIL;

    DWORD dwByte(0);
    map<std::string, bool> processedModels;

    // 먼저 모든 고유한 모델을 처리하여 map의 크기를 결정
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (nullptr != iter->Get_ModelName() && strcmp(iter->Get_Layer(), "Layer_Monster") != 0)
        {
            string modelName = iter->Get_ModelName();
            if (processedModels.find(modelName) == processedModels.end())
            {
                processedModels[modelName] = true;
            }
        }
    }

    // map의 크기(고유한 모델의 수)를 파일에 쓰기
    size_t mapSize = processedModels.size();
    WriteFile(hFile, &mapSize, sizeof(size_t), &dwByte, nullptr);

    // map을 초기화하고 다시 채우면서 모델 정보 저장
    processedModels.clear();

    // 생성된 Tool Obj들 저장
    for (auto& iter : CToolObj_Manager::GetInstance()->Get_ToolObjs())
    {
        if (nullptr != iter->Get_ModelName() && strcmp(iter->Get_Layer(), "Layer_Monster") != 0)
        {
            string modelName = iter->Get_ModelName();
            string modelPath = iter->Get_ModelPath();
            if (processedModels.find(modelName) == processedModels.end())
            {
                processedModels[modelName] = true;
                // "Model_"을 "PhysX_"로 대체하는 부분은 그대로 유지
                size_t pos = modelName.find("Model_");
                if (pos != string::npos)
                {
                    modelName.replace(pos, 6, "PhysX_");
                }
                char szModelName[MAX_PATH] = "";
                strcpy_s(szModelName, modelName.c_str());
                WriteFile(hFile, szModelName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
                WriteFile(hFile, modelPath.c_str(), sizeof(char) * MAX_PATH, &dwByte, nullptr);
            }
        }
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontPhysXSave();
    MSG_BOX("Data Save");
    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data_PhysX()
{
    const wchar_t* wszFileName = L"../Bin/MapData/PhysXData/Stage_PhysX.bin";
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

        // 여기서 읽어들인 모델 이름과 경로를 사용하여 필요한 작업을 수행합니다.
        // 예: PhysX 콜라이더 생성, 모델 로드 등

        // 예시:
        // Create_PhysXCollider(szModelName, szModelPath);

        // 디버그 출력 (필요시 사용)
        // OutputDebugStringA(("Loaded Model: " + std::string(szModelName) + "\n").c_str());
    }

    CloseHandle(hFile);
    CImgui_Manager::GetInstance()->Set_DontPhysXLoad();

    MSG_BOX("PhysX Data Loaded");
    return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data()
{
    HANDLE	hFile = CreateFile(Setting_FileName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    _char   szName[MAX_PATH] = "";
    _char   szLayer[MAX_PATH] = "";
    _char   szModelName[MAX_PATH] = "";
    _float4x4 WorldMatrix;
    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
    CModel::MODELTYPE eModelType = CModel::TYPE_END;
    _uint   iVerticesX = { 0 };
    _uint   iVerticesZ = { 0 };
    DWORD   dwByte(0);

    _uint   iTriggerType;
    //ReadFile(hFile, &iVerticesX, sizeof(_uint), &dwByte, nullptr);
    //ReadFile(hFile, &iVerticesZ, sizeof(_uint), &dwByte, nullptr);

    //CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_Object_InLayer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"))->back());

    // pDesc 넣어줘야함
    CTerrain::TERRAIN_DESC pDesc{};
    pDesc.iVerticesX = iVerticesX;
    pDesc.iVerticesZ = iVerticesZ;

    //pTerrain->Setting_NewTerrain(&pDesc);
    //pTerrain->Setting_LoadTerrain(&pDesc);
    
    while (true)
    {
        ReadFile(hFile, szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);

        if (strcmp(szName, "Prototype_GameObject_EventTrigger") == 0)
        {
            ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &iTriggerType, sizeof(_uint), &dwByte, nullptr);


            if (0 == dwByte)
                break;

            CToolObj::TOOLOBJ_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
            pDesc.mWorldMatrix = WorldMatrix;
            pDesc.TriggerType = iTriggerType;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
                return E_FAIL;
        }
        else 
        {
            ReadFile(hFile, szLayer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            ReadFile(hFile, szModelName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
            ReadFile(hFile, &eModelType, sizeof(CModel::MODELTYPE), &dwByte, nullptr);

            if (0 == dwByte)
                break;

            CToolObj::TOOLOBJ_DESC pDesc{};

            strcpy_s(pDesc.szObjName, szName);
            strcpy_s(pDesc.szLayer, szLayer);
            strcpy_s(pDesc.szModelName, szModelName);
            pDesc.mWorldMatrix = WorldMatrix;
            pDesc.eModelType = eModelType;

            if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_ToolObj"), &pDesc)))
                return E_FAIL;
        }
      
    
    }

    CloseHandle(hFile);

    CImgui_Manager::GetInstance()->Set_DontLoad();
    MSG_BOX("Data Load");

    return S_OK;
}



const _tchar* CLevel_GamePlay::Setting_FileName()
{
    _int iStageIdx = CImgui_Manager::GetInstance()->Get_StageIdx();

    switch (iStageIdx)
    {
    case STAGE_HOME:
        return  L"../Bin/MapData/Stage.dat";
      //  return  L"../../Data/Home.dat";
    case STAGE_ONE:
        return L"../Bin/MapData/Stage_Tutorial.dat";
    case STAGE_TWO:
        return L"../Bin/MapData/Stage_AndrasArena.dat";
    case STAGE_THREE:
        return L"../Bin/MapData/Stage_Jugglas.dat";
    case STAGE_BOSS:
        return L"../../Data/BossStage.dat";
    default:
        MSG_BOX("Setting File Name is Failed");
        return L"\0";
    }
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CLevel_GamePlay");
        return nullptr;
    }

    return pInstance;
}

void CLevel_GamePlay::Free()
{
    CToolObj_Manager::DestroyInstance();
    __super::Free();
}
