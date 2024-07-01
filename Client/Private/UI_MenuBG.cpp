#include "UI_MenuBG.h"

#include "GameInstance.h"

CUI_MenuBG::CUI_MenuBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_MenuBG::CUI_MenuBG(const CUI_MenuBG& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_MenuBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MenuBG::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	Setting_Position();

	return S_OK;
}

void CUI_MenuBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MenuBG::Tick(_float fTimeDelta)
{
}

void CUI_MenuBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, FOURTH);
}

HRESULT CUI_MenuBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MenuBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MenuBG::Bind_ShaderResources()
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

CUI_MenuBG* CUI_MenuBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MenuBG* pInstance = new CUI_MenuBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MenuBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MenuBG::Clone(void* pArg)
{
	CUI_MenuBG* pInstance = new CUI_MenuBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MenuBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MenuBG::Free()
{
	__super::Free();
}
