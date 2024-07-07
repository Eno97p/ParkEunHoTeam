#include "UI_FadeInOut.h"

#include "GameInstance.h"

CUI_FadeInOut::CUI_FadeInOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{pDevice, pContext}
{
}

CUI_FadeInOut::CUI_FadeInOut(const CUI_FadeInOut& rhs)
	: CUI{rhs}
{
}

HRESULT CUI_FadeInOut::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_FadeInOut::Initialize(void* pArg)
{
	UI_FADEINOUT_DESC* pDesc = static_cast<UI_FADEINOUT_DESC*>(pArg);

	m_isFadeIn = pDesc->isFadeIn;
	m_eFadeType = pDesc->eFadeType;

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

void CUI_FadeInOut::Priority_Tick(_float fTimeDelta)
{
}

void CUI_FadeInOut::Tick(_float fTimeDelta)
{
	m_fAlphaTimer += fTimeDelta* 0.4f;
	m_fDisolveValue += fTimeDelta * 0.4f;

	if (m_fDisolveValue >= 1.f)
	{
		m_fDisolveValue = 1.f;
		m_pGameInstance->Erase(this);
	}

	//if (m_fAlphaTimer >= 1.f)
	//{
	//	m_fAlphaTimer = 1.f;

	//	if (!m_isFadeIn) // Fade Out
	//	{
	//		// 씬 초기화 필요

	//	}
	//	m_pGameInstance->Erase(this);
	//}
}

void CUI_FadeInOut::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, TWELFTH); // 더 추가될 수 있음
}

HRESULT CUI_FadeInOut::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (TYPE_ALPHA == m_eFadeType)
	{
		m_pShaderCom->Begin(3);
	}
	else if (TYPE_DISSOLVE == m_eFadeType)
	{
		m_pShaderCom->Begin(4);
	}

	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_FadeInOut::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUITex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FadeInOut"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (TYPE_DISSOLVE == m_eFadeType)
	{
		/* For.Com_DissolveTexture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
			TEXT("Com_DissolveTexture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_FadeInOut::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 29)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fAlphaTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isFadeIn, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_FadeInOut* CUI_FadeInOut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_FadeInOut* pInstance = new CUI_FadeInOut(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_FadeInOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_FadeInOut::Clone(void* pArg)
{
	CUI_FadeInOut* pInstance = new CUI_FadeInOut(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_FadeInOut");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_FadeInOut::Free()
{
	__super::Free();

	Safe_Release(m_pDisolveTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
