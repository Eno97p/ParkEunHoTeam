#include "UI_QTE_Ring.h"

#include "GameInstance.h"

CUI_QTE_Ring::CUI_QTE_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_QTE_Ring::CUI_QTE_Ring(const CUI_QTE_Ring& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_QTE_Ring::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_QTE_Ring::Initialize(void* pArg)
{
	UI_RING_DESC* pDesc = static_cast<UI_RING_DESC*>(pArg);

	m_eRingType = pDesc->eRingType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_QTE_Ring::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QTE_Ring::Tick(_float fTimeDelta)
{
	// 점점 작아짐
	m_fSizeX -= 10.f; // 10
	m_fSizeY -= 10.f;
	Setting_Position();

	// Good보다 크면 : Bad
	// Good ~ Perfect 사이면 Good
	// Perfect ~ End 사이면 Perfect
	// End보다 작으면 Bad > 이때는 애초에 사라질 것

	if (m_fSizeX >= fGOOD)
	{
		m_eRingState = RS_BAD;
	}
	else if (m_fSizeX >= fPERFECT)
	{
		m_eRingState = RS_GOOD;
	}
	else if (m_fSizeX >= fEND)
	{
		m_eRingState = RS_PERFECT;
	}
	else
	{
		m_eRingState = RS_BAD;
		m_isEnd = true;
	}
}

void CUI_QTE_Ring::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_QTE_Ring::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(8);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_QTE_Ring::Add_Components()
{
	/* For. Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Ring"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QTE_Ring::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eRingType)))
		return E_FAIL;

	return S_OK;
}

CUI_QTE_Ring* CUI_QTE_Ring::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QTE_Ring* pInstance = new CUI_QTE_Ring(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QTE_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QTE_Ring::Clone(void* pArg)
{
	CUI_QTE_Ring* pInstance = new CUI_QTE_Ring(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QTE_Ring");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QTE_Ring::Free()
{
	__super::Free();
}
