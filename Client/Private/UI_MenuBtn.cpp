#include "UI_MenuBtn.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"

CUI_MenuBtn::CUI_MenuBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{pDevice, pContext}
{
}

CUI_MenuBtn::CUI_MenuBtn(const CUI_MenuBtn& rhs)
	: CUI_Interaction{rhs}
{
}

HRESULT CUI_MenuBtn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MenuBtn::Initialize(void* pArg)
{
	UI_MENUBTN_DESC* pDesc = static_cast<UI_MENUBTN_DESC*>(pArg);

	m_eMenuType = pDesc->eMenuType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 170.6f;
	m_fSizeY = 170.6f;

	Setting_Position();

	return S_OK;
}

void CUI_MenuBtn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_MenuBtn::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.15f), LONG(m_fY - m_fSizeY * 0.15f),
						LONG(m_fX + m_fSizeX * 0.15f) ,LONG(m_fY + m_fSizeY * 0.15f) };

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect)
	{
		m_iTextureNum = 1;
		if (m_pGameInstance->Mouse_Down(DIM_LB))
			Open_MenuPage();
	}
	else
		m_iTextureNum = 0;
}

void CUI_MenuBtn::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SEVENTH);
}

HRESULT CUI_MenuBtn::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_MenuBtn::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, Setting_Texture(),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MenuBtn::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum))) // 추후 마우스 충돌에 따라 texture 선택하여 출력
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

void CUI_MenuBtn::Setting_XY()
{
	switch (m_eMenuType)
	{
	case Client::CUI_MenuBtn::MENU_CH:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 130.f;
		break;
	case Client::CUI_MenuBtn::MENU_MAP:
		m_fX = (g_iWinSizeX >> 1) - 100.f;
		m_fY = 225.f;
		break;
	case Client::CUI_MenuBtn::MENU_WEAPON:
		m_fX = (g_iWinSizeX >> 1) + 100.f;
		m_fY = 225.f;
		break;
	case Client::CUI_MenuBtn::MENU_INV:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 320.f;
		break;
	case Client::CUI_MenuBtn::MENU_SET:
		m_fX = (g_iWinSizeX >> 1) - 100.f;
		m_fY = 415.f;
		break;
	case Client::CUI_MenuBtn::MENU_COD:
		m_fX = g_iWinSizeX >> 1;
		m_fY = 510.f;
		break;
	default:
		break;
	}
}

wstring CUI_MenuBtn::Setting_Texture()
{
	switch (m_eMenuType)
	{
	case Client::CUI_MenuBtn::MENU_CH:
		return TEXT("Prototype_Component_Texture_MenuBtn_Ch");
	case Client::CUI_MenuBtn::MENU_MAP:
		return TEXT("Prototype_Component_Texture_MenuBtn_Map");
	case Client::CUI_MenuBtn::MENU_WEAPON:
		return TEXT("Prototype_Component_Texture_MenuBtn_WP");
	case Client::CUI_MenuBtn::MENU_INV:
		return TEXT("Prototype_Component_Texture_MenuBtn_Inv");
	case Client::CUI_MenuBtn::MENU_SET:
		return TEXT("Prototype_Component_Texture_MenuBtn_Set");
	case Client::CUI_MenuBtn::MENU_COD:
		return TEXT("Prototype_Component_Texture_MenuBtn_Cod");
	default:
		return TEXT("");
	}
}

void CUI_MenuBtn::Open_MenuPage()
{
	switch (m_eMenuType)
	{
	case Client::CUI_MenuBtn::MENU_CH:
		CUI_Manager::GetInstance()->Set_MenuPage(true, "Menu_Ch");
		break;
	case Client::CUI_MenuBtn::MENU_MAP:
		break;
	case Client::CUI_MenuBtn::MENU_WEAPON:
		CUI_Manager::GetInstance()->Set_MenuPage(true, "Weapon");
		break;
	case Client::CUI_MenuBtn::MENU_INV:
		CUI_Manager::GetInstance()->Set_MenuPage(true, "Inventory");
		break;
	case Client::CUI_MenuBtn::MENU_SET:
		break;
	case Client::CUI_MenuBtn::MENU_COD:
		break;
	default:
		break;
	}

}

CUI_MenuBtn* CUI_MenuBtn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MenuBtn* pInstance = new CUI_MenuBtn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_MenuBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_MenuBtn::Clone(void* pArg)
{
	CUI_MenuBtn* pInstance = new CUI_MenuBtn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_MenuBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MenuBtn::Free()
{
	__super::Free();
}
