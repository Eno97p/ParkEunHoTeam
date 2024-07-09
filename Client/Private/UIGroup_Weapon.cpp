#include "UIGroup_Weapon.h"

#include "GameInstance.h"

#include "UI_MenuPageBG.h"
#include "UI_MenuPageTop.h"
#include "UI_QuickExplain.h"
#include "UI_Slot.h"
#include "UI_QuickInvBG.h"
#include "UI_WeaponRTop.h"
#include "UI_WPEquipSlot.h"
#include "UI_WPFontaine.h"
#include "UI_WeaponTab.h"

CUIGroup_Weapon::CUIGroup_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Weapon::CUIGroup_Weapon(const CUIGroup_Weapon& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Weapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	m_eTabType = TAB_L; // �⺻ : Weapon

	return S_OK;
}

void CUIGroup_Weapon::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Weapon::Tick(_float fTimeDelta)
{
	_bool isRender_End = false;
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			if (!m_isRenderOnAnim && !(pUI->Get_RenderOnAnim()))
			{
				pUI->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pUI->Get_RenderOnAnim())
			{
				pUI->Resset_Animation(false);
			}

			pUI->Tick(fTimeDelta);

			isRender_End = pUI->isRender_End();
		}
		if (isRender_End)
			m_isRend = false;
	}

	if (m_pGameInstance->Key_Down(DIK_TAB))
	{
		Change_Tab();
	}
}

void CUIGroup_Weapon::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Weapon::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_UI()
{
	CUI::UI_DESC pDesc{};

	// BG
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageBG"), &pDesc)));

	// Top
	CUI_MenuPageTop::UI_TOP_DESC pTopDesc{};
	pTopDesc.eLevel = LEVEL_STATIC;
	pTopDesc.eTopType = CUI_MenuPageTop::TOP_INV;
	m_vecUI.emplace_back(dynamic_cast<CUI_MenuPageTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPageTop"), &pTopDesc)));

	// InvBG
	CUI_QuickInvBG::UI_INVBG_DESC pInvBGDesc{};
	pInvBGDesc.eLevel = LEVEL_STATIC;
	pInvBGDesc.isInv = true;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickInvBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickInvBG"), &pInvBGDesc)));

	// Explain
	CUI_QuickExplain::UI_EXPLAIN_DESC pExplainDesc{};
	pExplainDesc.eLevel = LEVEL_STATIC;
	pExplainDesc.isInv = true;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickExplain*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickExplain"), &pExplainDesc)));

	// RTop
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_WeaponRTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponRTop"), &pDesc)));

	Create_Tab();
	Create_Slot();
	Create_EquipSlot();
	Create_Fontaine();

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Slot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 5; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = 230.f + (j * 76.f); // 160
			pDesc.fY = 200.f + (i * 76.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.eSlotType = CUI_Slot::SLOT_INV;
			pDesc.eUISort = NINETH;
			m_vecUI.emplace_back(dynamic_cast<CUI*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		}
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_EquipSlot()
{
	CUI_WPEquipSlot::UI_EQUIPSLOT_DESC pDesc{};

	CUI_WPEquipSlot::SLOT_NUM arrSlotNum[CUI_WPEquipSlot::NUM_END]
		= { CUI_WPEquipSlot::NUM_ONE,  CUI_WPEquipSlot::NUM_TWO, CUI_WPEquipSlot::NUM_THREE };

	for (size_t i = 0; i < 3; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.eSlotNum = arrSlotNum[i];
		m_vecUI.emplace_back(dynamic_cast<CUI_WPEquipSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPEquipSlot"), &pDesc)));
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Fontaine()
{
	CUI_WPFontaine::UI_FONTAINE_DESC pDesc{};

	for (size_t i = 0; i < 3; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.iSlotNum = i;
		m_vecUI.emplace_back(dynamic_cast<CUI_WPFontaine*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPFontaine"), &pDesc)));
	}

	return S_OK;
}

HRESULT CUIGroup_Weapon::Create_Tab()
{
	CUI_WeaponTab::UI_TAB_DESC	pDesc{};
	CUI_WeaponTab::TAB_TYPE		arrType[CUI_WeaponTab::TAB_END] = { CUI_WeaponTab::TAB_L, CUI_WeaponTab::TAB_R };

	for (size_t i = 0; i < 2; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.eTabType = arrType[i];
		m_vecUI.emplace_back(dynamic_cast<CUI_WeaponTab*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WeaponTab"), &pDesc)));
	}

	return S_OK;
}

void CUIGroup_Weapon::Change_Tab()
{
	if (TAB_L == m_eTabType)
		m_eTabType = TAB_R;
	else if (TAB_R == m_eTabType)
		m_eTabType = TAB_L;



}

CUIGroup_Weapon* CUIGroup_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Weapon* pInstance = new CUIGroup_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Weapon");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Weapon::Clone(void* pArg)
{
	CUIGroup_Weapon* pInstance = new CUIGroup_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Weapon");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Weapon::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
