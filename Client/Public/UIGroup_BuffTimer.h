#pragma once

#include "UIGroup.h"

BEGIN(Client)

class CUIGroup_BuffTimer final : public CUIGroup
{
public:
	typedef struct UIGroup_BuffTimer_Desc : public UIGROUP_DESC
	{
		_uint iBuffTimerIdx;
	}UIGROUP_BUFFTIMER_DESC;

private:
	CUIGroup_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_BuffTimer(const CUIGroup_BuffTimer& rhs);
	virtual ~CUIGroup_BuffTimer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint					m_iIndex = { 0 }; 

	_float					m_fBuffTimer = { 0.f };
	_float					m_fRatio = { 0.f };

	// Bar와 Timer를 만들 때 Index를 주고 해당 Index에 따라 출력 위치 조정 / 요소 개수에 변동 생긴 경우 요소들의 Index를 변경해가며 적용?
	vector<class CUI*>		m_vecUI;


	// 생각해보니까 ItemIcon + Bar + Timer UI 세 개를 가지는 클래스가 하나 필요하고, 그 클래스를 관리하는 것이 UIGroup_BuffTimer여야 하는 것이었음.......!

	// 이미 쓴 Item과 동일한 종류의 Item을 또 쓰면 해당 Timer가 초기화되는 식!

private:
	HRESULT					Create_BuffTimer();

public:
	static CUIGroup_BuffTimer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END