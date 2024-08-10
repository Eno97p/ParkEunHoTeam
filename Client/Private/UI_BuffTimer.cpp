#include "UI_BuffTimer.h"

#include "GameInstance.h"

#include "UI_BuffTimer_Bar.h"
#include "UI_BuffTimer_Timer.h"

CUI_BuffTimer::CUI_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_BuffTimer::CUI_BuffTimer(const CUI_BuffTimer& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_BuffTimer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_BuffTimer::Initialize(void* pArg)
{
    UI_BUFFTIMER_DESC* pDesc = static_cast<UI_BUFFTIMER_DESC*>(pArg);

    m_iBuffTimerIdx = pDesc->iBuffTimerIdx;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUI_BuffTimer::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BuffTimer::Tick(_float fTimeDelta)
{
    // 여기서 생성된 지 몇초인지 시간을 재고 그 시간이 끝나면 해당 객체 없애버리는 식으로 > BuffTimer UI는 이 값을 받아와서 매번 비율을 갱신해주는 식으로 하는 것이 좋을 듯함

    Setting_UIPosition();

    for (auto& pUI : m_vecUI)
        pUI->Tick(fTimeDelta);
}

void CUI_BuffTimer::Late_Tick(_float fTimeDelta)
{
    for (auto& pUI : m_vecUI)
        pUI->Late_Tick(fTimeDelta);
}

HRESULT CUI_BuffTimer::Render()
{
    return S_OK;
}

HRESULT CUI_BuffTimer::Create_UI()
{
    // Bar
    CUI::UI_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer_Bar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer_Bar"), &pDesc)));

    // Timer 
    m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer_Timer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer_Timer"), &pDesc)));

    return S_OK;
}

void CUI_BuffTimer::Setting_UIPosition()
{
    // Index에 따라 UI들의 Position을 세팅하는 함수
    // 각 UI들은 이 값을 받아가서 본인들의 OffSet에 맞게 세팅해야 함

    _float fX = { 0.f };

    switch (m_iBuffTimerIdx)
    {
    case 0:
        fX = 80.f;
        break;
    case 1:
        fX = 160.f;
        break;
    case 2:
        fX = 240.f;
        break;
    case 3:
        fX = 320.f;
        break;
    default:
        break;
    }

    vector<CUI*>::iterator ui = m_vecUI.begin();
    dynamic_cast<CUI_BuffTimer_Bar*>(*ui)->Update_Position(fX);

    // ++ 해서 추가해주기
    ++ui;
    dynamic_cast<CUI_BuffTimer_Timer*>(*ui)->Update_Position(fX);
}

CUI_BuffTimer* CUI_BuffTimer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_BuffTimer* pInstance = new CUI_BuffTimer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUI_BuffTimer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_BuffTimer::Clone(void* pArg)
{
    CUI_BuffTimer* pInstance = new CUI_BuffTimer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUI_BuffTimer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_BuffTimer::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
