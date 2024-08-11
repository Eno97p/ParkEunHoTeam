#include "UI_BuffTimer.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_BuffTimer_Bar.h"
#include "UI_BuffTimer_Timer.h"
#include "UI_ItemIcon.h"

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
    m_wstrTextureName = pDesc->wstrTextureName;

    if (FAILED(Create_UI(m_wstrTextureName)))
        return E_FAIL;

    return S_OK;
}

void CUI_BuffTimer::Priority_Tick(_float fTimeDelta)
{
}

void CUI_BuffTimer::Tick(_float fTimeDelta)
{
    m_fBuffTimer += fTimeDelta;
    if (60.f <= m_fBuffTimer)
    {
        m_isBuffEnd = true;
    }

    Setting_UIPosition();

    Update_BuffTime();

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

HRESULT CUI_BuffTimer::Create_UI(wstring wstrTextureName)
{
    // Bar
    CUI::UI_DESC pDesc{};
    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer_Bar*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer_Bar"), &pDesc)));

    // Timer 
    m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer_Timer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer_Timer"), &pDesc)));

    // ItemIcon
    CUI_ItemIcon::UI_ITEMICON_DESC pIconDesc{};
    pIconDesc.eLevel = LEVEL_STATIC;
    pIconDesc.eUISort = FIRST;
    pIconDesc.fX = 0.f;
    pIconDesc.fY = 0.f;
    pIconDesc.fSizeX = 60.f;
    pIconDesc.fSizeY = 60.f;
    pIconDesc.wszTexture = wstrTextureName;
    m_vecUI.emplace_back(dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pIconDesc)));

    return S_OK;
}

void CUI_BuffTimer::Setting_UIPosition()
{
    _float fX = { 0.f };
    fX = 80.f * (m_iBuffTimerIdx + 1);

    vector<CUI*>::iterator ui = m_vecUI.begin();
    dynamic_cast<CUI_BuffTimer_Bar*>(*ui)->Update_Position(fX);

    ++ui;
    dynamic_cast<CUI_BuffTimer_Timer*>(*ui)->Update_Position(fX);

    ++ui;
    dynamic_cast<CUI_ItemIcon*>(*ui)->Update_Pos(fX, (g_iWinSizeY >> 1) + 100.f);
}

void CUI_BuffTimer::Update_BuffTime()
{
    vector<CUI*>::iterator timer = m_vecUI.begin();
    ++timer;

    dynamic_cast<CUI_BuffTimer_Timer*>(*timer)->Set_BuffTime(m_fBuffTimer);

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
