#include "Map_Flat.h"
#include "GameInstance.h"

CFlatMap::CFlatMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMap { pDevice, pContext }
{
}

CFlatMap::CFlatMap(const CFlatMap & rhs)
	: CMap{ rhs }
{
}

HRESULT CFlatMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlatMap::Initialize(void * pArg)
{

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(((MAP_DEC*)pArg)->Scale.x, ((MAP_DEC*)pArg)->Scale.y, ((MAP_DEC*)pArg)->Scale.z);
	_vector vPos = XMLoadFloat4(&((MAP_DEC*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_Mapdec = *(MAP_DEC*)pArg;
	return S_OK;
}

void CFlatMap::Priority_Tick(_float fTimeDelta)
{
}

void CFlatMap::Tick(_float fTimeDelta)
{

	
}

void CFlatMap::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CFlatMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pShaderCom->Unbind_SRVs();
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}

	return S_OK;
}



HRESULT CFlatMap::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Flat"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	


	return S_OK;
}

HRESULT CFlatMap::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CFlatMap * CFlatMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlatMap*		pInstance = new CFlatMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : MapTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFlatMap::Clone(void * pArg)
{
	CFlatMap*		pInstance = new CFlatMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : MapTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlatMap::Free()
{
	__super::Free();

}
