#include "stdafx.h"

#include"GameInstance.h"
#include"CWorker.h"
#include "SavePoint.h"

#include "UI_Activate.h"


CSavePoint::CSavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CSavePoint::CSavePoint(const CSavePoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSavePoint::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CSavePoint::Initialize(void* pArg)
{
	_tagSavePoint_Desc* pDesc = (_tagSavePoint_Desc*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMVectorSet(pDesc->vPosition.x, pDesc->vPosition.y, pDesc->vPosition.z,1.0f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	if(FAILED(Create_Activate()))
		return E_FAIL;



	return S_OK;
}

void CSavePoint::Priority_Tick(_float fTimeDelta)
{
}

void CSavePoint::Tick(_float fTimeDelta)
{
	m_pActivateUI->Tick(fTimeDelta);
		




	vector<future<void>> futures;

	//for (int i = 0; i < 8; i++)
	//{
	//	this->Test();
	//}


	
	
		//futures.push_back(m_pGameInstance->AddWork([this]() {
		//	for (int i = 0; i < 8; i++)
		//	{
		//		this->Test();
		//	}
		//}));
	
	
	

	//futures.push_back(m_pGameInstance->AddWork([this]() {
	//	this->Test();
	//}));


	/*m_pGameInstance->AddWork([this]() {
		this->Test();
	});*/
}

void CSavePoint::Late_Tick(_float fTimeDelta)
{

	CComponent* pComponent = m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform"));
	CTransform* pPlayerTransformCom = nullptr;
	if (pComponent == nullptr)
		return;

	pPlayerTransformCom = dynamic_cast<CTransform*>(pComponent);


	_vector vPlayerPos = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSavePointPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDistance = XMVectorGetX(XMVector4Length(vPlayerPos - vSavePointPos));


	if (fabs(fDistance) < 5.0f)
	{
		m_pActivateUI->Late_Tick(fTimeDelta);
		if (KEY_TAP(DIK_F))
		{
			//Save
		}
		int test = 0;
	}





	if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.0f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG

		m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif // _DEBUG


	}
}

HRESULT CSavePoint::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNummeshCount = m_pModelCom->Get_NumMeshes();


	for (size_t i = 0; i < iNummeshCount; i++)
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;





		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}





	return S_OK;
}

HRESULT CSavePoint::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Well"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CPhysXComponent::PHYSX_DESC      PhysXDesc{};
	PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
	PhysXDesc.pComponent = m_pModelCom;
	PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
	PhysXDesc.pName = "SavePoint";
	XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	//XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	/* For.Com_PhysX */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_static_For_Well"),
		TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSavePoint::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

HRESULT CSavePoint::Create_Activate()
{
	CUI::UI_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	m_pActivateUI = dynamic_cast<CUI_Activate*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Activate"), &pDesc));
	if (nullptr == m_pActivateUI)
		return E_FAIL;



	return S_OK;
}

void CSavePoint::Test()
{
	int a = 0;
	for (int i = 0; i < 1000000; i++)
	{
		a += i;
	}

}

CSavePoint* CSavePoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSavePoint* pInstance = new CSavePoint(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSavePoint");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
    
}

CGameObject* CSavePoint::Clone(void* pArg)
{
	CSavePoint* pInstance = new CSavePoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSavePoint");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSavePoint::Free()
{
	__super::Free();
	Safe_Release(m_pActivateUI);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysXCom);
}
