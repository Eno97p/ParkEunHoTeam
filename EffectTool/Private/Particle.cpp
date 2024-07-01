#include "Particle.h"
#include "GameInstance.h"
CParticle::CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
	, m_pParticleDesc(nullptr)
{
}

CParticle::CParticle(const CParticle& rhs)
	:CBlendObject(rhs)
{
	m_pParticleDesc = new PARTICLEDESC;
}

HRESULT CParticle::Initialize(void* pArg)
{
	__super::Initialize(nullptr);

	*m_pParticleDesc = *((PARTICLEDESC*)pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pParticleDesc->vStartPos));

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDesolveTexture))))
		return E_FAIL;
	//a
	return S_OK;
}

void CParticle::Set_Target(CGameObject* Target)
{
	m_pTarget = (CTransform*)Target->Get_Component(TEXT("Com_Transform"));
}

void CParticle::Free()
{
	__super::Free();
	Safe_Release(m_pDesolveTexture);
	Safe_Release(m_pShaderCom);
	Safe_Delete(m_pParticleDesc);
}
