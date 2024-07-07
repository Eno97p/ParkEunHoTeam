#include "UIGroup_Quick.h"

#include "GameInstance.h"

#include "UI_QuickBG.h"
#include "UI_QuickTop.h"
#include "UI_QuickExplain.h"
#include "UI_QuickInvBG.h"
#include "UI_Slot.h"

CUIGroup_Quick::CUIGroup_Quick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{pDevice, pContext}
{
}

CUIGroup_Quick::CUIGroup_Quick(const CUIGroup_Quick& rhs)
	: CUIGroup{rhs}
{
}

HRESULT CUIGroup_Quick::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Quick::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Quick::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Quick::Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Tick(fTimeDelta);
	}
}

void CUIGroup_Quick::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_Quick::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Quick::Create_UI()
{
	CUI::UI_DESC pDesc{};

	// QuickBG 
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickBG"), &pDesc)));

	// QuickTop
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickTop"), &pDesc)));

	// Quick Explain
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.eLevel = LEVEL_STATIC;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickExplain*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickExplain"), &pDesc)));

	// Quick InvBG
	CUI_QuickInvBG::UI_INVBG_DESC pInvBGDesc{};
	pInvBGDesc.eLevel = LEVEL_STATIC;
	pInvBGDesc.isInv = false;
	m_vecUI.emplace_back(dynamic_cast<CUI_QuickInvBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QuickInvBG"), &pInvBGDesc)));

	Create_Slot();
	Create_InvSlot();

	return S_OK;
}

HRESULT CUIGroup_Quick::Create_Slot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 6; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = 160.f + (j * 76.f); // 160
			pDesc.fY = 300.f + (i * 76.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.eSlotType = CUI_Slot::SLOT_QUICK;
			pDesc.eUISort = FIFTH;
			m_vecUI.emplace_back(dynamic_cast<CUI_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		}
	}

	return S_OK;
}

HRESULT CUIGroup_Quick::Create_InvSlot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	for (size_t i = 0; i < 4; ++i)
	{
		for (size_t j = 0; j < 5; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = (g_iWinSizeX >> 1) + 140.f + (j * 76.f); // 160
			pDesc.fY = 200.f + (i * 76.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.eSlotType = CUI_Slot::SLOT_QUICK;
			pDesc.eUISort = TENTH;
			m_vecUI.emplace_back(dynamic_cast<CUI_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		}
	}

	return S_OK;
}

CUIGroup_Quick* CUIGroup_Quick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Quick* pInstance = new CUIGroup_Quick(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Quick");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Quick::Clone(void* pArg)
{
	CUIGroup_Quick* pInstance = new CUIGroup_Quick(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Quick");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Quick::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
