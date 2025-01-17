#include "stdafx.h"
#include "Particle_Point.h"
#include "GameInstance.h"

CParticle_Point::CParticle_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParticle(pDevice, pContext)
{
}

CParticle_Point::CParticle_Point(const CParticle_Point & rhs)
	: CParticle(rhs)
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Point::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	OwnDesc = new PARTICLEPOINT;
	*OwnDesc = *reinterpret_cast<PARTICLEPOINT*>(pArg);


	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&OwnDesc->SuperDesc.vStartPos));
	return S_OK;
}

void CParticle_Point::Priority_Tick(_float fTimeDelta)
{
	
}

void CParticle_Point::Tick(_float fTimeDelta)
{
	if (m_pVIBufferCom->Check_Instance_Dead() || m_Delete == true)
	{
		m_pGameInstance->Erase(this);
		return;
	}

	if (m_pTarget != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_State(CTransform::STATE_POSITION));
	}

	switch (OwnDesc->SuperDesc.eType)
	{
	case SPREAD:
		m_pVIBufferCom->Spread(fTimeDelta);
		break;
	case DROP:
		m_pVIBufferCom->Drop(fTimeDelta);
		break;
	case GROWOUT:
		m_pVIBufferCom->GrowOut(fTimeDelta);
		break;
	case SPREAD_SIZEUP:
		m_pVIBufferCom->Spread_Size_Up(fTimeDelta);
		break;
	case SPREAD_NONROTATION:
		m_pVIBufferCom->Spread_Non_Rotation(fTimeDelta);
		break;
	case TORNADO:
		m_pVIBufferCom->CreateSwirlEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN:
		m_pVIBufferCom->Spread_Speed_Down(fTimeDelta);
		break;
	case SLASH_EFFECT:
		m_pVIBufferCom->SlashEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN_SIZE_UP:
		m_pVIBufferCom->Spread_Speed_Down_SizeUp(fTimeDelta);
		break;
	case GATHER:
		m_pVIBufferCom->Gather(fTimeDelta);
		break;
	case EXTINCTION:
		m_pVIBufferCom->Extinction(fTimeDelta);
		break;
	case GROWOUTY:
		m_pVIBufferCom->GrowOutY(fTimeDelta);
		break;
	case GROWOUT_SPEEDDOWN:
		m_pVIBufferCom->GrowOut_Speed_Down_Texture(fTimeDelta);
		break;
	case SPIRAL_EXTINCTION:
		m_pVIBufferCom->Spiral_Extinction(fTimeDelta);
		break;
	case SPIRAL_SPERAD:
		m_pVIBufferCom->Spiral_Expansion(fTimeDelta);
		break;
	case LENZ_FLARE:
		m_pVIBufferCom->Lenz_Flare(fTimeDelta);
		break;
	case LEAF_FALL:
		m_pVIBufferCom->Leaf_Fall(fTimeDelta);
		break;
	case BLOW:
		m_pVIBufferCom->Blow(fTimeDelta);
		break;
	case UP_TO_STOP:
		m_pVIBufferCom->Up_To_Stop(fTimeDelta);
		break;
	case ONLY_UP:
		m_pVIBufferCom->Only_Up(fTimeDelta);
		break;
	}

}

void CParticle_Point::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (OwnDesc->SuperDesc.IsBlur)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLUR, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);

	if (OwnDesc->SuperDesc.IsBloom)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CParticle_Point::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Point::Render_Bloom()
{
	if (FAILED(Bind_BlurResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Point::Render_Blur()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Point::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&(OwnDesc->SuperDesc.InstanceDesc))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, OwnDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CParticle_Point::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->SuperDesc.Desolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->SuperDesc.DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->SuperDesc.vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &OwnDesc->SuperDesc.IsColor, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &OwnDesc->SuperDesc.vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &OwnDesc->SuperDesc.vEndColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->SuperDesc.IsAlpha, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->SuperDesc.fDesolveLength, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
		return E_FAIL;
	return S_OK;
}

HRESULT CParticle_Point::Bind_BlurResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Desolve", &OwnDesc->SuperDesc.Desolve, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pDesolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", OwnDesc->SuperDesc.DesolveNum)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolveColor", &OwnDesc->SuperDesc.vDesolveColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BlurPower", &OwnDesc->SuperDesc.fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_BloomColor", &OwnDesc->SuperDesc.vBloomColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &OwnDesc->SuperDesc.IsAlpha, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DesolvePower", &OwnDesc->SuperDesc.fDesolveLength, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CParticle_Point * CParticle_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticle_Point*		pInstance = new CParticle_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Point::Clone(void * pArg)
{
	CParticle_Point*		pInstance = new CParticle_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Point::Free()
{
	__super::Free();
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Delete(OwnDesc);
}
