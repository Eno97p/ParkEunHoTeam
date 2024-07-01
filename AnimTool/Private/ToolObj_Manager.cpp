#include "ToolObj_Manager.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"

#include "Monster.h"
#include "ToolObj.h"

IMPLEMENT_SINGLETON(CToolObj_Manager)

CToolObj_Manager::CToolObj_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
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

_uint CToolObj_Manager::Get_CurKeyFrame(_uint iAnimIdx, _uint iChannelIdx)
{
    _uint iCurKeyFrame = { 0 };

    if (!m_ToolObjs.empty())
    {
        vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

        if ((*iter) != nullptr)
            iCurKeyFrame = (*iter)->Get_CurKeyFrame(iAnimIdx, iChannelIdx);
    }

    return iCurKeyFrame;
}

_double CToolObj_Manager::Get_Duration(_uint iAnimIdx)
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    return     (*iter)->Get_Duration(iAnimIdx);
}

const _double* CToolObj_Manager::Get_CurrentPos(_uint iAnimIdx)
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    return    (*iter)->Get_CurrentPos(iAnimIdx);
}

HRESULT CToolObj_Manager::Add_CloneObj(_int iLayerIdx, _int iSelectIdx, _vector vPosition)
{
    // 새로 생성하는 데이터

    CToolObj::TOOLOBJ_DESC pDesc = {};

    Setting_Desc(iLayerIdx, iSelectIdx, pDesc);

    // vPosition
    _float4 vPositionStore;
    XMStoreFloat4(&vPositionStore, vPosition); // 저장용으로 바꿔줌
    _matrix WorldMatrix = XMLoadFloat4x4(&pDesc.mWorldMatrix); // 연산용으로 변경

    WorldMatrix = XMMatrixIdentity();

    WorldMatrix.r[3] = vPosition;
    XMStoreFloat4x4(&pDesc.mWorldMatrix, WorldMatrix);

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_ToolObj"), &pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolObj_Manager::Add_AnimModel(_int iSelectIdx)
{
    CToolObj::TOOLOBJ_DESC pDesc = {};

    strcpy_s(pDesc.szObjName, Setting_ObjName(iSelectIdx)); // 어차피 이 값은 로드용
    strcpy_s(pDesc.szLayer, "Layer_AnimObj");
    strcpy_s(pDesc.szModelName, Setting_ObjName(iSelectIdx)); // 어차피 이 값은 로드용

    XMStoreFloat4x4(&pDesc.mWorldMatrix, XMMatrixIdentity());

    if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_AnimObj"), TEXT("Prototype_ToolObj"), &pDesc)))
        return E_FAIL;

    return S_OK;
}

void CToolObj_Manager::Initialize()
{
}

void CToolObj_Manager::Setting_Desc(_int iLayerIdx, _int iSelectIdx, CToolObj::TOOLOBJ_DESC& pDesc)
{
    // Layer 분류
    switch (iLayerIdx)
    {
    case 0: // Monster
    {
        strcpy_s(pDesc.szLayer, "Layer_Monster");
        strcpy_s(pDesc.szModelName, Setting_ObjName(/*iLayerIdx, */iSelectIdx));
        pDesc.eModelType = CModel::TYPE_ANIM;

        vector<wstring>::iterator iter = m_MonsterName.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szObjName, MAX_PATH, nullptr, nullptr);

        break;
    }
    case 1: // Map Element
    {
        strcpy_s(pDesc.szLayer, "Layer_MapElement");
        strcpy_s(pDesc.szModelName, "Prototype_GameObject_MapElement");

        pDesc.eModelType = CModel::TYPE_NONANIM; // Map Element는 무조건 NonAnim 일듯

        vector<wstring>::iterator iter = m_MapElement.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szObjName, MAX_PATH, nullptr, nullptr);

        break;
    }
    case 2: // Map Obj
    {
        strcpy_s(pDesc.szLayer, "Layer_MapObj");

        strcpy_s(pDesc.szModelName, Setting_ObjName(/*iLayerIdx, */iSelectIdx)); //     "Prototype_GameObject_MapObj"

        if (4 == iSelectIdx || 5 == iSelectIdx || 7 == iSelectIdx) // 총 관련 MapObj(Item)
            pDesc.eModelType = CModel::TYPE_NONANIM;
        else
            pDesc.eModelType = CModel::TYPE_ANIM;


        vector<wstring>::iterator iter = m_MapObj.begin();
        for (int i = 0; i < iSelectIdx; ++i)
        {
            iter++;
        }
        WideCharToMultiByte(CP_ACP, 0, (*iter).c_str(), (*iter).length(), pDesc.szObjName, MAX_PATH, nullptr, nullptr); // wstring을 _char로?

        break;
    }
    default:
        break;
    }
}

