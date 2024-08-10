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
    // 여기서 생성된 지 몇초인지 시간을 재고 그 시간이 끝나면 해당 객체 없애버리는 식으로 > BuffTimer UI는 이 값을 받아와서 매번 비율을 갱신해주는 식으로 하는 것이 좋을 듯함

    m_fBuffTimer += fTimeDelta;
    if (5.f <= m_fBuffTimer)
    {
        // 삭제해주어야 함
        m_isBuffEnd = true;
    }

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
    pIconDesc.wszTexture = wstrTextureName; // 여기에는 방금 사용한 아이템의 정보를 넣어주어야 함 >> 일단 위치 Test를 위해 임의로 넣기
    // 아이템 사용 시 해당 객체를 생성할 것이므로 그때 인자로 값을 넣어주는 식으로 하면 될 거 같움
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
