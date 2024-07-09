#include "UI_WeaponTab.h"

#include "GameInstance.h"

CUI_WeaponTab::CUI_WeaponTab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_WeaponTab::CUI_WeaponTab(const CUI_WeaponTab& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_WeaponTab::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WeaponTab::Initialize(void* pArg)
{
	UI_TAB_DESC* pDesc = static_cast<UI_TAB_DESC*>(pArg);

	m_eTabType = pDesc->eTabType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Data();
	Setting_Position();

	return S_OK;
}

void CUI_WeaponTab::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WeaponTab::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);
}

void CUI_WeaponTab::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, NINETH);
}

HRESULT CUI_WeaponTab::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WeaponTab::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponTab"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WeaponTab::Bind_ShaderResources()
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

void CUI_WeaponTab::Setting_Data()
{
	switch (m_eTabType)
	{
	case Client::CUI_WeaponTab::TAB_L:
	{
		m_fX = (g_iWinSizeX >> 1) - 35.f;
		m_fSizeX = SELECT_SIZE; // 128
		m_fSizeY = SELECT_SIZE;
		break;
	}
	case Client::CUI_WeaponTab::TAB_R:
	{
		m_fX = (g_iWinSizeX >> 1) + 35.f;
		m_fSizeX = NONE_SIZE; // 128
		m_fSizeY = NONE_SIZE;
		break;
	}
	default:
		break;
	}

	m_fY = 80.f;
}

CUI_WeaponTab* CUI_WeaponTab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WeaponTab* pInstance = new CUI_WeaponTab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WeaponTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WeaponTab::Clone(void* pArg)
{
	CUI_WeaponTab* pInstance = new CUI_WeaponTab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WeaponTab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WeaponTab::Free()
{
	__super::Free();
}
