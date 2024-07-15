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
     m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&desc->mWorldMatrix));

    if (FAILED(Add_Components((MAP_ELEMENT_DESC*)(pArg))))
        return E_FAIL;

    /*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 3.f, 5.f, 1.f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);*/

    //m_pTransformCom->Scaling(100.f, 100.f, 100.f);
    
   //CVIBuffer_Instance::INSTANCE_MAP_DESC instanceDesc{};
   //instanceDesc.WorldMats = desc->WorldMats;
   //instanceDesc.iNumInstance = desc->iInstanceCount;
   //m_pModelCom->Ready_Instance_ForMapElements(instanceDesc);

    //FOR CULLING
    //m_pGameInstance->AddCullingObject(this, m_pPhysXCom->Get_Actor());

    return S_OK;

}

void CPassive_Element::Priority_Tick(_float fTimeDelta)
{
}

void CPassive_Element::Tick(_float fTimeDelta)
{
    return;
}

void CPassive_Element::Late_Tick(_float fTimeDelta)
{
    //FOR CULLING
    //_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //if (!m_bNoCullElement)
    //{
    //    if (m_pGameInstance->isVisible(vPos, m_pPhysXCom->Get_Actor()))
    //        //if (m_pGameInstance->IsVisibleObject(this))
    //    {
    //        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
    //    }
    //}
    //else 
    {
       m_pGameInstance->Add_RenderObject(CRenderer::RENDER_MIRROR, this);
       m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
    }
  


#ifdef _DEBUG

   m_pGameInstance->Add_DebugComponent(m_pPhysXCom);
#endif


}

HRESULT CPassive_Element::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (m_pGameInstance->Key_Down(DIK_UP))
    {
        m_iTest++;
    }


    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        //if (i == 0)
        {
            m_pShaderCom->Unbind_SRVs();

            if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
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


        
        //_uint red = i;
        //if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &red, sizeof(_uint))))
        //    return E_FAIL;

        //if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CPassive_Element::Render_Mirror()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint   iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (i != 1) continue;
        
        m_pShaderCom->Unbind_SRVs();

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(3);

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CPassive_Element::Add_Components(MAP_ELEMENT_DESC* desc)
{
    // LEVEL GAMEPLAY 나중에 수정@@@@@@@@@@@@@@@

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (desc->wstrModelName == TEXT("Prototype_Component_Model_BasicCube") || desc->wstrModelName == TEXT("Prototype_Component_Model_BasicGround"))
    {
        m_bNoCullElement = true;
    }
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    // wstrModelName을 수정하여 physxName 생성
    wstring physxName = desc->wstrModelName;
    size_t pos = physxName.find(L"Model_");
    if (pos != wstring::npos)
    {
        physxName.replace(pos, 6, L"PhysX_");
    }

    CPhysXComponent::PHYSX_DESC		PhysXDesc{};
    PhysXDesc.fMatterial = _float3(0.5f, 0.5f, 0.5f);
    XMStoreFloat4x4(&PhysXDesc.fWorldMatrix, m_pTransformCom->Get_WorldMatrix());
    PhysXDesc.pComponent = m_pModelCom;
    PhysXDesc.eGeometryType = PxGeometryType::eTRIANGLEMESH;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, physxName.c_str(),
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
