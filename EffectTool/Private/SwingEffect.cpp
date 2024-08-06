#include "SwingEffect.h"
#include "GameInstance.h"
CSwingEffect::CSwingEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSwingEffect::CSwingEffect(const CSwingEffect& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSwingEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwingEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	m_OwnDesc = make_shared<SWINGEFFECT>(*((SWINGEFFECT*)pArg));
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	if (FAILED(Add_Components()))
		return E_FAIL;

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMat);
	WorldMat.r[0] = XMVector4Normalize(WorldMat.r[0]);
	WorldMat.r[1] = XMVector4Normalize(WorldMat.r[1]);
	WorldMat.r[2] = XMVector4Normalize(WorldMat.r[2]);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	if (FAILED(Add_Child_Effects()))
		return E_FAIL;

	return S_OK;
}

void CSwingEffect::Priority_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Priority_Tick(fTimeDelta);
}

void CSwingEffect::Tick(_float fTimeDelta)
{
	m_OwnDesc->fLifeTime -= fTimeDelta;
	if (m_OwnDesc->fLifeTime < 0.f)
		m_pGameInstance->Erase(this);

	_matrix WorldMat = XMLoadFloat4x4(m_OwnDesc->ParentMat);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WorldMat.r[3]);

	for (auto& iter : m_EffectClasses)
		iter->Tick(fTimeDelta);
}

void CSwingEffect::Late_Tick(_float fTimeDelta)
{
	for (auto& iter : m_EffectClasses)
		iter->Late_Tick(fTimeDelta);
}

HRESULT CSwingEffect::Add_Components()
{
	return S_OK;
}

HRESULT CSwingEffect::Add_Child_Effects()
{
	m_OwnDesc->SpiralDesc.ParentMatrix = m_OwnDesc->ParentMat;

	CGameObject* StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Swing_Spiral"), &m_OwnDesc->SpiralDesc);
	m_EffectClasses.emplace_back(StockValue);

	//
	//StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Spiral"), &m_OwnDesc->SpiralDesc);
	//m_EffectClasses.emplace_back(StockValue);

	//StockValue = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HealEffect_Line"), &m_OwnDesc->LineDesc);
	//m_EffectClasses.emplace_back(StockValue);
	return S_OK;
}

CSwingEffect* CSwingEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwingEffect* pInstance = new CSwingEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CSwingEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSwingEffect::Clone(void* pArg)
{
	CSwingEffect* pInstance = new CSwingEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSwingEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwingEffect::Free()
{
	__super::Free();
	for (auto& iter : m_EffectClasses)
		Safe_Release(iter);

}
