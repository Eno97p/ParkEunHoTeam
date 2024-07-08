#include "UIGroup_Logo.h"

#include "GameInstance.h"

#include "UI_LogoBG.h"

CUIGroup_Logo::CUIGroup_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_Logo::CUIGroup_Logo(const CUIGroup_Logo& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_Logo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_Logo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_Logo::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_Logo::Tick(_float fTimeDelta)
{
	// UI별로 다른 Fade InOut 필요 >> 우선 배치부터 할 것
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			pUI->Tick(fTimeDelta);
		}
	}
}

void CUIGroup_Logo::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
		{
			pUI->Late_Tick(fTimeDelta);
		}
	}
}

HRESULT CUIGroup_Logo::Render()
{
	return S_OK;
}

HRESULT CUIGroup_Logo::Create_UI()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	
	// LogoBG >>> 근데 dynamic_cast 필요 없는 듯
	m_vecUI.emplace_back(dynamic_cast<CUI_LogoBG*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_LogoBG"), &pDesc)));

	return S_OK;
}

CUIGroup_Logo* CUIGroup_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_Logo* pInstance = new CUIGroup_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_Logo");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_Logo::Clone(void* pArg)
{
	CUIGroup_Logo* pInstance = new CUIGroup_Logo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_Logo");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_Logo::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
