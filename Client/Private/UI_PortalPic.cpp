#include "UI_PortalPic.h"

#include "GameInstance.h"

CUI_PortalPic::CUI_PortalPic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CUI_PortalPic::CUI_PortalPic(const CUI_PortalPic& rhs)
    : CUI{ rhs }
{
}

HRESULT CUI_PortalPic::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_PortalPic::Initialize(void* pArg)
{
	UI_PORTALPIC_DESC* pDesc = static_cast<UI_PORTALPIC_DESC*>(pArg);

	m_iPicNum = pDesc->iPicNum;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pDesc->fAngle);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	//Setting_Pos();

	return S_OK;
}

void CUI_PortalPic::Priority_Tick(_float fTimeDelta)
{
}

void CUI_PortalPic::Tick(_float fTimeDelta)
{
}

void CUI_PortalPic::Late_Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_Scale(3.7f, 6.f, 4.5f);

	CGameInstance::GetInstance()->Add_UI(this, FIRST);
}

HRESULT CUI_PortalPic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0); // 0
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_PortalPic::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PortalPic"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	// Prototype_Component_Texture_UI_PortalPic_Dissolve

	return S_OK;
}

HRESULT CUI_PortalPic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iPicNum)))
		return E_FAIL;

	return S_OK;
}

void CUI_PortalPic::Setting_Pos()
{
	LEVEL eCurrentLevel = static_cast<LEVEL>(CGameInstance::GetInstance()->Get_CurrentLevel());
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	// Level에 따라 위치와 사진 변화

	switch (eCurrentLevel)
	{
	case Client::LEVEL_GAMEPLAY:
		m_iPicNum = 0;
		vPos = XMVectorSet(250.f, 523.f, 97.f, 1.f);
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 190.f);
		break;
	case Client::LEVEL_ACKBAR:
		m_iPicNum = 0;
		vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		break;
	case Client::LEVEL_JUGGLAS:
		m_iPicNum = 0;
		vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		break;
	case Client::LEVEL_GRASSLAND:
		m_iPicNum = 0;
		vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		break;
	default:
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

CUI_PortalPic* CUI_PortalPic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PortalPic* pInstance = new CUI_PortalPic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_PortalPic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PortalPic::Clone(void* pArg)
{
	CUI_PortalPic* pInstance = new CUI_PortalPic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_PortalPic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PortalPic::Free()
{
	__super::Free();
}
