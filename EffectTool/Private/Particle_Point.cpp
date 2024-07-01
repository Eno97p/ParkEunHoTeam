
#include "..\Public\Particle_Point.h"

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

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_IsColored = ((PARTICLEPOINT*)pArg)->isColored;


	if (FAILED(Add_Components(((PARTICLEPOINT*)pArg)->Texture)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((PARTICLEPOINT*)pArg)->vStartPos));
	
	return S_OK;
}

void CParticle_Point::Priority_Tick(_float fTimeDelta)
{
	
}

void CParticle_Point::Tick(_float fTimeDelta)
{
	if (m_pVIBufferCom->Check_Instance_Dead())
		m_pGameInstance->Erase(this);

	if (m_pTarget != nullptr)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTarget->Get_State(CTransform::STATE_POSITION));
	}

	switch (m_pParticleDesc->eType)
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
	}

}

void CParticle_Point::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CParticle_Point::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_IsColored == true)
		m_pShaderCom->Begin(2);
	else
		m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CParticle_Point::Add_Components(const wstring& Texcom)
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&(m_pParticleDesc->InstanceDesc))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevel(), Texcom,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CParticle_Point::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &m_pParticleDesc->vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &m_pParticleDesc->vEndColor, sizeof(_float3))))
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
}
