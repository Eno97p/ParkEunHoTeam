#include "WhisperSword_Anim.h"
#include "GameInstance.h"


CWhisperSword_Anim::CWhisperSword_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWhisperSword_Anim::CWhisperSword_Anim(const CWhisperSword_Anim& rhs)
	: CWeapon{ rhs }
{
}

HRESULT CWhisperSword_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWhisperSword_Anim::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(0.5f, 0.5f, 0.5f);
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(90.f));

	//CParticle_Trail::TRAIL_DESC traild{};
	//
	//traild.traildesc.ParentMat = &m_WorldMatrix;
	//traild.traildesc.vPivotPos = _float3(0.f, 0.7f, 0.f);
	//traild.traildesc.fLifeTime = 3.f;
	//traild.traildesc.iNumInstance = 100;
	//traild.traildesc.IsLoop = true;
	//traild.traildesc.vSize = _float3(1.f, 1.f, 1.f);
	//traild.traildesc.vSpeed = 30.f;
	//traild.vStartColor = _float3(1.f, 0.f, 0.f);
	//traild.vEndColor = _float3(1.f, 1.f, 0.f);
	//
	//
	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail"), &traild);



	return S_OK;
}

void CWhisperSword_Anim::Priority_Tick(_float fTimeDelta)
{
}

void CWhisperSword_Anim::Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 5.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 5.f;
		if (m_fDisolveValue < 0.f)
		{
			m_eDisolveType = TYPE_INCREASE;
		}
		break;
	default:
		break;
	}

	CModel::ANIMATION_DESC		AnimDesc{ 0, true};

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	m_pModelCom->Play_Animation(fTimeDelta * 3.f);

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

#ifdef _DEBUG
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
#endif
	m_PhysXCom->Tick(&m_WorldMatrix);
}

void CWhisperSword_Anim::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

	//m_PhysXCom->Late_Tick(&m_WorldMatrix);
#ifdef _DEBUG
	if (m_bIsActive)
	{
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	}
	m_pGameInstance->Add_DebugComponent(m_PhysXCom);
#endif


}

HRESULT CWhisperSword_Anim::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CWhisperSword_Anim::Render_LightDepth()
{

	return S_OK;
}

HRESULT CWhisperSword_Anim::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(6);

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CWhisperSword_Anim::Add_Components()
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	ColliderDesc.eType = CCollider::TYPE_OBB;
	ColliderDesc.vExtents = _float3(0.5f, 1.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WhisperSword_Anim"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//Prototype_Component_Physx

	CPhysXComponent::PHYSX_DESC PhysXDesc{};
	PhysXDesc.eGeometryType = PxGeometryType::eCAPSULE;
	PhysXDesc.fMatterial= _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.fWorldMatrix = m_WorldMatrix;
	PhysXDesc.fCapsuleProperty= _float2(0.1f, 0.5f);
	PhysXDesc.pName= "Weapon";
	PhysXDesc.filterData.word0 =  Engine::CollisionGropuID::GROUP_WEAPON;
	//XMStoreFloat4x4(&PhysXDesc.fOffsetMatrix, XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixTranslation(10.f,0.f,10.f));
	//PhysXDesc.filterData.word1 =  Engine::CollisionGropuID::GROUP_ENVIRONMENT | Engine::CollisionGropuID::GROUP_ENEMY;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_PhysXCom),&PhysXDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWhisperSword_Anim::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CWhisperSword_Anim* CWhisperSword_Anim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWhisperSword_Anim* pInstance = new CWhisperSword_Anim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CWhisperSword_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWhisperSword_Anim::Clone(void* pArg)
{
	CWhisperSword_Anim* pInstance = new CWhisperSword_Anim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CWhisperSword_Anim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWhisperSword_Anim::Free()
{
	__super::Free();

	Safe_Release(m_PhysXCom);
}
