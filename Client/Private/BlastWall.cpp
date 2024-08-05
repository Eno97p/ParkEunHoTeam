#include "stdafx.h"

#include "BlastWall.h"


#include"GameInstance.h"

CBlastWall::CBlastWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlastWall::CBlastWall(const CBlastWall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlastWall::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CBlastWall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(75.f, 523.f, 98.f, 1.0f));


	return S_OK;
}

void CBlastWall::Priority_Tick(_float fTimeDelta)
{
}

void CBlastWall::Tick(_float fTimeDelta)
{
}

void CBlastWall::Late_Tick(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CBlastWall::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

	/*	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//	return E_FAIL;



		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
#pragma region 모션블러
	m_PrevWorldMatrix = *m_pTransformCom->Get_WorldFloat4x4();
	m_PrevViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
#pragma endregion 모션블러
	return S_OK;

}

HRESULT CBlastWall::Add_Components()
{

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fragile_Rock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	m_iNumMeshes = m_pModelCom->Get_NumMeshes();
	m_pPhysXCom = new CPhysXComponent* [m_iNumMeshes];
	ZeroMemory(m_pPhysXCom, sizeof(CPhysXComponent*) * m_iNumMeshes);
	for(size_t i = 0; i < m_iNumMeshes; i++)
	{
		wstring idxStr = to_wstring(i);

		CPhysXComponent::PHYSX_DESC		PhysXDesc;
		PhysXDesc.eGeometryType = PxGeometryType::eCONVEXMESH;
		PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
		PhysXDesc.pMesh = m_pModelCom->Get_Meshes()[i];
		XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());

		/* For.Com_Physx */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
			TEXT("Com_PhysX") + idxStr, reinterpret_cast<CComponent**>(&m_pPhysXCom[i]),&PhysXDesc)))
			return E_FAIL;

	}



	return S_OK;
}

HRESULT CBlastWall::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
#pragma region 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;
	_bool bMotionBlur = m_pGameInstance->Get_MotionBlur() || m_bMotionBlur;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MotionBlur", &bMotionBlur, sizeof(_bool))))
		return E_FAIL;
#pragma endregion 모션블러
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CBlastWall* CBlastWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBlastWall* pInstance = new CBlastWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBlastWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlastWall::Clone(void* pArg)
{
	CBlastWall* pInstance = new CBlastWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBlastWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlastWall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	for(size_t i = 0; i < m_iNumMeshes; i++)
		Safe_Release(m_pPhysXCom[i]);
	Safe_Delete_Array(m_pPhysXCom);
	//Safe_Release(m_pPhysXCom);
}
