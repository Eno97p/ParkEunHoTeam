#include "UI_LoadingCircle.h"

#include "GameInstance.h"

CUI_LoadingCircle::CUI_LoadingCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_LoadingCircle::CUI_LoadingCircle(const CUI_LoadingCircle& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_LoadingCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LoadingCircle::Initialize(void* pArg)
{
	UI_CIRCLE_DESC* pDesc = static_cast<UI_CIRCLE_DESC*>(pArg);

	m_eCircleType = pDesc->eCircleType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Position();

	return S_OK;
}

void CUI_LoadingCircle::Priority_Tick(_float fTimeDelta)
{
}

void CUI_LoadingCircle::Tick(_float fTimeDelta)
{
	/*if (!m_isRenderAnimFinished)
		Render_Animation(fTimeDelta);*/

	Turn_Animation(fTimeDelta);

}

void CUI_LoadingCircle::Late_Tick(_float fTimeDelta)
{
	CGameInstance::GetInstance()->Add_UI(this, SECOND);
}

HRESULT CUI_LoadingCircle::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_LoadingCircle::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingCircle"),  
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LoadingCircle::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_eCircleType)))
		return E_FAIL;

	_float4x4 rotationMatrix;
	XMStoreFloat4x4(&rotationMatrix, m_RotationMatrix);
	if (FAILED(m_pShaderCom->Bind_Matrix("g_RotationMatrix", &rotationMatrix)))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaTimer", &m_fRenderTimer, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bIsFadeIn", &m_isRenderOffAnim, sizeof(_bool))))
		return E_FAIL;*/

	return S_OK;
}

void CUI_LoadingCircle::Turn_Animation(_float fTimeDelta)
{
	//m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 1.f);

	if(CIRCLE_ONE == m_eCircleType)
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(5.f);
	else
		m_fRotationAngle += fTimeDelta * XMConvertToRadians(-5.f);

	if (m_fRotationAngle > XM_2PI)
		m_fRotationAngle -= XM_2PI;

	m_RotationMatrix = XMMatrixRotationZ(m_fRotationAngle); // XMMatrix
}

CUI_LoadingCircle* CUI_LoadingCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LoadingCircle::Clone(void* pArg)
{
	CUI_LoadingCircle* pInstance = new CUI_LoadingCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUI_LoadingCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LoadingCircle::Free()
{
	__super::Free();
}
