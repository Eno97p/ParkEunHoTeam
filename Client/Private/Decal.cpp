#include "stdafx.h"
#include "..\Public\Decal.h"

#include "GameInstance.h"
#include "ComputeShader_Buffer.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC* gameobjDesc = (GAMEOBJECT_DESC*)pArg;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(gameobjDesc->mWorldMatrix._41, gameobjDesc->mWorldMatrix._42, gameobjDesc->mWorldMatrix._43, 1.f));

	return S_OK;
}

void CDecal::Priority_Tick(_float fTimeDelta)
{
}

void CDecal::Tick(_float fTimeDelta)
{
}

void CDecal::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DECAL, this);
}

pair<_uint, _matrix> CDecal::Render_Decal()
{
	// first : 텍스쳐 번호, second : 월드 역행렬(건들지 말것)
	return pair(43, m_pTransformCom->Get_WorldMatrix_Inverse());
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDecal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
