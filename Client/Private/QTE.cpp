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
	Start_BtnEvent();

	for (auto& pBtn : m_vecBtn)
		pBtn->Tick(fTimeDelta);

	// Btn의 m_isScore가 전부 true일 때 모든 Btn의 score 값을 받아와서 Bad가 하나라도 있으면 실패 처리? 그리고 없애버리기!
	// 체크하고 없애는 코드 필요 > 추후 Andras와 연결 필요

	if (Check_End())
	{
		Check_ResultScore();
	}
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
		pDesc.fSizeX = 256.f; // 512
		pDesc.fSizeY = 256.f;
		m_vecBtn.emplace_back(dynamic_cast<CUI_QTE_Btn*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_QTE_Btn"), &pDesc)));
	}

	return S_OK;
}

void CQTE::Start_BtnEvent()
{
	vector<CUI_QTE_Btn*>::iterator btn = m_vecBtn.begin();

	while (true)
	{
		if (btn == m_vecBtn.end())
			return;

		if ((*btn)->Get_isScore())
		{
			++btn;
			if (btn == m_vecBtn.end())
				return;
			
			if(!(*btn)->Get_isScore())
				(*btn)->Set_Start(true);
		}
		else
			++btn;
	}
}

_bool CQTE::Check_End()
{
	// 마지막 녀석이 End 되었으면 true 반환
	vector<CUI_QTE_Btn*>::iterator btn = m_vecBtn.begin();
	for (size_t i = 0; i < m_vecBtn.size() - 1; ++i)
		++btn;

	if ((*btn)->Get_isScore()) // 마지막 Btn의 점수가 났다면
		return true;
	else
		return false;
}

void CQTE::Check_ResultScore()
{
	// Btn을 순회하며 Score를 체크하고 하나라도 Bad가 있으면 ~ / 통과했으면 ~ 기믹 처리 필요(Andras와 연결)
	for (auto& pBtn : m_vecBtn)
	{
		if (pBtn->Get_ScoreType() == CUI_QTE_Btn::SCORE_BAD) // Bad가 하나라도 있으면 ~~~ 처리
		{


			return; 
		}
	}

	// 없으면 ~~~ 처리

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
