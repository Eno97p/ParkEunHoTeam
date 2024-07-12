#include "UI_Slot.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "CMouse.h"
#include "UI_Slot_Frame.h"
#include "UIGroup.h"
#include "UI_ItemIcon.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Interaction{ pDevice, pContext }
{
}

CUI_Slot::CUI_Slot(const CUI_Slot& rhs)
	: CUI_Interaction{ rhs }
{
}

HRESULT CUI_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
	UI_SLOT_DESC* pDesc = static_cast<UI_SLOT_DESC*>(pArg);

	m_eUISort = pDesc->eUISort;
	m_eSlotType = pDesc->eSlotType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Create_Frame();

	Setting_Position();

	return S_OK;
}

void CUI_Slot::Priority_Tick(_float fTimeDelta)
{
}

void CUI_Slot::Tick(_float fTimeDelta)
{
	if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);

	__super::Tick(fTimeDelta);

	m_isSelect = IsCollisionRect(m_pMouse->Get_CollisionRect());

	if (m_isSelect && m_pGameInstance->Mouse_Down(DIM_LB))
	{
		Open_SubPage();
	}

	if (nullptr != m_pSelectFrame)
		m_pSelectFrame->Tick(fTimeDelta);
}

void CUI_Slot::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, m_eUISort);

	if (nullptr != m_pSelectFrame && m_isSelect)
		m_pSelectFrame->Late_Tick(fTimeDelta);
}

HRESULT CUI_Slot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Slot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot::Bind_ShaderResources()
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

HRESULT CUI_Slot::Create_Frame()
{
	CUI_Slot_Frame::UI_SLOTFRAME_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 85.3f;
	pDesc.fSizeY = 85.3f;
	
	if (SLOT_QUICK == m_eSlotType)
		pDesc.eUISort = ELEVENTH;
	else if (SLOT_INV == m_eSlotType || SLOT_WEAPON == m_eSlotType)
		pDesc.eUISort = TENTH;
	else if (SLOT_INVSUB == m_eSlotType)
		pDesc.eUISort = FOURTEENTH;

	m_pSelectFrame = dynamic_cast<CUI_Slot_Frame*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Slot_Frame"), &pDesc));
	if (nullptr == m_pSelectFrame)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Slot::Create_ItemIcon()
{
	CUI_ItemIcon::UI_ITEMICON_DESC pDesc{};

	pDesc.eLevel = LEVEL_STATIC;
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 64.f;
	pDesc.fSizeY = 64.f;
	//pDesc.wszTexture = wstrTextureName; // Inventory로부터 받아와서 적용


	return S_OK;
}

void CUI_Slot::Open_SubPage()
{
	if (SLOT_INV == m_eSlotType) // 인벤토리에 있는 슬롯을 클릭한 경우
	{
		// 사용 가능한 아이템이라는 조건문 추가 필요 (나중에)

		CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_AnimFinished(false);
		CUI_Manager::GetInstance()->Render_UIGroup(true, "InvSub");
		CUI_Manager::GetInstance()->Get_UIGroup("InvSub")->Set_RenderOnAnim(true);
	}
}

CUI_Slot* CUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Slot* pInstance = new CUI_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Slot::Clone(void* pArg)
{
	CUI_Slot* pInstance = new CUI_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pItemIcon);
	Safe_Release(m_pSelectFrame);
}