const char* CToolObj_Manager::Setting_ObjName(/*_int iLayerIdx, */_int iSelectIdx)
{
    switch (iSelectIdx)
    {
    case 0:
        return "Prototype_GameObject_Wanda";
    case 1:
        return "Prototype_GameObject_Homonculus";
    case 2:
        return "Prototype_GameObject_JobMob";
    case 3:
        return "Prototype_GameObject_Juggulus";
    case 4:
        return "Prototype_GameObject_Malkhel";
    case 5:
        return "Prototype_GameObject_Mantari";
    case 6:
        return "Prototype_GameObject_NPC_Choron";
    case 7:
        return "Prototype_GameObject_NPC_Valnir";
    case 8:
        return "Prototype_GameObject_Arrow_Jobmob";
    default:
        break;
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

    m_ToolObjs.erase(iter);

    /*if(FAILED(m_pGameInstance->Delete_Object(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), iSelectIdx)))
        return E_FAIL; */

    return S_OK;
}

HRESULT CToolObj_Manager::Delete_AnimModel()
{
    if (m_ToolObjs.empty())
        return S_OK;

    m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_AnimObj"));

    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    m_ToolObjs.erase(iter);

    m_ToolObjs.clear();

    if (m_ToolObjs.empty())
        return S_OK;
    else
        return E_FAIL;
}

void CToolObj_Manager::Setting_ChannelList()
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    (*iter)->Setting_ChannelList();
}

void CToolObj_Manager::Change_AnimState(_uint iAnimState)
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    (*iter)->Change_AnimState(iAnimState);
}

void CToolObj_Manager::Set_Animation(_uint iAnimIdx,_bool isLoop)
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    (*iter)->Set_Animation(iAnimIdx, isLoop);
}

void CToolObj_Manager::Setting_KeyFrameSliders(_uint iAnimIdx, _uint iChannelIdx)
{
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    (*iter)->Setting_KeyFrameSliders(iAnimIdx, iChannelIdx);
}

const _tchar* CToolObj_Manager::Setting_FileName()
{
    _int iFileIdx = CImgui_Manager::GetInstance()->Get_FileIdx();

    switch (iFileIdx)
    {
    case 0:
        return  L"../../Client/Bin/DataFiles/Animation/Wander.dat";
    case 1:
        return  L"../../Client/Bin/DataFiles/Animation/Homomculus.dat";
    case 2:
        return  L"../../Client/Bin/DataFiles/Animation/Job_Mob.dat";
    case 3:
        return  L"../../Client/Bin/DataFiles/Animation/Juggulus.dat";
    case 4:
        return  L"../../Client/Bin/DataFiles/Animation/Malkhel.dat";
    case 5:
        return  L"../../Client/Bin/DataFiles/Animation/Mantari.dat";
    case 6:
        return  L"../../Client/Bin/DataFiles/Animation/Npc_Choron.dat";
    case 7:
        return  L"../../Client/Bin/DataFiles/Animation/Npc_Valnir.dat";
    default:
        MSG_BOX("Setting File Name is Failed");
        return L"\0";
    }

    return L"\0";
}

