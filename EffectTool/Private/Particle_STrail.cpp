
#include "Particle_STrail.h"
#include "GameInstance.h"

CSTrail::CSTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBlendObject(pDevice, pContext)
{
}

CSTrail::CSTrail(const CSTrail & rhs)
	: CBlendObject(rhs)
{
}

HRESULT CSTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSTrail::Initialize(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_pTrailDesc = make_shared<STRAIL_DESC>(*((STRAIL_DESC*)pArg));


	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CSTrail::Priority_Tick(_float fTimeDelta)
{
	
}

void CSTrail::Tick(_float fTimeDelta)
{
	//if (m_pVIBufferCom->Check_Instance_Dead())
	//	m_pGameInstance->Erase(this);

	//switch (m_pTrailDesc->eFuncType)
	//{
	//case TRAIL_EXTINCT:
	//	m_pVIBufferCom->ExtinctTrail(fTimeDelta);
	//	break;
	//case TRAIL_ETERNAL:
	//	m_pVIBufferCom->EternalTrail(fTimeDelta);
	//	break;
	//default:
	//	break;
	//}

	m_pVIBufferCom->Tick(fTimeDelta);

}
void CSTrail::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);

}

HRESULT CSTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}




HRESULT CSTrail::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Sword_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&(m_pTrailDesc->traildesc))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sword_Trail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_pTrailDesc->Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CSTrail::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}


CSTrail * CSTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSTrail*		pInstance = new CSTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSTrail::Clone(void * pArg)
{
	CSTrail*		pInstance = new CSTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
