#include "UIGroup_WeaponSlot.h"

#include "GameInstance.h"

#include "UI_WeaponSlotBG.h"
#include "UI_WeaponSlot.h"

CUIGroup_WeaponSlot::CUIGroup_WeaponSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{pDevice, pContext}
{
}

CUIGroup_WeaponSlot::CUIGroup_WeaponSlot(const CUIGroup_WeaponSlot& rhs)
    : CUIGroup{rhs}
{
}

HRESULT CUIGroup_WeaponSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_WeaponSlot::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_WeaponSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_WeaponSlot::Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Tick(fTimeDelta);
    }
}

void CUIGroup_WeaponSlot::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_WeaponSlot::Render()
{
    return S_OK;
}

HRESULT CUIGroup_WeaponSlot::Create_UI()
{
    // WeaponSlot BG
    CUI::UI_DESC pDesc{};

    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlotBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlotBG"), &pDesc)));

    // WeaponSlot
    CUI_WeaponSlot::UI_WEAPONSLOT_DESC pSlotDesc;

    // Slot One
    pSlotDesc.eLevel = LEVEL_STATIC;
    pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_ONE;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

    // Slot Two
    ZeroMemory(&pSlotDesc, sizeof(pSlotDesc));
    pSlotDesc.eLevel = LEVEL_STATIC;
    pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_TWO;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

    // Slot Three
    ZeroMemory(&pSlotDesc, sizeof(pSlotDesc));
    pSlotDesc.eLevel = LEVEL_STATIC;
    pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_THREE;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

    return S_OK;
}

CUIGroup_WeaponSlot* CUIGroup_WeaponSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUIGroup_WeaponSlot* pInstance = new CUIGroup_WeaponSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUIGroup_WeaponSlot");
        return nullptr;
    }

    return pInstance;
}

CGameObject* CUIGroup_WeaponSlot::Clone(void* pArg)
{
    CUIGroup_WeaponSlot* pInstance = new CUIGroup_WeaponSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIGroup_WeaponSlot");
        return nullptr;
    }

    return pInstance;
}

void CUIGroup_WeaponSlot::Free()
{
    __super::Free();

    for (auto& pUI : m_vecUI)
        Safe_Release(pUI);
}
