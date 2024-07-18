#include "Juggulus_HandTwo.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"

CJuggulus_HandTwo::CJuggulus_HandTwo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CJuggulus_HandTwo::CJuggulus_HandTwo(const CJuggulus_HandTwo& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CJuggulus_HandTwo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJuggulus_HandTwo::Initialize(void* pArg)
{
	CPartObject::PARTOBJ_DESC* pDesc = (CPartObject::PARTOBJ_DESC*)pArg;
	m_pCurHp = pDesc->pCurHp;
	m_pMaxHp = pDesc->pMaxHp;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(1, true));

	m_pTransformCom->Scaling(2.f, 2.f, 2.f);
	m_vParentPos = XMVectorSet(pDesc->pParentMatrix->m[3][0], pDesc->pParentMatrix->m[3][1], pDesc->pParentMatrix->m[3][2], 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vParentPos);

	list<CGameObject*> PlayerList = m_pGameInstance->Get_GameObjects_Ref(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	m_pPlayer = dynamic_cast<CPlayer*>(PlayerList.front());
	Safe_AddRef(m_pPlayer);
	m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

void CJuggulus_HandTwo::Priority_Tick(_float fTimeDelta)
{
}

void CJuggulus_HandTwo::Tick(_float fTimeDelta)
{
	switch (m_eDisolveType)
	{
	case TYPE_INCREASE:
		m_fDisolveValue += fTimeDelta * 2.f;
		if (m_fDisolveValue > 1.f)
		{
			m_eDisolveType = TYPE_IDLE;
			m_fDisolveValue = 1.f;
		}
		break;
	case TYPE_DECREASE:
		m_fDisolveValue -= fTimeDelta * 2.f;
		if (m_fDisolveValue < 0.f)
		{
			m_eDisolveType = TYPE_INCREASE;
		}
		break;
	default:
		break;
	}

	Change_Animation(fTimeDelta);

	m_pBehaviorCom->Update(fTimeDelta);

}

void CJuggulus_HandTwo::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	}
}

HRESULT CJuggulus_HandTwo::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
			return E_FAIL;

		m_pShaderCom->Begin(7);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CJuggulus_HandTwo::Render_Bloom()
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

		m_pShaderCom->Begin(8);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CJuggulus_HandTwo::Render_LightDepth()
{
	return S_OK;
}

HRESULT CJuggulus_HandTwo::Add_Nodes()
{
	m_pBehaviorCom->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Composit_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_pBehaviorCom->Add_Action_Node(TEXT("Top_Selector"), TEXT("Idle"), bind(&CJuggulus_HandTwo::Idle, this, std::placeholders::_1));

	m_pBehaviorCom->Add_CoolDown(TEXT("Attack_Selector"), TEXT("ScoopCool"), 15.f);
	m_pBehaviorCom->Add_Action_Node(TEXT("ScoopCool"), TEXT("Scoop"), bind(&CJuggulus_HandTwo::Scoop, this, std::placeholders::_1));
	m_pBehaviorCom->Add_Action_Node(TEXT("Attack_Selector"), TEXT("Spawn"), bind(&CJuggulus_HandTwo::Spawn, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CJuggulus_HandTwo::Scoop(_float fTimeDelta)
{
 	if (m_eDisolveType == TYPE_IDLE && m_bScoop)
	{
		m_eDisolveType = TYPE_DECREASE;
	}

	if (m_eDisolveType == TYPE_INCREASE && m_bScoop)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos += XMVectorSet(-5.f, -20.f, -5.f, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_iState = STATE_SCOOP;
		m_bScoop = false;
		return RUNNING;
	}
 
	if (m_iState == STATE_SCOOP)
	{
		if (m_isAnimFinished)
		{
			m_eDisolveType = TYPE_DECREASE;
			return SUCCESS;
		}
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CJuggulus_HandTwo::Spawn(_float fTimeDelta)
{
	if (m_iState == STATE_SCOOP)
	{
		if (m_eDisolveType == TYPE_INCREASE)
		{
			m_iState = STATE_SPAWN;
		}
		return RUNNING;
	}

	if (m_iState == STATE_SPAWN)
	{
		if (m_isAnimFinished)
		{
			if (m_eDisolveType == TYPE_IDLE)
			{
				CLandObject::LANDOBJ_DESC		LandObjDesc{}; 
				LandObjDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
				LandObjDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));
				m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Homonculus"), &LandObjDesc);
				m_eDisolveType = TYPE_DECREASE;
				return RUNNING;
			}
			else
			{
				m_bScoop = true;
				m_iState = STATE_IDLE;
				return SUCCESS;
			}
		}
		return RUNNING;
	}
	else return FAILURE;
}

NodeStates CJuggulus_HandTwo::Idle(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vParentPos);
	m_iState = STATE_IDLE;
	return SUCCESS;
}

void CJuggulus_HandTwo::Add_Hp(_int iValue)
{
	*m_pCurHp = min(*m_pMaxHp, max(0, *m_pCurHp + iValue));
}

HRESULT CJuggulus_HandTwo::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_JuggulusHandTwo"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_pBehaviorCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDisolveTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CJuggulus_HandTwo::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pDisolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DisolveTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DisolveValue", &m_fDisolveValue, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CJuggulus_HandTwo::Change_Animation(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 9, true };
	_float fAnimSpeed = 1.f;

	if (m_iState == STATE_IDLE)
	{
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 1;
		fAnimSpeed = 1.f;
	}
	else if (m_iState == STATE_SCOOP)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		fAnimSpeed = 1.f;
	}
	else if (m_iState == STATE_SPAWN)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		fAnimSpeed = 1.f;
	}

	m_pModelCom->Set_AnimationIndex(AnimDesc);

	_bool isLerp = false; // false

	m_pModelCom->Play_Animation(fTimeDelta * fAnimSpeed, isLerp);

	m_isAnimFinished = m_pModelCom->Get_AnimFinished();
}

CJuggulus_HandTwo* CJuggulus_HandTwo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJuggulus_HandTwo* pInstance = new CJuggulus_HandTwo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CJuggulus_HandTwo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJuggulus_HandTwo::Clone(void* pArg)
{
	CJuggulus_HandTwo* pInstance = new CJuggulus_HandTwo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CJuggulus_HandTwo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJuggulus_HandTwo::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pBehaviorCom);
}
