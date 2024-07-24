#include "UI_UpGPage_Slot.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Inventory.h"
#include "CMouse.h"
#include "ItemData.h"
#include "UI_ItemIcon.h"

#include "UI_UpGPage_SelectSlot.h"
#include "UI_UpGPage_ItemSlot.h"

CUI_UpGPage_Slot::CUI_UpGPage_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_UpGPage_Slot::CUI_UpGPage_Slot(const CUI_UpGPage_Slot& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_UpGPage_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_UpGPage_Slot::Initialize(void* pArg)
{
	UI_UPGSLOT_DESC* pDesc = static_cast<UI_UPGSLOT_DESC*>(pArg);

	m_iSlotIdx = pDesc->iSlotIdx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	Create_UI();

	return S_OK;
}

void CUI_UpGPage_Slot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_UpGPage_Slot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	__super::Tick(fTimeDelta);
	
	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	m_pSelectUI->Tick(fTimeDelta);
	m_pItemSlot->Tick(fTimeDelta);
}

void CUI_UpGPage_Slot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, EIGHT);

	if (m_isSelect)
		m_pSelectUI->Late_Tick(fTimeDelta);

	m_pItemSlot->Late_Tick(fTimeDelta);
}

HRESULT CUI_UpGPage_Slot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_UpGPage_Slot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpGPage_Slot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_UpGPage_Slot::Bind_ShaderResources()
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

HRESULT CUI_UpGPage_Slot::Create_UI()
{
	CUI::UI_DESC pDesc{};

	// Select UI
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 341.3f; // 512
	pDesc.fSizeY = 85.3f; // 128
	m_pSelectUI = dynamic_cast<CUI_UpGPage_SelectSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_SelectSlot"), &pDesc));

	// Item Slot 
	pDesc.fX = m_fX - 130.f;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 80.f; // 128
	pDesc.fSizeY = 80.f;
	m_pItemSlot = dynamic_cast<CUI_UpGPage_ItemSlot*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UIGroup_UpGPage_ItemSlot"), &pDesc));

	return S_OK;
}

void CUI_UpGPage_Slot::Create_ItemIcon()
{
	// 처음 시작할 때는 바로 Inventory의 weapon 받아와서 생성한 채로 시작하고 그 후에 추가되는 weapon들에 대한 정보는 함수로?
	// 그럼 함수를 Inventory weapon의 제일 마지막을 slot에 추가하는 것으로 할 것(어차피 순서 변하지 않으니까 상관 없을 듯함)

	CInventory::GetInstance()->Get_Weapons();


}

CUI_UpGPage_Slot* CUI_UpGPage_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_UpGPage_Slot* pInstance = new CUI_UpGPage_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_UpGPage_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_UpGPage_Slot::Clone(void* pArg)
{
	CUI_UpGPage_Slot* pInstance = new CUI_UpGPage_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_UpGPage_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_UpGPage_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pSelectUI);
	Safe_Release(m_pItemSlot);
}
