#include "Lagoon.h"
#include "GameInstance.h"
CLagoon::CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMap_Element(pDevice, pContext)
{
}

CLagoon::CLagoon(const CLagoon& rhs)
	:CMap_Element(rhs)
{
}

HRESULT CLagoon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLagoon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

    CMap_Element::MAP_ELEMENT_DESC* desc = (CMap_Element::MAP_ELEMENT_DESC*)(pArg);
    m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&desc->mWorldMatrix));

	return S_OK;
}

void CLagoon::Priority_Tick(_float fTimeDelta)
{
}

void CLagoon::Tick(_float fTimeDelta)
{

}

void CLagoon::Late_Tick(_float fTimeDelta)
{

	// WaveWorks 시뮬레이션 업데이트
	//Update_WaveWorks_Simulation(fTimeDelta);

	//if (m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_OwnDesc->vStartScale.y))
	//{
	//	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//	m_pTransformCom->BillBoard();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_PRIORITY, this);
	//	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	//}
        m_fTimeDelta = fTimeDelta;
}

HRESULT CLagoon::Render()
{
    HRESULT hr = S_OK;

    // 셰이더 리소스 바인딩
    if (FAILED(hr = Bind_ShaderResources()))
        return hr;

    // 카메라 위치 바인딩
    if (FAILED(hr = m_pShaderCom->Bind_RawValue("g_eyePos", m_pGameInstance->Get_CamPosition_float4(), sizeof(XMFLOAT4))))
        return hr;

  

    return S_OK;
}
HRESULT CLagoon::Add_Components()
{
    ///* For.Com_VIBuffer */
    //if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY,/* wstr.c_str()*/TEXT("Prototype_Component_Model_Elevator"),
    //    TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxLagoon"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_WindGust */
    if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WindGust"),
        TEXT("Com_WindGust"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_WINDGUST]))))
        return E_FAIL;


    /* For.Com_Foam_Intensity */
    if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Foam_Intensity"),
        TEXT("Com_Foam_Intensity"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_FOAM_INTENSITY]))))
        return E_FAIL;


    /* For.Com_Foam_Intensity */
    if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Foam_Bubbles"),
        TEXT("Com_Foam_Bubbles"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEX_FOAM_BUBBLES]))))
        return E_FAIL;


    return S_OK;
}

HRESULT CLagoon::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    return S_OK;
}

CLagoon* CLagoon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLagoon* pInstance = new CLagoon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CLagoon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLagoon::Clone(void* pArg)
{
	CLagoon* pInstance = new CLagoon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLagoon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLagoon::Free()
{
	__super::Free();

	Safe_Release(m_pOceanVB);
	Safe_Release(m_pOceanIB);
	Safe_Release(m_pOceanVSHSDSCB);
	Safe_Release(m_pOceanPSCB);
	Safe_Release(m_pOceanPerInstanceCB);

}