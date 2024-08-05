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
}

void CQTE::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQTE::Render()
{
	return S_OK;
}

HRESULT CQTE::Create_QteBtn()
{
	CUI_QTE_Btn::UI_DESC{};





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