HRESULT CToolObj_Manager::Save_Data()
{
    const wchar_t* wszFileName[MAX_PATH] = { Setting_FileName() };
    HANDLE	hFile = CreateFile(*wszFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    DWORD       dwByte(0);
    _bool       isLoop = { false };     // 반복 여부
  
    _char       szSoundFile[MAX_PATH] = "";
    _char       szEffectFile[MAX_PATH] = "";    // 추후 파일명이 아니라 Desc로 불러오긴 할 것
    _uint       iBoneIdx = { 0 };               // Bone Idx
    _bool       isSave = { false };             // Collider 저장 여부
    _bool       isAble = { false };
    _uint       iColliderBoneIdx = { 0 };
    _float      fStartKeyframe = { 0.0 };
    _float      fEndKeyframe = { 0.0 };
    

    // 애니메이션 개수만큼 루프
    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    for (size_t i = 0; i < (*iter)->Get_NumAnim(); ++i)
    {
        isLoop = CImgui_Manager::GetInstance()->Get_isLoop();
        WriteFile(hFile, &isLoop, sizeof(_bool), &dwByte, nullptr);

        vector<string> vecApplySound = *CImgui_Manager::GetInstance()->Get_ApplySound(i);
        _uint iApplySoundSize = vecApplySound.size();
        WriteFile(hFile, &iApplySoundSize, sizeof(_uint), &dwByte, nullptr);

        // Sound
        for (auto& pSound : vecApplySound) // Apply Sound 개수만큼 저장
        {
            strcpy_s(szSoundFile, pSound.c_str());
            WriteFile(hFile, &szSoundFile, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        }

        // Effect
        vector<string> vecApplyEffect = *CImgui_Manager::GetInstance()->Get_ApplyEffect(i);
        _uint iApplyEffectSize = vecApplyEffect.size();
        WriteFile(hFile, &iApplyEffectSize, sizeof(_uint), &dwByte, nullptr);
        for (auto& pEffect : vecApplyEffect) // Apply Effect 개수만큼 저장
        {
            strcpy_s(szEffectFile, pEffect.c_str());
            WriteFile(hFile, szEffectFile, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
        }

        for (auto& pEffectBoneIdx : *CImgui_Manager::GetInstance()->Get_EffectBoneIdx(i)) // Apply Effect BoneIdx 개수만큼 저장
        {
            iBoneIdx = pEffectBoneIdx;
            WriteFile(hFile, &iBoneIdx, sizeof(_int), &dwByte, nullptr);
        }

        isSave = CImgui_Manager::GetInstance()->IsColliderSave(i);
        WriteFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);

        if (isSave) // Collider 저장 여부 True 일 때 데이터 저장
        {
            isAble = CImgui_Manager::GetInstance()->Get_isAble(i); // 멤버 변수가 아니라 map에서 꺼내와야 하지 않을지
            iColliderBoneIdx = CImgui_Manager::GetInstance()->Get_ColliderBoneIdx(i);
            fStartKeyframe = CImgui_Manager::GetInstance()->Get_StartKeyframe(i);
            fEndKeyframe = CImgui_Manager::GetInstance()->Get_EndKeyframe(i);

            WriteFile(hFile, &isAble, sizeof(_bool), &dwByte, nullptr);
            WriteFile(hFile, &iColliderBoneIdx, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &fStartKeyframe, sizeof(_float), &dwByte, nullptr);
            WriteFile(hFile, &fEndKeyframe, sizeof(_float), &dwByte, nullptr);
        }
    }

    CloseHandle(hFile);

    MSG_BOX("Animation Data Save");

    return S_OK;
}

HRESULT CToolObj_Manager::Load_Data()
{
    // Binary 파일에서 읽어온 값들로 갱신 (기존의 데이터 지울 것)

    HANDLE	hFile = CreateFile(Setting_FileName(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // L"../../Data/Test.dat"
    if (nullptr == hFile)
        return E_FAIL;

    DWORD       dwByte(0);
    _bool       isLoop = { false };     // 반복 여부
    _char       szSoundFile[MAX_PATH] = "";
    _char       szEffectFile[MAX_PATH] = "";    // 추후 파일명이 아니라 Desc로 불러오긴 할 것
    _uint       iBoneIdx = { 0 };               // Bone Idx
    _bool       isSave = { false };             // Collider 저장 여부
    _bool       isAble = { false };
    _uint       iColliderBoneIdx = { 0 };
    _float      fStartKeyframe = { 0.0 };
    _float      fEndKeyframe = { 0.0 };

    vector<CToolObj*>::iterator iter = m_ToolObjs.begin();

    for (size_t i = 0; i < (*iter)->Get_NumAnim(); ++i)
    {
        ReadFile(hFile, &isLoop, sizeof(_bool), &dwByte, nullptr);
        CImgui_Manager::GetInstance()->Set_isLoop(isLoop);

        // 기존 데이터 없애는 코드 필요 !!!!!!!!!!!
        
        // Apply Sound에 저장 필요
        _uint iApplySoundSize;
        ReadFile(hFile, &iApplySoundSize, sizeof(_uint), &dwByte, nullptr);

        for (size_t j = 0; j < iApplySoundSize; ++j)
        {
            ReadFile(hFile, &szSoundFile, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            CImgui_Manager::GetInstance()->Load_ApplySound(i, szSoundFile); // ""으로 값을 넣으면 잘 들어감
        }

        _uint iApplyEffectSize;
        ReadFile(hFile, &iApplyEffectSize, sizeof(_uint), &dwByte, nullptr);

        for (size_t j = 0; j < iApplyEffectSize; ++j)
        {
            ReadFile(hFile, szEffectFile, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
            CImgui_Manager::GetInstance()->Load_ApplyEffect(i, szEffectFile);
        }

        for (size_t j = 0; j < iApplyEffectSize; ++j)
        {
            ReadFile(hFile, &iBoneIdx, sizeof(_int), &dwByte, nullptr);
            CImgui_Manager::GetInstance()->Load_EffectBoneIdx(i, iBoneIdx);
        }

        // Collider
        ReadFile(hFile, &isSave, sizeof(_bool), &dwByte, nullptr);
        if (isSave) // Collider 저장 여부 True 일 때만 받아오기
        {
            ReadFile(hFile, &isAble, sizeof(_bool), &dwByte, nullptr);
            ReadFile(hFile, &iColliderBoneIdx, sizeof(_uint), &dwByte, nullptr);
            ReadFile(hFile, &fStartKeyframe, sizeof(_float), &dwByte, nullptr);
            ReadFile(hFile, &fEndKeyframe, sizeof(_float), &dwByte, nullptr);

            if (0 == dwByte)
                break;

            CImgui_Manager::GetInstance()->Load_isAble(i, isAble);
            CImgui_Manager::GetInstance()->Load_ColliderIdx(i, iColliderBoneIdx);
            CImgui_Manager::GetInstance()->Load_StartKeyframe(i, fStartKeyframe);
            CImgui_Manager::GetInstance()->Load_EndKeyframe(i, fEndKeyframe);
        }
    }

    CloseHandle(hFile);

    MSG_BOX("Animation Data Load");

    return S_OK;
}

void CToolObj_Manager::Free()
{
    for (auto& pObj : m_ToolObjs)
        Safe_Release(pObj);

    m_MapObj.clear();
    m_MapElement.clear();
    m_MonsterName.clear();
    m_ToolObjs.clear();

    Safe_Release(m_pGameInstance);
}
