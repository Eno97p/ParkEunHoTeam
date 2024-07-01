#include "UI_WPFontaine.h"

#include "GameInstance.h"

CUI_WPFontaine::CUI_WPFontaine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_WPFontaine::CUI_WPFontaine(const CUI_WPFontaine& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_WPFontaine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WPFontaine::Initialize(void* pArg)
{
	UI_FONTAINE_DESC* pDesc = static_cast<UI_FONTAINE_DESC*>(pArg);

	m_iSlotNum = pDesc->iSlotNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 270.f + (m_iSlotNum * 110.f);
	m_fY = g_iWinSizeY - 115.f;
	m_fSizeX = 64.f; // 256
	m_fSizeY = 512.f;

	Setting_Position();

	return S_OK;
}

void CUI_WPFontaine::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WPFontaine::Tick(_float fTimeDelta)
{
}

void CUI_WPFontaine::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TENTH);
}

HRESULT CUI_WPFontaine::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WPFontaine::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WPFontaine"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WPFontaine::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iSlotNum)))
		return E_FAIL;

	return S_OK;
}

CUI_WPFontaine* CUI_WPFontaine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WPFontaine* pInstance = new CUI_WPFontaine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WPFontaine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WPFontaine::Clone(void* pArg)
{
	CUI_WPFontaine* pInstance = new CUI_WPFontaine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WPFontaine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WPFontaine::Free()
{
	__super::Free();
}
