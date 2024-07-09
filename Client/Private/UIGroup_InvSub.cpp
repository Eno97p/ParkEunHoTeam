#include "UIGroup_InvSub.h"

#include "GameInstance.h"
#include "UI_MenuPage_BGAlpha.h"
#include "UI_Slot.h"
#include "UI_InvSub_Btn.h"

CUIGroup_InvSub::CUIGroup_InvSub(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_InvSub::CUIGroup_InvSub(const CUIGroup_InvSub& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_InvSub::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_InvSub::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_InvSub::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_InvSub::Tick(_float fTimeDelta)
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
}

void CUIGroup_InvSub::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_InvSub::Render()
{
	return S_OK;
}

HRESULT CUIGroup_InvSub::Create_UI()
{
	// BG
	CUI_MenuPage_BGAlpha::UI_MP_BGALPHA_DESC pBGDesc{};

	pBGDesc.eLevel = LEVEL_STATIC;
	pBGDesc.eUISort = TWELFTH;
	m_vecUI.emplace_back(dynamic_cast<CUI*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_MenuPage_BGAlpha"), &pBGDesc)));

	Create_Slot();
	Create_Btn();

	return S_OK;
}

HRESULT CUIGroup_InvSub::Create_Slot()
{
	CUI_Slot::UI_SLOT_DESC pDesc{};

	for (size_t i = 0; i < 2; ++i)
	{
		for (size_t j = 0; j < 6; ++j)
		{
			ZeroMemory(&pDesc, sizeof(pDesc));
			pDesc.eLevel = LEVEL_STATIC;
			pDesc.fX = (g_iWinSizeX >> 1) - 230.f + (j * 81.f); // 160
			pDesc.fY = (g_iWinSizeY >> 1) + 100.f + (i * 81.f);
			pDesc.fSizeX = 85.3f;
			pDesc.fSizeY = 85.3f;
			pDesc.eSlotType = CUI_Slot::SLOT_INVSUB;
			pDesc.eUISort = THIRTEENTH;
			m_vecUI.emplace_back(dynamic_cast<CUI_Slot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot"), &pDesc)));
		}
	}

	return S_OK;
}

HRESULT CUIGroup_InvSub::Create_Btn()
{
	CUI_InvSub_Btn::BTN_TYPE arrBtnType[CUI_InvSub_Btn::BTN_END] =
	{
		CUI_InvSub_Btn::BTN_SET, CUI_InvSub_Btn::BTN_USE, CUI_InvSub_Btn::BTN_CANCEL
	};

	CUI_InvSub_Btn::UI_BTN_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;

	for (size_t i = 0; i < 3; ++i)
	{
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.eLevel = LEVEL_STATIC;
		pDesc.fX = (g_iWinSizeX >> 1) - 30.f;
		pDesc.fY = (g_iWinSizeY >> 1) - 130.f + (i * 60.f);
		pDesc.fSizeX = 360.f; // 512
		pDesc.fSizeY = 50.f; // 128
		pDesc.eBtnType = arrBtnType[i];
		m_vecUI.emplace_back(dynamic_cast<CUI_InvSub_Btn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_InvSub_Btn"), &pDesc)));
	}

	return S_OK;
}

CUIGroup_InvSub* CUIGroup_InvSub::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_InvSub* pInstance = new CUIGroup_InvSub(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_InvSub");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_InvSub::Clone(void* pArg)
{
	CUIGroup_InvSub* pInstance = new CUIGroup_InvSub(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_InvSub");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_InvSub::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
