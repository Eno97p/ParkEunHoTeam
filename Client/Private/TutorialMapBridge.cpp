#include "TutorialMapBridge.h"

#include "GameInstance.h"


CTutorialMapBridge::CTutorialMapBridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActive_Element(pDevice, pContext)
{
}

CTutorialMapBridge::CTutorialMapBridge(const CTutorialMapBridge& rhs)
	: CActive_Element(rhs)
{
}

HRESULT CTutorialMapBridge::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorialMapBridge::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTutorialMapBridge::Priority_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_5))
	{
		m_eDSState = DS_FADEOUT;
		m_bPhysxOff = true;
	}

	if (m_pGameInstance->Key_Down(DIK_6))
	{
		m_eDSState = DS_FADEIN;
		m_bPhysxOff = false;

	}
}

void CTutorialMapBridge::Tick(_float fTimeDelta)
{
	if (m_eDSState == DS_FADEOUT)
	{
		if (m_fAccTime < 1.f)
		{
			m_fAccTime += fTimeDelta;
		}
		else
		{
			m_fAccTime = 1.f;
			m_eDSState = DS_END;
		}

	}
	else if(m_eDSState == DS_FADEIN)
	{

		if (m_fAccTime > 0.f)
		{
			m_fAccTime -= fTimeDelta;
		}
		else
		{
			m_fAccTime = 0.f;
			m_eDSState = DS_END;
		}

	}

	if(!m_bPhysxOff)
		m_pPhysXCom->Tick(m_pTransformCom->Get_WorldFloat4x4());
}

void CTutorialMapBridge::Late_Tick(_float fTimeDelta)
{
	if (m_bPhysxOff)
	{
		m_pPhysXCom->Late_Tick(m_pTransformCom->Get_WorldFloat4x4Ref());
	}	

#ifdef _DEBUG

	m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTutorialMapBridge::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if ( i != 29)
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;


		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &i, sizeof(_uint))))
		//	return;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTutorialMapBridge::Add_Components(void* pArg)
{


	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TutorialMapBridge"),
	//	TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseCom))))
		return E_FAIL;

	CPhysXComponent::PHYSX_DESC		PhysXDesc{};
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.fBoxProperty = _float3(10.f, 1.f, 10.f);				//박스 크기
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&PhysXDesc.fOffsetMatrix,  XMMatrixRotationX(XMConvertToRadians(90.0f))* XMMatrixTranslation(20.f, 10.f, 0.f));  //오프셋 위치
	

	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.eGeometryType = PxGeometryType::eBOX;
	PhysXDesc.pName = "Bridge";
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTutorialMapBridge::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTutorialMapBridge* CTutorialMapBridge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorialMapBridge* pInstance = new CTutorialMapBridge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTutorialMapBridge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTutorialMapBridge::Clone(void* pArg)
{
	CTutorialMapBridge* pInstance = new CTutorialMapBridge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTutorialMapBridge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorialMapBridge::Free()
{
	__super::Free();

	Safe_Release(m_pPhysXCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNoiseCom);
	Safe_Release(m_pShaderCom);
}
