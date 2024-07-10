#include "UIGroup_BossHP.h"

#include "GameInstance.h"
#include "UI.h"

CUIGroup_BossHP::CUIGroup_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_BossHP::CUIGroup_BossHP(const CUIGroup_BossHP& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_BossHP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_BossHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_UI()))
		return E_FAIL;

	return S_OK;
}

void CUIGroup_BossHP::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_BossHP::Tick(_float fTimeDelta)
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

void CUIGroup_BossHP::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_BossHP::Render()
{
	return S_OK;
}

HRESULT CUIGroup_BossHP::Create_UI()
{
	// HP

	// Bar

	return S_OK;
}

CUIGroup_BossHP* CUIGroup_BossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_BossHP* pInstance = new CUIGroup_BossHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_BossHP");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_BossHP::Clone(void* pArg)
{
	CUIGroup_BossHP* pInstance = new CUIGroup_BossHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_BossHP");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_BossHP::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
