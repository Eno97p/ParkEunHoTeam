#include "UI_StateHP.h"

#include "GameInstance.h"

CUI_StateHP::CUI_StateHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{pDevice, pContext}
{
}

CUI_StateHP::CUI_StateHP(const CUI_StateHP& rhs)
    : CUI{rhs}
{
}

HRESULT CUI_StateHP::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_StateHP::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 390.f;
	m_fY = 45.f;
	m_fSizeX = 768.f;
	m_fSizeY = 24.f;

	Setting_Position();

	return S_OK;
}

void CUI_StateHP::Priority_Tick(_float fTimeDelta)
{
}

void CUI_StateHP::Tick(_float fTimeDelta)
{
}

void CUI_StateHP::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, THIRD);
}

HRESULT CUI_StateHP::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_StateHP::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HUD_StateHP"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_StateHP::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}

CUI_StateHP* CUI_StateHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_StateHP* pInstance = new CUI_StateHP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_StateHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_StateHP::Clone(void* pArg)
{
	CUI_StateHP* pInstance = new CUI_StateHP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_StateHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_StateHP::Free()
{
	__super::Free();
}
