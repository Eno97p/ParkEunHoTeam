#include"PhysXComponent_HeightField.h"


#include "Shader.h"
#include "GameInstance.h"
#include"GameObject.h"

#include"Mesh.h"



CPhysXComponent_HeightField::CPhysXComponent_HeightField(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPhysXComponent{ pDevice, pContext }
{
}

CPhysXComponent_HeightField::CPhysXComponent_HeightField(const CPhysXComponent_HeightField & rhs)
	: CPhysXComponent{ rhs }

#ifdef _DEBUG
	,m_OutDesc{rhs.m_OutDesc}
#endif // _DEBUG
{
}

HRESULT CPhysXComponent_HeightField::Initialize_Prototype(const wstring& strPrototypeTag)
{
	CVIBuffer_Terrain* TerrainCom = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Prototype(m_pGameInstance->Get_CurrentLevel(), strPrototypeTag));
	if(!TerrainCom)
		return E_FAIL;






	if(FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	if (FAILED(Create_HeightField()))
		return E_FAIL;

	



	return S_OK;
}

HRESULT CPhysXComponent_HeightField::Initialize(void * pArg)
{
	CPhysXComponent::PHYSX_DESC* pDesc = static_cast<CPhysXComponent::PHYSX_DESC*>(pArg);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
		

	return S_OK;
}

#ifdef _DEBUG
HRESULT CPhysXComponent_HeightField::Render()
{
	if (m_OutDesc.bIsOnDebugRender)
	{
		m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

		m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
		m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));


		m_pShader->Begin(1);


		//Test: Render Call 
		//m_pBuffer[0]->Bind_Buffers();
		//m_pBuffer[0]->Render();
		for (auto& pPhysXBuffer : m_pBuffer)
		{
			if (nullptr != pPhysXBuffer)
			{
				pPhysXBuffer->Bind_Buffers();
				pPhysXBuffer->Render();
			}


		}
	}

	return S_OK;
}

#endif
HRESULT CPhysXComponent_HeightField::Create_HeightField()
{


	return S_OK;
}


CPhysXComponent_HeightField * CPhysXComponent_HeightField::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strPrototypeTag)
{
	CPhysXComponent_HeightField*		pInstance = new CPhysXComponent_HeightField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strPrototypeTag)))
	{
		MSG_BOX("Failed to Created : CPhysXComponent_HeightField");
		return nullptr;
	}

	return pInstance;
}

CComponent * CPhysXComponent_HeightField::Clone(void * pArg)
{
	CPhysXComponent_HeightField*		pInstance = new CPhysXComponent_HeightField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysXComponent_HeightField");
		return nullptr;
	}

	return pInstance;
}

void CPhysXComponent_HeightField::Free()
{
	__super::Free();


	

}
