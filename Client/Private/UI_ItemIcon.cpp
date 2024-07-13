#include "UI_ItemIcon.h"

#include "GameInstance.h"

CUI_ItemIcon::CUI_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_ItemIcon::CUI_ItemIcon(const CUI_ItemIcon& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_ItemIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ItemIcon::Initialize(void* pArg)
{
	UI_ITEMICON_DESC* pDesc = static_cast<UI_ITEMICON_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;
	m_wszTexture = pDesc->wszTexture;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX; // 512
	m_fSizeY = pDesc->fSizeY; // 512

	Setting_Position();

	return S_OK;
}

void CUI_ItemIcon::Priority_Tick(_float fTimeDelta)
{
}

void CUI_ItemIcon::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_ItemIcon::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort); // SECOND
}

HRESULT CUI_ItemIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_ItemIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_wszTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_ItemIcon::Bind_ShaderResources()
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

CUI_ItemIcon* CUI_ItemIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_ItemIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_ItemIcon::Clone(void* pArg)
{
	CUI_ItemIcon* pInstance = new CUI_ItemIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_ItemIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_ItemIcon::Free()
{
	__super::Free();
}
