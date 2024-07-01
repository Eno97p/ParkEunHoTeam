#include "UI_CharacterBG.h"

#include "GameInstance.h"

CUI_CharacterBG::CUI_CharacterBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_CharacterBG::CUI_CharacterBG(const CUI_CharacterBG& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_CharacterBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CharacterBG::Initialize(void* pArg)
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

void CUI_CharacterBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_CharacterBG::Tick(_float fTimeDelta)
{
}

void CUI_CharacterBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, EIGHT);
}

HRESULT CUI_CharacterBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_CharacterBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharacterBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CharacterBG::Bind_ShaderResources()
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

CUI_CharacterBG* CUI_CharacterBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_CharacterBG* pInstance = new CUI_CharacterBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_CharacterBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_CharacterBG::Clone(void* pArg)
{
	CUI_CharacterBG* pInstance = new CUI_CharacterBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_CharacterBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CharacterBG::Free()
{
	__super::Free();
}
