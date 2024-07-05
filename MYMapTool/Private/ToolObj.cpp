#include "ToolObj.h"

#include "GameInstance.h"
#include "Imgui_Manager.h"
#include "ToolObj_Manager.h"
#include <string>
#include <codecvt>
#include <locale>

//wstring char_to_wstring(const _char* str) {
//	wstring_convert<codecvt_utf8<wchar_t>> converter;
//	return converter.from_bytes(str);
//}

CToolObj::CToolObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CToolObj::CToolObj(const CToolObj& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CToolObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CToolObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	TOOLOBJ_DESC* pDesc = (TOOLOBJ_DESC*)pArg;

	strcpy_s(m_szName, pDesc->szObjName);
	strcpy_s(m_szLayer, pDesc->szLayer);
	strcpy_s(m_szModelName, pDesc->szModelName);
	m_eModelType = pDesc->eModelType;

	memcpy(&m_vColorOffset, &pDesc->f4Color, sizeof(_float4));

	if (!m_bIsChild)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;
	}


	if (CModel::TYPE_ANIM == m_eModelType)
		m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	Setting_WorldMatrix(pDesc);

	CToolObj_Manager::GetInstance()->Get_ToolObjs().emplace_back(this); // Obj

	// 생성 목록에 리스트 번호 매기기
	string strSize = to_string(CToolObj_Manager::GetInstance()->Get_ToolObjs().size()) + ". ";
	strcat_s(m_szListName, strSize.c_str());
	strcat_s(m_szListName, m_szName);
	CImgui_Manager::GetInstance()->Add_vecCreateObj(m_szListName);

	return S_OK;
}

void CToolObj::Priority_Tick(_float fTimeDelta)
{
}

void CToolObj::Tick(_float fTimeDelta)
{
	if (CModel::TYPE_ANIM == m_eModelType)
		m_pModelCom->Play_Animation(fTimeDelta);


	// 바람 방향 변화
	static float fWindAngle = 0.0f;
	fWindAngle += fTimeDelta * 0.1f;
	m_WindDirection = XMFLOAT3(cos(fWindAngle), 0.0f, sin(fWindAngle));

}

void CToolObj::Late_Tick(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
}

HRESULT CToolObj::Render()
{
	//풀떼기 내일 ㄱㄱ 지오메트리로 다시 정의해야할듯
	//if (m_iShaderPath == 2) // ColoredDiffuse 패스인 경우
	//{
	//	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//}
	//else // 다른 패스인 경우
	//{
	//	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) // 해당 Model의 Mesh만큼 순회
	{
		//m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		//if (i == 0 || i == 54 || i == 58) continue;

		//if (i == 0) continue;
		if (CModel::TYPE_ANIM == m_eModelType)
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		//if ( i != 29)
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

	/*	_bool f = false;
		if (FAILED(m_pShaderCom->Bind_RawValue(("g_bNormal"), &f, sizeof(_bool))))
			return E_FAIL;*/

	/*	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS)))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColorOffset", &m_vColorOffset, sizeof(_uint))))
			return E_FAIL;*/


		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Red", &i, sizeof(_uint))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_Test", &i, sizeof(_uint))))
		//	return;

		m_pShaderCom->Begin(m_iShaderPath);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CToolObj::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(m_pGameInstance->Get_ShadowEye(), m_pGameInstance->Get_ShadowFocus(), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 1000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (i == 54 || i == 58) continue;
		if (CModel::TYPE_ANIM == m_eModelType)
		{
			m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
	
		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	return S_OK;
}


HRESULT CToolObj::Add_Component()
{
	std::wstring wstr(m_szModelName, m_szModelName + strlen(m_szModelName));
	//wstr = TEXT("Prototype_Component_Model_Fiona");
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(Check_AnimType()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolObj::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//// 바람 관련 변수 바인딩
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_WindDirection", &m_WindDirection, sizeof(XMFLOAT3))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_WindStrength", &m_WindStrength, sizeof(float))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_WindFrequency", &m_WindFrequency, sizeof(float))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_WindGustiness", &m_WindGustiness, sizeof(float))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeDelta, sizeof(float))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CToolObj::Check_AnimType()
{
	// Obj Anim 체크
	if (CModel::TYPE_ANIM == m_eModelType)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}
	else if (CModel::TYPE_NONANIM == m_eModelType)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	return S_OK;
}

void CToolObj::Setting_WorldMatrix(void* pArg)
{
	TOOLOBJ_DESC* pDesc = (TOOLOBJ_DESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3]);
	m_pTransformCom->Set_Scale(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[2][2]);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(pDesc->mWorldMatrix.m[0][0], pDesc->mWorldMatrix.m[0][1], pDesc->mWorldMatrix.m[0][2], pDesc->mWorldMatrix.m[0][3]));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(pDesc->mWorldMatrix.m[1][0], pDesc->mWorldMatrix.m[1][1], pDesc->mWorldMatrix.m[1][2], pDesc->mWorldMatrix.m[1][3]));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(pDesc->mWorldMatrix.m[2][0], pDesc->mWorldMatrix.m[2][1], pDesc->mWorldMatrix.m[2][2], pDesc->mWorldMatrix.m[2][3]));
}

CToolObj* CToolObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolObj* pInstance = new CToolObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CToolObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToolObj::Clone(void* pArg)
{
	CToolObj* pInstance = new CToolObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CToolObj");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
