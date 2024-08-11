#include "Lagoon.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CLagoon::CLagoon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CLagoon::CLagoon(const CLagoon& rhs)
	: CToolObj(rhs)
{
}

HRESULT CLagoon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLagoon::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;




	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Lagoon");
		strcpy_s(m_szLayer, "Layer_Active_Element");
		strcpy_s(m_szModelName, "Prototype_Component_Model_Lagoon");
		m_eModelType = CModel::TYPE_NONANIM;
		m_iTriggerType = pDesc->TriggerType;

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//TRIGGER STATE SET
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CLagoon::Priority_Tick(_float fTimeDelta)
{

}

void CLagoon::Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if (m_fAccTime > 100000.f)
	{
		m_fAccTime = 0;
	}
}

void CLagoon::Late_Tick(_float fTimeDelta)
{
	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CLagoon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
			return E_FAIL;

	/*	if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
				return E_FAIL;
		}*/
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}
}

HRESULT CLagoon::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

		m_pShaderCom->Unbind_SRVs();


			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", 1, aiTextureType_EMISSIVE)))
				return E_FAIL;


		m_pShaderCom->Begin(6);

		m_pModelCom->Render(1);
}

HRESULT CLagoon::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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

	/*if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 7)))
		return E_FAIL;*/

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

	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
}
