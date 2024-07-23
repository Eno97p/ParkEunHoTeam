#include "UIGroup_UpGPage.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "UI_UpGPageBG.h"
#include "UI_UpGPageTop.h"
#include "UI_UpGPage_NameBox.h"
#include "UI_UpGPage_Circle.h"
#include "UI_UpGPageBtn.h"
#include "UI_UpGPage_Slot.h"
#include "UI_UpGPage_MatSlot.h"
#include "UI_UpGPage_Value.h"

CUIGroup_UpGPage::CUIGroup_UpGPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_UpGPage::CUIGroup_UpGPage(const CUIGroup_UpGPage& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_UpGPage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_UpGPage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_UpGPage::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_UpGPage::Tick(_float fTimeDelta)
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

		for (auto& pSlot : m_vecSlot)
		{
			if (!m_isRenderOnAnim && !(pSlot->Get_RenderOnAnim()))
			{
				pSlot->Resset_Animation(true);
			}
			else if (m_isRenderOnAnim && pSlot->Get_RenderOnAnim())
			{
				pSlot->Resset_Animation(false);
			}

			pSlot->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_UpGPage::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);

		for (auto& pSlot : m_vecSlot)
			pSlot->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_UpGPage::Render()
{
	return S_OK;
}

HRESULT CUIGroup_UpGPage::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	// BG
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageBG"), &pDesc)));

	// Top
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageTop*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageTop"), &pDesc)));

	// NameBox 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_NameBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_NameBox"), &pDesc)));

	// Circle 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_Circle*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Circle"), &pDesc)));

	// Btn 
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPageBtn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPageBtn"), &pDesc)));


	// MatSlot  
	CUI_UpGPage_MatSlot::UI_MATSLOT_DESC pMatSlotDesc{};

	pMatSlotDesc.eLevel = LEVEL_STATIC;
	pMatSlotDesc.eMatSlotType = CUI_UpGPage_MatSlot::MATSLOT_L;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_MatSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_MatSlot"), &pMatSlotDesc)));

	pMatSlotDesc.eMatSlotType = CUI_UpGPage_MatSlot::MATSLOT_R;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_MatSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_MatSlot"), &pMatSlotDesc)));


	// Value
	CUI_UpGPage_Value::UI_VALUE_DESC pValueDesc{};

	pValueDesc.eLevel = LEVEL_STATIC;
	pValueDesc.eValueType = CUI_UpGPage_Value::VALUE_SOUL;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_Value*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Value"), &pValueDesc)));

	pValueDesc.eValueType = CUI_UpGPage_Value::VALUE_MATERIAL;
	m_vecUI.emplace_back(dynamic_cast<CUI_UpGPage_Value*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Value"), &pValueDesc)));


	if (FAILED(Create_Slot()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIGroup_UpGPage::Create_Slot()
{
	CUI::UI_DESC pDesc{};
	
	for (size_t i = 0; i < 4; ++i)
	{
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.fX = 250.f;
		pDesc.fY = 230.f + (i * 80.f);
		pDesc.fSizeX = 341.3f; // 512
		pDesc.fSizeY = 85.3f; // 128
		m_vecSlot.emplace_back(dynamic_cast<CUI_UpGPage_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_Slot"), &pDesc)));
	}

	// >> Inventory가 가지고 있는 Weapon 수에 맞춰 Render를 켜도록

	return S_OK;
}

CUIGroup_UpGPage* CUIGroup_UpGPage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_UpGPage* pInstance = new CUIGroup_UpGPage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_UpGPage");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_UpGPage::Clone(void* pArg)
{
	CUIGroup_UpGPage* pInstance = new CUIGroup_UpGPage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_UpGPage");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_UpGPage::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);

	for (auto& pSlot : m_vecSlot)
		Safe_Release(pSlot);
}
