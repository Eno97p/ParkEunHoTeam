#include "UI_WPEquipSlot.h"

#include "GameInstance.h"
#include "CMouse.h"
#include "UI_WPEquipNone.h"

CUI_WPEquipSlot::CUI_WPEquipSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_WPEquipSlot::CUI_WPEquipSlot(const CUI_WPEquipSlot& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_WPEquipSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_WPEquipSlot::Initialize(void* pArg)
{
	UI_EQUIPSLOT_DESC* pDesc = static_cast<UI_EQUIPSLOT_DESC*>(pArg);

	m_eSlotNum = pDesc->eSlotNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_XY();
	m_fSizeX = 170.6f; // 256
	m_fSizeY = 170.6f;

	Setting_Position();
	Create_Frame();

	return S_OK;
}

void CUI_WPEquipSlot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_WPEquipSlot::Tick(_float fTimeDelta)
{
	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.1f), LONG(m_fY - m_fSizeY * 0.3f),
			LONG(m_fX + m_fSizeX * 0.1f) ,LONG(m_fY + m_fSizeY * 0.3f) };

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect)
	{
		m_fY = SELECT_Y;
		m_pNoneFrame->Set_PosY(SELECT_Y);
	}
	else
	{
		m_fY = DEFAULT_Y;
		m_pNoneFrame->Set_PosY(DEFAULT_Y);
	}

	Setting_Position();

	if (nullptr != m_pNoneFrame)
		m_pNoneFrame->Tick(fTimeDelta);
}

void CUI_WPEquipSlot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, ELEVENTH);

	if (nullptr != m_pNoneFrame && !m_isSelect)
		m_pNoneFrame->Late_Tick(fTimeDelta);
}

HRESULT CUI_WPEquipSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WeaponEquipSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_WPEquipSlot::Bind_ShaderResources()
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

HRESULT CUI_WPEquipSlot::Create_Frame()
{
	CUI::UI_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 170.6f; // 256
	pDesc.fSizeY = 170.6f;
	m_pNoneFrame = dynamic_cast<CUI_WPEquipNone*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_WPEquipNone"), &pDesc));

	return S_OK;
}

void CUI_WPEquipSlot::Setting_XY()
{
	switch (m_eSlotNum)
	{
	case Client::CUI_WPEquipSlot::NUM_ONE:
		m_fX = 270.f;
		break;
	case Client::CUI_WPEquipSlot::NUM_TWO:
		m_fX = 380.f;
		break;
	case Client::CUI_WPEquipSlot::NUM_THREE:
		m_fX = 490.f;

		break;
	default:
		break;
	}

	m_fY = DEFAULT_Y;
}

CUI_WPEquipSlot* CUI_WPEquipSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_WPEquipSlot* pInstance = new CUI_WPEquipSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_WPEquipSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_WPEquipSlot::Clone(void* pArg)
{
	CUI_WPEquipSlot* pInstance = new CUI_WPEquipSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_WPEquipSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_WPEquipSlot::Free()
{
	__super::Free();
	Safe_Release(m_pNoneFrame);
}
