#include "Grass.h"
#include "GameInstance.h"
CGrass::CGrass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CGrass::CGrass(const CGrass& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CGrass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGrass::Initialize(void* pArg)
{
	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;



	if (nullptr != pArg)
	{

		

	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pVIBufferCom->Setup_Onterrain(dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer"))));
	//m_pVIBufferCom->Initial_RotateY();


	GRASS_DESC* gd = static_cast<GRASS_DESC*>(pArg);
	m_vTopCol = gd->vTopCol;
	m_vBotCol = gd->vBotCol;

	//랜덤으로 탑색 변경 살짞 어둡게
	_float randFloat = RandomFloat(0.f, 0.2f);
	m_vTopColorOffset = { randFloat , randFloat , randFloat };
	m_vTopCol.x -= randFloat;
	m_vTopCol.y -= randFloat;
	m_vTopCol.z -= randFloat;

	return S_OK;
}

void CGrass::Priority_Tick(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	if (m_fAccTime > 100000.f)
	{
		m_fAccTime = 0.f;
	}

	// 바람 세기 변화 로직
	m_fWindChangeTime += fTimeDelta;

	// 새로운 목표 풍속 설정
	if (m_fWindChangeTime >= m_fWindChangeDuration)
	{
		m_fTargetWindStrength = (rand() % 100) / 100.f * 5.f + 10.f; // 범위 축소
		m_fWindChangeTime = 0.f;
	}

	// 현재 풍속을 목표 풍속으로 부드럽게 보간
	float t = m_fWindChangeTime / m_fWindChangeDuration;
	float smoothT = t * t * (3 - 2 * t); // 부드러운 보간을 위한 에르미트 보간
	m_fWindStrength = Lerp(m_fWindStrength, m_fTargetWindStrength, smoothT);

	// 주기적인 변동 제거

	// 풍속 범위 제한
	m_fWindStrength = Clamp(m_fWindStrength, 10.f, 15.f);
}

void CGrass::Tick(_float fTimeDelta)
{
	//m_pVIBufferCom->Drop(fTimeDelta);
}

void CGrass::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CGrass::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

_uint CGrass::Get_NumInstance()
{
	return m_pVIBufferCom->Get_Instance_Desc()->iNumInstance;
}

vector<_float4x4*> CGrass::Get_VtxMatrices()
{
	return m_pVIBufferCom->Get_VtxMatrices();
}

HRESULT CGrass::Add_Components(void* pArg)
{
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};

	/* For.Prototype_Component_VIBuffer_Instance_Point*/
	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vOffsetPos = _float3(0.0f, 0.f, 0.0f);
	InstanceDesc.vPivotPos = m_vPivotPos;
	InstanceDesc.vRange = _float3(25.0f, 0.f, 25.0f);
	InstanceDesc.vSize = _float2(1.f, 5.f);
	InstanceDesc.vSpeed = _float2(1.f, 7.f);
	InstanceDesc.vLifeTime = _float2(10.f, 15.f);
	InstanceDesc.isLoop = true;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;




	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Grass"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	GRASS_DESC* desc = static_cast<GRASS_DESC*>(pArg);
	{

		/* For.Com_Texture */
		if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, desc->wstrModelName.c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;

		///* For.Com_Texture */
		//if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grass_TT_Normal"),
		//	TEXT("Com_Normal"), reinterpret_cast<CComponent**>(&m_pNormalCom))))
		//	return E_FAIL;
	}

	

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NoiseRepeat"),
		TEXT("Com_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseCom))))
		return E_FAIL;


	CVIBuffer_Instance_Point::INSTANCE_MAP_DESC mapdesc{};

	mapdesc.WorldMats = (desc->WorldMats);

	m_pVIBufferCom->Ready_Instance_ForGrass(mapdesc);

	return S_OK;
}

HRESULT CGrass::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	/*if (FAILED(m_pNormalCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
		return E_FAIL;*/

	if (FAILED(m_pNoiseCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTopColor", &m_vTopCol, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBotColor", &m_vBotCol, sizeof(_float3))))
		return E_FAIL;	
	
	_float bill = 0.5f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBillboardFactor", &bill, sizeof(_float))))
		return E_FAIL;	
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fElasticityFactor", &bill, sizeof(_float))))
		return E_FAIL;
	
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vTopColorOffset", &m_vTopColorOffset, sizeof(_float3))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;

	m_fWindStrength = 10.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWindStrength", &m_fWindStrength, sizeof(_float))))
		return E_FAIL;	
	
	_float3 WindDir = {1.f, 0.f, 0.f};
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vWindDirection", &WindDir, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CGrass* CGrass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrass* pInstance = new CGrass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGrass::Clone(void* pArg)
{
	CGrass* pInstance = new CGrass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CGrass");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrass::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseCom);
	//Safe_Release(m_pNormalCom);
	Safe_Release(m_pShaderCom);
}
