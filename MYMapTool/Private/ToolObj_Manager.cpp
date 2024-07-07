#include "ToolObj_Manager.h"

#include "GameInstance.h"

#include "Monster.h"
#include "ToolObj.h"
#include "Imgui_Manager.h"

IMPLEMENT_SINGLETON(CToolObj_Manager)

CToolObj_Manager::CToolObj_Manager()
    : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);

    Initialize();
}

const _float4x4* CToolObj_Manager::Get_Obj_WorldMatrix(_int iIndex)
{
    if (m_ToolObjs.size() <= iIndex)
        return nullptr;
    
    return m_ToolObjs[iIndex]->Get_WorldFloat4x4();
}

wstring CharToWstring(const char* charArray)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, charArray, -1, NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, charArray, -1, &wstrTo[0], size_needed);
    return wstrTo;
}

HRESULT CToolObj_Manager::Add_CloneObj(_int iLayerIdx, _int iSelectIdx, _vector vPosition, _vector vColor)
{
    // 새로 생성하는 데이터

    CToolObj::TOOLOBJ_DESC pDesc = {};

    XMStoreFloat4(&pDesc.f4Color, vColor);

    Setting_Desc(iLayerIdx, iSelectIdx, pDesc);

    //strcpy_s(pDesc.szLayer, "Layer_Monster");
    //strcpy_s(pDesc.szModelName, CToolObj_Manager::GetInstance()->Setting_ObjName(iLayerIdx, iSelectIdx));
    //strcpy_s(pDesc.szObjName, "Prototype_ToolObj");
    //pDesc.eModelType = CModel::TYPE_ANIM;

    //Setting_Desc(iLayerIdx, iSelectIdx, pDesc);

    // vPosition
    _float4 vPositionStore;
    XMStoreFloat4(&vPositionStore, vPosition); // 저장용으로 바꿔줌
    _matrix WorldMatrix = XMLoadFloat4x4(&pDesc.mWorldMatrix); // 연산용으로 변경

    WorldMatrix = XMMatrixIdentity();

    WorldMatrix.r[3] = vPosition;
    XMStoreFloat4x4(&pDesc.mWorldMatrix, WorldMatrix);

    if (iLayerIdx == 2) // ActiveELement
    {
        wstring wst = L"";
        wst = CharToWstring(pDesc.szObjName);
        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Active_Element"), wst.c_str(), &pDesc)))
            return E_FAIL;
    }
    else if(iLayerIdx == 0) 
    {
        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &pDesc)))
            return E_FAIL;
    }
    else if (iLayerIdx == 1)
    {
        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_ToolObj"), &pDesc)))
            return E_FAIL;
    }
    else if (iLayerIdx == 3)
    {
        pDesc.TriggerType = /*(CEventTrigger::TRIGGER_TYPE)*/iSelectIdx;
        if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_EventTrigger"), &pDesc)))
            return E_FAIL;
    }


    return S_OK;
}

