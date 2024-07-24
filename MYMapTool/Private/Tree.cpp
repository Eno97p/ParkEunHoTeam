#include "Tree.h"
#include "ToolObj_Manager.h"
#include "GameInstance.h"
#include "Player.h"

CTree::CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CToolObj(pDevice, pContext)
{
}

CTree::CTree(const CTree& rhs)
	: CToolObj(rhs)
{
}

HRESULT CTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;



	_float4x4 tmp = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
	if (nullptr != pArg)
	{

		CToolObj::TOOLOBJ_DESC* pDesc = (CToolObj::TOOLOBJ_DESC*)pArg;
		strcpy_s(m_szName, "Prototype_GameObject_Tree");
		strcpy_s(m_szLayer, "Layer_Vegetation");
		strcpy_s(m_szModelName, "Prototype_Component_Model_BasicTree");
		m_eModelType = CModel::TYPE_NONANIM;
		m_iTriggerType = pDesc->TriggerType;

		CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj


		// 생성 목록에 리스트 번호 매기기
		string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
		strcat_s(m_szListName, strSize.c_str());
		strcat_s(m_szListName, m_szName);
		CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);


		/*m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);*/

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((GRASS_DESC*)pArg)->vStartPos));

		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		tmp = pDesc->mWorldMatrix;
		//TRIGGER STATE SET
	}

	TREE_DESC* gd = static_cast<TREE_DESC*>(pArg);
	m_LeafCol = gd->vLeafCol;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	vector<_float4x4*> WorldMats;
	for (_int i = 0; i < 1; ++i)
	{
		_float4x4* pNewMat = new _float4x4(tmp);  // 새로운 행렬 동적 할당
		//pNewMat->m[3][0] += i * 20.f;  // X 위치를 3씩 증가
		WorldMats.emplace_back(pNewMat);
	}

	CVIBuffer_Instance::INSTANCE_MAP_DESC instanceDesc{};
	instanceDesc.WorldMats = WorldMats;
	instanceDesc.iNumInstance = 1;
	m_pModelCom->Ready_Instance_ForMapElements(instanceDesc);

	// 메모리 해제
	for (auto& pMat : WorldMats)
	{
		delete pMat;
	}

	return S_OK;
}

void CTree::Priority_Tick(_float fTimeDelta)
{

}

void CTree::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta * 3.f;
	if (m_fTime > 10000.f)
	{
		m_fTime = 0.f;
	}
}

void CTree::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
#endif

	//DECAL
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
}

HRESULT CTree::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		m_pShaderCom->Unbind_SRVs();

		if (FAILED(m_pModelCom->Bind_Material_Instance_ForMapElements(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//	return E_FAIL;


		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
		//	return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
		//	return E_FAIL;

		//if ( i != 29)
		
		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &m_iTest, sizeof(_uint))))
		//	return E_FAIL;

		//if (i == 1)
		//{
		//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE)))
		//		return E_FAIL;
		//}
		if (i == 1)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
				return E_FAIL;

			_float3 vWindDirection = CImgui_Manager::GetInstance()->Get_GlobalWindDir(); // 바람의 방향
			XMVector3Normalize(XMLoadFloat3(&vWindDirection));
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vWindDirection", &vWindDirection, sizeof(_float3))))
				return E_FAIL;

			float fWindStrength = CImgui_Manager::GetInstance()->Get_GlobalWindStrenth(); // 바람의 세기
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fWindStrength", &fWindStrength, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_LeafCol", &m_LeafCol, sizeof(_float3))))
				return E_FAIL;

			m_pShaderCom->Begin(5);
		}
		else
		{
			m_pShaderCom->Begin(0);

		

		}

		if (FAILED(m_pModelCom->Render_Instance_ForMapElements(i)))
			return E_FAIL;
	}
}

HRESULT CTree::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_iTest++;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;

	_float3 vWindDirection = CImgui_Manager::GetInstance()->Get_GlobalWindDir(); // 바람의 방향
	XMVector3Normalize(XMLoadFloat3(&vWindDirection));
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vWindDirection", &vWindDirection, sizeof(_float3))))
		return E_FAIL;

	float fWindStrength = CImgui_Manager::GetInstance()->Get_GlobalWindStrenth(); // 바람의 세기
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWindStrength", &fWindStrength, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_LeafCol", &m_LeafCol, sizeof(_float3))))
		return E_FAIL;

	m_pShaderCom->Begin(5);


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	

	m_pShaderCom->Unbind_SRVs();

	if (FAILED(m_pModelCom->Bind_Material_Instance_ForMapElements(m_pShaderCom, "g_DiffuseTexture", 1, aiTextureType_DIFFUSE)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render_Instance_ForMapElements(1)))
		return E_FAIL;


	m_pShaderCom->Begin(1);

	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CTree::Add_Components(void* pArg)
{
	wstring wstr = string_to_wstring(m_szModelName);

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, wstr.c_str(),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_MapElement"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* For.Com_Collider */
	//CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	//ColliderDesc.eType = CCollider::TYPE_AABB;

	//// 월드 매트릭스에서 스케일 추출
	//_float3 vScale;
	//vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	//vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	//vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	//// 기본 크기에 스케일 적용
	//ColliderDesc.vExtents = _float3(0.5f * vScale.x, 1.f * vScale.y, 0.5f * vScale.z);
	//ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	//if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
	//	TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CTree::Bind_ShaderResources()
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

CTree* CTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTree* pInstance = new CTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTree::Clone(void* pArg)
{
	CTree* pInstance = new CTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree::Free()
{
	__super::Free();

	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
}
