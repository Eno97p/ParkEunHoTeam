#include "UIGroup_BuffTimer.h"

#include "GameInstance.h"
#include "UI_BuffTimer.h"

CUIGroup_BuffTimer::CUIGroup_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIGroup{ pDevice, pContext }
{
}

CUIGroup_BuffTimer::CUIGroup_BuffTimer(const CUIGroup_BuffTimer& rhs)
	: CUIGroup{ rhs }
{
}

HRESULT CUIGroup_BuffTimer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIGroup_BuffTimer::Initialize(void* pArg)
{
	UIGROUP_BUFFTIMER_DESC* pDesc = static_cast<UIGROUP_BUFFTIMER_DESC*>(pArg);

	m_iIndex = pDesc->iBuffTimerIdx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_BuffTimer())) // Test용 (추후 동적으로 생성되도록 변경할 것임. 위치 확인 위해 테스트용으로 호출)
		return E_FAIL;

	//m_isRend = true; // 항상 그려지도록 하기 (요소가 없으면 알아서 안 그려질 것) >> 이 값 자체가 필요 없지 않을까?

	return S_OK;
}

void CUIGroup_BuffTimer::Priority_Tick(_float fTimeDelta)
{
}

void CUIGroup_BuffTimer::Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
		pUI->Tick(fTimeDelta);
}

void CUIGroup_BuffTimer::Late_Tick(_float fTimeDelta)
{
	for (auto& pUI : m_vecUI)
		pUI->Late_Tick(fTimeDelta);
}

HRESULT CUIGroup_BuffTimer::Render()
{
	return S_OK;
}

HRESULT CUIGroup_BuffTimer::Create_BuffTimer()
{
	CUI_BuffTimer::UI_BUFFTIMER_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	pDesc.iBuffTimerIdx = m_vecUI.size();
	m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer"), &pDesc)));

	pDesc.iBuffTimerIdx = m_vecUI.size();
	m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer"), &pDesc)));

	pDesc.iBuffTimerIdx = m_vecUI.size();
	m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer"), &pDesc)));

	pDesc.iBuffTimerIdx = m_vecUI.size();
	m_vecUI.emplace_back(dynamic_cast<CUI_BuffTimer*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_BuffTimer"), &pDesc)));

	return S_OK;
}

CUIGroup_BuffTimer* CUIGroup_BuffTimer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIGroup_BuffTimer* pInstance = new CUIGroup_BuffTimer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUIGroup_BuffTimer");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CUIGroup_BuffTimer::Clone(void* pArg)
{
	CUIGroup_BuffTimer* pInstance = new CUIGroup_BuffTimer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CUIGroup_BuffTimer");
		return nullptr;
	}

	return pInstance;
}

void CUIGroup_BuffTimer::Free()
{
	__super::Free();

	for (auto& pUI : m_vecUI)
		Safe_Release(pUI);
}