void CToolObj_Manager::Initialize()
{
    m_Monsters = {   TEXT("Prototype_Component_Model_Fiona"), 
                        TEXT("Prototype_Component_Model_Lantern"),
                        TEXT("Prototype_Component_Model_Octopus"),
                        TEXT("Prototype_Component_Model_Crab"),
                        TEXT("Prototype_Component_Model_EliteMst"),
                        TEXT("Prototype_Component_Model_Boss_1P") };

    m_PassiveElements = { TEXT("Prototype_Component_Model_BasicCube"),  TEXT("Prototype_Component_Model_BasicDonut"), TEXT("Prototype_Component_Model_BasicGround"),
                        TEXT("Prototype_Component_Model_TronesT03"), TEXT("Prototype_Component_Model_AndrasTEST"), TEXT("Prototype_Component_Model_AndrasArenaRocks"), TEXT("Prototype_Component_Model_Grasses_TT"), TEXT("Prototype_Component_Model_RasSamrahCastle"),
                        TEXT("Prototype_Component_Model_RasSamrahCastle2"),
        //TUTORIAL
        TEXT("Prototype_Component_Model_TutorialMap"), TEXT("Prototype_Component_Model_TutorialDecoStructure"), TEXT("Prototype_Component_Model_TutorialDecoCubes"), TEXT("Prototype_Component_Model_TutorialDecoMaze"),

        //Stage 1
        TEXT("Prototype_Component_Model_WellArea"), TEXT("Prototype_Component_Model_MergedSmallHouses23"),
                        TEXT("Prototype_Component_Model_MergedSmallHouses65"), TEXT("Prototype_Component_Model_AckbarCastle"),
                        TEXT("Prototype_Component_Model_AqueducTower"), TEXT("Prototype_Component_Model_BigStairSides"),
                        TEXT("Prototype_Component_Model_WoodStair"), TEXT("Prototype_Component_Model_WoodPlatform"),
                        TEXT("Prototype_Component_Model_WoodPlank"),
                        TEXT("Prototype_Component_Model_RichStairs_Rambarde"),
                        TEXT("Prototype_Component_Model_BigRocks"),
                        TEXT("Prototype_Component_Model_BigRocks4"),

        //DECO ELEMENTS
                        TEXT("Prototype_Component_Model_BoxA"), TEXT("Prototype_Component_Model_BoxB"),
                        TEXT("Prototype_Component_Model_YantraStatue"),
                        TEXT("Prototype_Component_Model_Light_Brasero"), TEXT("Prototype_Component_Model_Light_BraseroSmall"),
                        TEXT("Prototype_Component_Model_Light_Candle"),  TEXT("Prototype_Component_Model_Light_CandleGroup"),
                        TEXT("Prototype_Component_Model_Light_Crystal"),
                        TEXT("Prototype_Component_Model_Light_TorchA"), TEXT("Prototype_Component_Model_Light_TorchB"),
                        TEXT("Prototype_Component_Model_CharetteNew"), TEXT("Prototype_Component_Model_CharetteBroke"),
                        TEXT("Prototype_Component_Model_Facade1"), TEXT("Prototype_Component_Model_Facade2"),
                        TEXT("Prototype_Component_Model_Facade3"), TEXT("Prototype_Component_Model_Facade4"),
                        TEXT("Prototype_Component_Model_Facade5"), TEXT("Prototype_Component_Model_Facade6"),
                        TEXT("Prototype_Component_Model_Facade7"), TEXT("Prototype_Component_Model_Facade8"),
                        TEXT("Prototype_Component_Model_Facade9"), TEXT("Prototype_Component_Model_Facade10"),

                        TEXT("Prototype_Component_Model_RuinsPilar"), TEXT("Prototype_Component_Model_AckbarHouseLarge"),
                        TEXT("Prototype_Component_Model_AckbarHouseSmall"), TEXT("Prototype_Component_Model_AckbarSwampHouse"),
                        TEXT("Prototype_Component_Model_AckbarHouseRoofL"), TEXT("Prototype_Component_Model_AckbarHouseRoofM"),
                        TEXT("Prototype_Component_Model_AckbarHouseRoofS"),
                        TEXT("Prototype_Component_Model_AqueducTowerRoof"),
                        TEXT("Prototype_Component_Model_UnderRoofFilling")
    };

    m_ActiveElements = { TEXT("Prototype_Component_Model_Grass"),
        
        TEXT("Prototype_Component_Model_TutorialMapBridge"),
        TEXT("Prototype_Component_Model_Well"),
        //FakeWall
        TEXT("Prototype_Component_Model_BasicDonut"),
        TEXT("Prototype_Component_Model_BasicCube"),
        //EventTrigger
        TEXT("Prototype_Component_Model_BasicCube"),
        TEXT("Prototype_Component_Model_SkySphere"),
        
        TEXT("Prototype_Component_Model_Item_Hell"),
        TEXT("Prototype_Component_Model_Boss_Gun"), TEXT("Prototype_Component_Model_Cannon") };
}

