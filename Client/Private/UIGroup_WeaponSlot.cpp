#include "UIGroup_WeaponSlot.h"

#include "GameInstance.h"
#include "Inventory.h"

#include "UI_WeaponSlotBG.h"
#include "UI_WeaponSlot.h"
#include "UI_ItemIcon.h"

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

        if (nullptr != m_pWeaponSlot)
            m_pWeaponSlot->Tick(fTimeDelta);

        if (nullptr != m_pSkillSlot)
            m_pSkillSlot->Tick(fTimeDelta);

        if (nullptr != m_pQuickSlot)
            m_pQuickSlot->Tick(fTimeDelta); // Inventory의 Quick vector에서 얻어올 것
    }
}

void CUIGroup_WeaponSlot::Late_Tick(_float fTimeDelta)
{
    if (m_isRend)
    {
        for (auto& pUI : m_vecUI)
            pUI->Late_Tick(fTimeDelta);

        if (nullptr != m_pWeaponSlot)
            m_pWeaponSlot->Late_Tick(fTimeDelta);

        if (nullptr != m_pSkillSlot)
            m_pSkillSlot->Late_Tick(fTimeDelta);

        if (nullptr != m_pQuickSlot)
            m_pQuickSlot->Late_Tick(fTimeDelta);
    }
}

HRESULT CUIGroup_WeaponSlot::Render()
{
    return S_OK;
}

void CUIGroup_WeaponSlot::Update_QuickSlot(wstring wstrTextureName)
{
    m_pQuickSlot->Change_Texture(wstrTextureName); // 터짐 (null 이슈?)
}

HRESULT CUIGroup_WeaponSlot::Create_UI()
{
    // WeaponSlot BG
    CUI::UI_DESC pDesc{};

    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlotBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlotBG"), &pDesc)));

    CUI_ItemIcon::UI_ITEMICON_DESC pIconDesc{};

    ZeroMemory(&pIconDesc, sizeof(pIconDesc));
    pIconDesc.eLevel = LEVEL_STATIC;
    pIconDesc.fX = 190.f;
    pIconDesc.fY = g_iWinSizeY - 100.f;
    pIconDesc.fSizeX = 64.f;
    pIconDesc.fSizeY = 64.f;
    pIconDesc.eUISort = SECOND;
    pIconDesc.wszTexture = TEXT("Prototype_Component_Texture_ItemIcon_None"); // 임시로? 투명 텍스쳐
    m_pQuickSlot = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pIconDesc));

    //// WeaponSlot
    //CUI_WeaponSlot::UI_WEAPONSLOT_DESC pSlotDesc;

    //// Slot One
    //pSlotDesc.eLevel = LEVEL_STATIC;
    //pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_ONE;
    //m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

    //// Slot Two
    //ZeroMemory(&pSlotDesc, sizeof(pSlotDesc));
    //pSlotDesc.eLevel = LEVEL_STATIC;
    //pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_TWO;
    //m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

    //// Slot Three
    //ZeroMemory(&pSlotDesc, sizeof(pSlotDesc));
    //pSlotDesc.eLevel = LEVEL_STATIC;
    //pSlotDesc.eSlotNum = CUI_WeaponSlot::SLOT_THREE;
    //m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlot"), &pSlotDesc)));

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

    Safe_Release(m_pWeaponSlot);
    Safe_Release(m_pSkillSlot);
    Safe_Release(m_pQuickSlot);
}
