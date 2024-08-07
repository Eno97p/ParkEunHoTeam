#include "AndrasLazer_Base.h"
#include "GameInstance.h"

CAndrasLazerBase::CAndrasLazerBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBlendObject(pDevice, pContext)
{
}

CAndrasLazerBase::CAndrasLazerBase(const CAndrasLazerBase& rhs)
	:CBlendObject(rhs)
{
}

HRESULT CAndrasLazerBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndrasLazerBase::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<ANDRAS_LASER_BASE_DESC>(*((ANDRAS_LASER_BASE_DESC*)pArg));
	CGameObject::GAMEOBJECT_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(m_OwnDesc->fRotationSpeed);
	desc.fSpeedPerSec = 0.f;
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);
	WorldMat.r[3] = XMVector3TransformCoord(XMLoadFloat3(&m_OwnDesc->vOffset), WorldMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]); //* m_OwnDesc->vStartSize.y;
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]); //* m_OwnDesc->vStartSize.z;
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_CurrentSize = {0.f,0.f,0.f };
	return S_OK;
}

void CAndrasLazerBase::Priority_Tick(_float fTimeDelta)
{
}

void CAndrasLazerBase::Tick(_float fTimeDelta)
{
	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMatrix);

	m_fCurLifeTime += fTimeDelta;
	if (m_fCurLifeTime >= m_OwnDesc->fMaxLifeTime)
	{
		m_fCurLifeTime = m_OwnDesc->fMaxLifeTime;
		m_pGameInstance->Erase(this);
	}
	m_fLifeTimeRatio = m_fCurLifeTime / m_OwnDesc->fMaxLifeTime;
	m_fLifeTimeRatio = max(0.f, min(m_fLifeTimeRatio, 1.f));

	if (m_fLifeTimeRatio < 0.2f)
	{
		_vector CurSize = XMLoadFloat3(&m_CurrentSize);
		_vector LerpSize = XMLoadFloat3(&m_OwnDesc->vMaxSize);
		CurSize = XMVectorLerp(CurSize, LerpSize, 0.5f);
		XMStoreFloat3(&m_CurrentSize, CurSize);
	}
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);
	m_pTransformCom->Set_Scale(m_CurrentSize.x, m_CurrentSize.y, m_CurrentSize.z);
}

void CAndrasLazerBase::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLOOM, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CAndrasLazerBase::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);

	}
	return S_OK;
}

HRESULT CAndrasLazerBase::Render_Bloom()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CAndrasLazerBase::Render_Distortion()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		m_pShaderCom->Begin(1);
		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CAndrasLazerBase::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andras_Lazer_Base"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshEffect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Desolve16"),
		TEXT("Com_Desolve"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndrasLazerBase::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DesolveTexture", m_OwnDesc->NumDesolve)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurTime", &m_fCurLifeTime, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Speed", &m_OwnDesc->fUVSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fLifeTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_OwnDesc->fColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CAndrasLazerBase* CAndrasLazerBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndrasLazerBase* pInstance = new CAndrasLazerBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAndrasLazerBase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAndrasLazerBase::Clone(void* pArg)
{
	CAndrasLazerBase* pInstance = new CAndrasLazerBase(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAndrasLazerBase");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAndrasLazerBase::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
}
