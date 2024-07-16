#include "UI_LoadingBG.h"

#include "GameInstance.h"

CUI_LoadingBG::CUI_LoadingBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_LoadingBG::CUI_LoadingBG(const CUI_LoadingBG& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_LoadingBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_LoadingBG::Initialize(void* pArg)
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

void CUI_LoadingBG::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LoadingBG::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_LoadingBG::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SIXTEENTH);
}

HRESULT CUI_LoadingBG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LoadingBG::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingBG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingBG::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI_LoadingBG* CUI_LoadingBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingBG* pInstance = new CUI_LoadingBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LoadingBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LoadingBG::Clone(void* pArg)
{
	CUI_LoadingBG* pInstance = new CUI_LoadingBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LoadingBG");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingBG::Free()
{
	__super::Free();
}