void CToolObj_Manager::Setting_Desc(_int iLayerIdx, _int iSelectIdx, CToolObj::TOOLOBJ_DESC& pDesc)
{
    // Layer 분류
    switch (iLayerIdx)
    {
    case 0: // Monster
    {
        strcpy_s(pDesc.szLayer, "Layer_Monster");
        strcpy_s(pDesc.szObjName, Setting_ObjName(iLayerIdx, iSelectIdx));

        //Anim 모델 체크박스 확인
        if (CImgui_Manager::GetInstance()->isAnimModel())
        {
            pDesc.eModelType = CModel::TYPE_ANIM;
        }
        else
        {
            pDesc.eModelType = CModel::TYPE_NONANIM;
        }

        //OBJ 이름 설정
        vector<wstring>::iterator iter = m_Monsters.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szModelName, MAX_PATH, nullptr, nullptr);

        break;
    }
    case 1: // Layer_Passive_Element
    {
        strcpy_s(pDesc.szLayer, "Layer_Passive_Element");
        strcpy_s(pDesc.szObjName, Setting_ObjName(iLayerIdx, iSelectIdx));

        //Anim 모델 체크박스 확인
        if (CImgui_Manager::GetInstance()->isAnimModel())
        {
            pDesc.eModelType = CModel::TYPE_ANIM;
        }
        else
        {
            pDesc.eModelType = CModel::TYPE_NONANIM;
        }

        vector<wstring>::iterator iter = m_PassiveElements.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szModelName, MAX_PATH, nullptr, nullptr);

        break;
    }
    case 2: // Layer_Active_Element
    {
        strcpy_s(pDesc.szLayer, "Layer_Active_Element");
        strcpy_s(pDesc.szObjName, Setting_ObjName(iLayerIdx, iSelectIdx));

        //Anim 모델 체크박스 확인
        if (CImgui_Manager::GetInstance()->isAnimModel())
        {
            pDesc.eModelType = CModel::TYPE_ANIM;
        }
        else
        {
            pDesc.eModelType = CModel::TYPE_NONANIM;
        }


        vector<wstring>::iterator iter = m_ActiveElements.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szModelName, MAX_PATH, nullptr, nullptr); // wstring을 _char로?

        break;
    }
    case 3: // Layer_Trigger
    {
        strcpy_s(pDesc.szLayer, "Layer_Trigger");
        strcpy_s(pDesc.szObjName, "Prototype_GameObject_EventTrigger");
        {
            pDesc.eModelType = CModel::TYPE_NONANIM;
        }
        strcpy_s(pDesc.szModelName, "Prototype_Component_Model_BasicCube");

        break;
    }
    default:
        break;
    }
}

const char* CToolObj_Manager::Setting_ObjName(_int iLayerIdx, _int iSelectIdx)
{

    // Monster
    if (0 == iLayerIdx)
    {
        return "Prototype_GameObject_Monster";

        //추후 몬스터 종류별로 이름 추가

        switch (iSelectIdx)
        {
        case 0:
            return "Prototype_GameObject_Mst_CatFish_Gunner";
        case 1:
            return "Prototype_GameObject_Mst_Lantern";
        case 2:
            return "Prototype_GameObject_Mst_Octopus";
        case 3:
            return "Prototype_GameObject_Mst_Crab";
        case 4:
            return "Prototype_GameObject_Mst_EliteMst";
        case 5:
            return "Prototype_GameObject_Mst_Boss";
        default:
            break;
        }
    }
    // Passive Element
    else if (1 == iLayerIdx)     
    {

        //Anim 모델 체크박스 확인
        if (CImgui_Manager::GetInstance()->isDecoObject())
        {
            return "Prototype_GameObject_Deco_Element";
        }
        else
        {
            return "Prototype_GameObject_Passive_Element";
        }

    }
    // Active Element
    else if (2 == iLayerIdx)     
    {
        //추후 ActiveElement 종류별로 이름 추가
        switch (iSelectIdx)
        {
        case 0:
            return "Prototype_GameObject_Grass";
        case 1:
            return "Prototype_GameObject_TutorialMapBridge";
        case 2:
            return "Prototype_GameObject_Well";
        case 3:
            return "Prototype_GameObject_FakeWall";
        case 4:
            return "Prototype_GameObject_FakeWall";
        case 5:
            return "Prototype_GameObject_EventTrigger";
        case 6:
            return "Prototype_GameObject_EventTrigger";
        case 7:
            return "Prototype_GameObject_Active_Element";
        case 8:
            return "Prototype_GameObject_Active_Element";
        default:
            break;
        }
    }

    return nullptr;
}

HRESULT CToolObj_Manager::Delete_Obj(_int iSelectIdx)
{
    if (iSelectIdx >= m_ToolObjs.size())
        return E_FAIL;

    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();
    for (int i = 0; i < iSelectIdx; ++i)
        ++iter;

    CGameObject* pToolObj = *iter;  // 삭제할 CToolObj 포인터를 저장

    //이벤트 매니저로 삭제
    //if(FAILED(m_pGameInstance->Delete_Object(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), iSelectIdx)))
    //    return E_FAIL; 
    m_pGameInstance->Erase(pToolObj);  // 저장한 포인터를 사용하여 Erase() 함수 호출
    m_ToolObjs.erase(iter);

    return S_OK;
}
void CToolObj_Manager::Free()
{
    //for (auto& pObj : m_ToolObjs)
    //    Safe_Release(pObj);

    m_ActiveElements.clear();
    m_PassiveElements.clear();
    m_Monsters.clear();
    m_ToolObjs.clear();

    Safe_Release(m_pGameInstance);
}
