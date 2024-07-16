#include "Electronic.h"
#include "GameInstance.h"

CElectronic::CElectronic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CElectronic::CElectronic(const CElectronic& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CElectronic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CElectronic::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	
	OwnDesc = make_shared<ELECTRONICDESC>(*((ELECTRONICDESC*)pArg));

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&OwnDesc->vStartPos));
	return S_OK;
}

void CElectronic::Priority_Tick(_float fTimeDelta)
{
}

void CElectronic::Tick(_float fTimeDelta)
{
	fTime += fTimeDelta;
	if (m_pVIBufferCom->isDead())
		m_pGameInstance->Erase(this);

	m_pVIBufferCom->Tick(fTimeDelta);

}

void CElectronic::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CElectronic::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CElectronic::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Buffers();
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CElectronic::Render_Blur()
{
	return S_OK;
}

HRESULT CElectronic::Add_Components()
{
	/* VIBUFFER */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Lightning"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &(OwnDesc->BufferDesc))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Lightning"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, OwnDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PerlinNoise"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseTex))))
		return E_FAIL;

	return S_OK;
}

HRESULT CElectronic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	//	return E_FAIL;
	if (FAILED(m_pNoiseTex->Bind_ShaderResource(m_pShaderCom, "g_PerlinNoise", OwnDesc->iNumNoise)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &fTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->vColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CElectronic::Bind_BlurResources()
{
	return S_OK;
}

CElectronic* CElectronic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElectronic* pInstance = new CElectronic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CElectronic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElectronic::Clone(void* pArg)
{
	CElectronic* pInstance = new CElectronic(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CElectronic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CElectronic::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseTex);
	Safe_Release(m_pShaderCom);
}
