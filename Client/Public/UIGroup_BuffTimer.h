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

	// Bar�� Timer�� ���� �� Index�� �ְ� �ش� Index�� ���� ��� ��ġ ���� / ��� ������ ���� ���� ��� ��ҵ��� Index�� �����ذ��� ����?
	vector<class CUI*>		m_vecUI;


	// �����غ��ϱ� ItemIcon + Bar + Timer UI �� ���� ������ Ŭ������ �ϳ� �ʿ��ϰ�, �� Ŭ������ �����ϴ� ���� UIGroup_BuffTimer���� �ϴ� ���̾���.......!

	// �̹� �� Item�� ������ ������ Item�� �� ���� �ش� Timer�� �ʱ�ȭ�Ǵ� ��!

private:
	HRESULT					Create_BuffTimer();

public:
	static CUIGroup_BuffTimer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END