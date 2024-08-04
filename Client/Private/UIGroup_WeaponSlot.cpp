#include "UIGroup_WeaponSlot.h"

#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"

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
        Key_Input();

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
    m_pQuickSlot->Change_Texture(wstrTextureName);
}

void CUIGroup_WeaponSlot::Update_WeaponSlot(wstring wstrTextureName, SLOT_TYPE eSlotType)
{
    if (SLOT_QUICK == eSlotType)
    {
        m_pQuickSlot->Change_Texture(wstrTextureName);
    }
    else if (SLOT_WEAPON == eSlotType)
    {
        m_pWeaponSlot->Change_Texture(wstrTextureName);
    }
    else if (SLOT_SKILL == eSlotType)
    {
        m_pSkillSlot->Change_Texture(wstrTextureName);
    }
}

void CUIGroup_WeaponSlot::Reset_SlotTexture(SLOT_TYPE eSlotType)
{
    if (SLOT_QUICK == eSlotType)
    {
        m_pQuickSlot->Change_Texture(TEXT("Prototype_Component_Texture_ItemIcon_None"));
    }
    else if (SLOT_WEAPON == eSlotType)
    {
        m_pWeaponSlot->Change_Texture(TEXT("Prototype_Component_Texture_ItemIcon_None"));
    }
    else if (SLOT_SKILL == eSlotType)
    {
        m_pSkillSlot->Change_Texture(TEXT("Prototype_Component_Texture_ItemIcon_None"));
    }
}

HRESULT CUIGroup_WeaponSlot::Create_UI()
{
    // WeaponSlot BG
    CUI::UI_DESC pDesc{};

    pDesc.eLevel = LEVEL_STATIC;
    m_vecUI.emplace_back(dynamic_cast<CUI_WeaponSlotBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponSlotBG"), &pDesc)));

    CUI_ItemIcon::UI_ITEMICON_DESC pIconDesc{};

    // QuickSlot
    //ZeroMemory(&pIconDesc, sizeof(pIconDesc)); >> wstring 있을 때는 Zeromemory 사용 X
    pIconDesc.eLevel = LEVEL_STATIC;
    pIconDesc.fX = 186.f;
    pIconDesc.fY = g_iWinSizeY - 96.f;
    pIconDesc.fSizeX = 64.f;
    pIconDesc.fSizeY = 64.f;
    pIconDesc.eUISort = SECOND;
    pIconDesc.wszTexture = TEXT("Prototype_Component_Texture_ItemIcon_None");
    m_pQuickSlot = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pIconDesc));

    // Weapon Slot
    pIconDesc.fX = 90.f;
    pIconDesc.fY = g_iWinSizeY - 100.f;
    pIconDesc.eUISort = SECOND;
    pIconDesc.wszTexture = TEXT("Prototype_Component_Texture_ItemIcon_None");
    m_pWeaponSlot = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pIconDesc));

    // Skill Slot
    pIconDesc.fX = 140.f;
    pIconDesc.fY = g_iWinSizeY - 140.f;
    pIconDesc.eUISort = SECOND;
    pIconDesc.wszTexture = TEXT("Prototype_Component_Texture_ItemIcon_None");
    m_pSkillSlot = dynamic_cast<CUI_ItemIcon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_ItemIcon"), &pIconDesc));

    return S_OK;
}

void CUIGroup_WeaponSlot::Key_Input()
{
    if (m_pGameInstance->Key_Down(DIK_V)) // Quick Item Slot Change
    {
        if (!CInventory::GetInstance()->Get_isQuickEmpty()) // 아무 것도 없을 때 예외 처리
        {
            if (m_iQuickIdx < CInventory::GetInstance()->Get_QuickMapSize() - 1)
            {
                ++m_iQuickIdx;
            }
            else
            {
                m_iQuickIdx = 0;
            }

            map<_uint, CItemData*>::iterator quickaccess = CInventory::GetInstance()->Get_QuickMap()->begin();

            for (size_t i = 0; i < m_iQuickIdx; ++i)
                ++quickaccess;

            m_pQuickSlot->Change_Texture((*quickaccess).second->Get_TextureName());
        }
    }
    else if (m_pGameInstance->Key_Down(DIK_B)) // Quick Weapon Slot Change
    {
        if (m_iWeaponIdx < CInventory::GetInstance()->Get_EquipWeaponSize() - 1) // 2가 아니라 값을 넣어주어야
            ++m_iWeaponIdx;
        else
            m_iWeaponIdx = 0;

        while (true)
        {
            // 무기가 아예 없는 경우 예외 처리도 필요
            if (CInventory::GetInstance()->Get_EquipWeaponSize() == 0)
                break;

            if (CInventory::GetInstance()->Get_EquipWeapon(m_iWeaponIdx) == nullptr)
            {
                if (m_iWeaponIdx < 2)
                    ++m_iWeaponIdx;
                else
                    m_iWeaponIdx = 0;
            }
            else
                break;
        }

        // 값 없을 때 None 처리 필요
        if (CInventory::GetInstance()->Get_EquipWeaponSize() != 0)
        {
            m_pWeaponSlot->Change_Texture(CInventory::GetInstance()->Get_EquipWeapon(m_iWeaponIdx)->Get_TextureName());

            list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());

            // HUD의 Icon에 따라 Player가 실제로 들고 있는 Weapon 변경
            pPlayer->Update_Weapon(m_pWeaponSlot->Get_TextureName());

            // 현재 손에 들고 있는 무기를 Equip Slot에서 해제 시 처리는 x
        }
    }
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
