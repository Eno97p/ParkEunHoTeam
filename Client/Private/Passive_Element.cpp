#include "stdafx.h"
#include "..\Public\Passive_Element.h"

#include "GameInstance.h"


CPassive_Element::CPassive_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Element{ pDevice, pContext }
{
}

CPassive_Element::CPassive_Element(const CPassive_Element& rhs)
    : CMap_Element{ rhs }
{
}

HRESULT CPassive_Element::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPassive_Element::Initialize(void* pArg)
{


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;





    MAP_ELEMENT_DESC* desc = (MAP_ELEMENT_DESC*)(pArg);
    //m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&desc->mWorldMatrix));

    if (FAILED(Add_Components((MAP_ELEMENT_DESC*)(pArg))))
        return E_FAIL;

    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 3.f, 5.f, 1.f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);*/

    //m_pTransformCom->Scaling(100.f, 100.f, 100.f);
    
    CVIBuffer_Instance::INSTANCE_DESC instanceDesc{};
    instanceDesc.WorldMats = desc->WorldMats;
    instanceDesc.iNumInstance = desc->iInstanceCount;
    m_pModelCom->Ready_Instance_ForMapElements(instanceDesc);


    return S_OK;

}

void CPassive_Element::Priority_Tick(_float fTimeDelta)
{
}

void CPassive_Element::Tick(_float fTimeDelta)
{

  
}

void CPassive_Element::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);


#ifdef _DEBUG

    m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif


}

HRESULT CPassive_Element::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;



    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //if (i == 0)
        {
            m_pShaderCom->Unbind_SRVs();

            if (FAILED(m_pModelCom->Bind_Material_Instance(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
                return E_FAIL;


			   /* if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
					return E_FAIL;*/

		/*		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
					return E_FAIL;*/
		}
		//else if (i == 1)
		//{
		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//		return E_FAIL;
		//}

            /*   if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
                  return E_FAIL;

               if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
                  return E_FAIL;*/
        //else if (i == 1)
        //{
        //   if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
        //      return E_FAIL;
        //}


        m_pShaderCom->Begin(0);

        // 모든 객체의 인스턴스를 한 번에 렌더링
        if (FAILED(m_pModelCom->Render_Instance_ForMapElements(i)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CPassive_Element::Add_Components(MAP_ELEMENT_DESC* desc)
{
    //_int iLevelIndex = m_pGameInstance->Get_

    //LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
    //LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
    //LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
    //LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@
    //LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str() /*TEXT("Prototype_Component_Model_TronesT03")*/,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;




    


    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-333.f, 154.f, -160.f, 1.f));
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
    //matWorld= m_pTransformCom->Get_WorldMatrix()

    CPhysXComponent::PHYSX_DESC		PhysXDesc{};
    PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
    XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
    PhysXDesc.pComponent = m_pModelCom;
    PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physx_static"),
        TEXT("Com_PhysX"), reinterpret_cast<CComponent**>(&m_pPhysXCom), &PhysXDesc)))
        return E_FAIL;





    return S_OK;
}

HRESULT CPassive_Element::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    return S_OK;
}

CPassive_Element* CPassive_Element::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPassive_Element* pInstance = new CPassive_Element(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPassive_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPassive_Element::Clone(void* pArg)
{
    CPassive_Element* pInstance = new CPassive_Element(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CPassive_Element");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPassive_Element::Free()
{
    __super::Free();


    Safe_Release(m_pPhysXCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
