#include "QTE.h"

#include "GameInstance.h"
#include "UI_QTE_Btn.h"

CQTE::CQTE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CQTE::CQTE(const CQTE& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CQTE::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQTE::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Create_QteBtn()))
		return E_FAIL;

	return S_OK;
}

void CQTE::Priority_Tick(_float fTimeDelta)
{
}

void CQTE::Tick(_float fTimeDelta)
{
	for (auto& pBtn : m_vecBtn)
		pBtn->Tick(fTimeDelta);
}

void CQTE::Late_Tick(_float fTimeDelta)
{
	for (auto& pBtn : m_vecBtn)
		pBtn->Late_Tick(fTimeDelta);
}

HRESULT CQTE::Render()
{
	return S_OK;
}

HRESULT CQTE::Create_QteBtn()
{
	_uint iRand = (rand() % 3) + 1; // 1 ~ 3

	CUI_QTE_Btn::UI_QTE_BTN_DESC pDesc{};
	pDesc.eLevel = LEVEL_STATIC;
	
	for (size_t i = 0; i < iRand; ++i)
	{
		pDesc.iBtnIndex = i;
		pDesc.fX = (_float)((rand() % (g_iWinSizeX >> 2)) + (g_iWinSizeX >> 1) * 0.5f) + i * 100.f;
		pDesc.fY = (_float)((rand() % (g_iWinSizeY >> 2)) + (g_iWinSizeY >> 1) * 0.5f) + i * 100.f;
		pDesc.fSizeX = 140.f; // 256
		pDesc.fSizeY = 140.f;
		m_vecBtn.emplace_back(dynamic_cast<CUI_QTE_Btn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QTE_Btn"), &pDesc)));
	}

	return S_OK;
}

CQTE* CQTE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQTE* pInstance = new CQTE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CQTE");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CQTE::Clone(void* pArg)
{
	CQTE* pInstance = new CQTE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CQTE");
		return nullptr;
	}

	return pInstance;
}

void CQTE::Free()
{
	__super::Free();

	for (auto& pBtn : m_vecBtn)
		Safe_Release(pBtn);
}
