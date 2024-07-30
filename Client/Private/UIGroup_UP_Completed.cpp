#include "UIGroup_UP_Completed.h"

#include "GameInstance.h"
#include "Inventory.h"

#include "UI.h"

CUIGroup_UP_Completed::CUIGroup_UP_Completed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIGroup{ pDevice, pContext }
{
}

CUIGroup_UP_Completed::CUIGroup_UP_Completed(const CUIGroup_UP_Completed& rhs)
    : CUIGroup{ rhs }
{
}

HRESULT CUIGroup_UP_Completed::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIGroup_UP_Completed::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_UI()))
        return E_FAIL;

    return S_OK;
}

void CUIGroup_UP_Completed::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_UP_Completed::Tick(_float fTimeDelta)
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

void CUIGroup_UP_Completed::Late_Tick(_float fTimeDelta)
{
	if (m_isRend)
	{
		for (auto& pUI : m_vecUI)
			pUI->Late_Tick(fTimeDelta);
	}
}

HRESULT CUIGroup_UP_Completed::Render()
{
	return S_OK;
}

HRESULT CUIGroup_UP_Completed::Create_UI()
{
	return S_OK;
}

CUIGroup_UP_Completed* CUIGroup_UP_Completed::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_UP_Completed* pInstance = new CUIGroup_UP_Completed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_UP_Completed");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_UP_Completed::Clone(void* pArg)
{
	CUIGroup_UP_Completed* pInstance = new CUIGroup_UP_Completed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_UP_Completed");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_UP_Completed::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
