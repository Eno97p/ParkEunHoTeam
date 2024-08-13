#include "UI_QTE_Btn.h"

#include "GameInstance.h"

#include "UI_QTE_Ring.h"

CUI_QTE_Btn::CUI_QTE_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_QTE_Btn::CUI_QTE_Btn(const CUI_QTE_Btn& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_QTE_Btn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_QTE_Btn::Initialize(void* pArg)
{
	UI_QTE_BTN_DESC* pDesc = static_cast<UI_QTE_BTN_DESC*>(pArg);

	m_iBtnNum = pDesc->iBtnIndex;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	if (FAILED(Create_Ring()))
		return E_FAIL;

	return S_OK;
}

void CUI_QTE_Btn::Priority_Tick(_float fTimeDelta)
{
}

void CUI_QTE_Btn::Tick(_float fTimeDelta)
{
	if (nullptr != m_pRing)
		m_pRing->Tick(fTimeDelta);
}

void CUI_QTE_Btn::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);

	if (nullptr != m_pRing)
		m_pRing->Late_Tick(fTimeDelta);
}

HRESULT CUI_QTE_Btn::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_QTE_Btn::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QTE_Btn"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QTE_Btn::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iBtnNum)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_QTE_Btn::Create_Ring()
{
	// 
	CUI_QTE_Ring::UI_RING_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.eRingType = static_cast<CUI_QTE_Ring::RING_TYPE>(m_iBtnNum);
	pDesc.fX = m_fX;
	pDesc.fY = m_fY;
	pDesc.fSizeX = 512.f; // 512;
	pDesc.fSizeY = 512.f;

	m_pRing = dynamic_cast<CUI_QTE_Ring*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QTE_Ring"), &pDesc));

	return S_OK;
}

CUI_QTE_Btn* CUI_QTE_Btn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_QTE_Btn* pInstance = new CUI_QTE_Btn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_QTE_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_QTE_Btn::Clone(void* pArg)
{
	CUI_QTE_Btn* pInstance = new CUI_QTE_Btn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_QTE_Btn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_QTE_Btn::Free()
{
	__super::Free();

	Safe_Release(m_pRing);
}
