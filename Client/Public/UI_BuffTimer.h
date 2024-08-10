#pragma once

#include "UI.h"

BEGIN(Client)

// BuffTimer ���� UI���� ��� ������ �����ϴ� Ŭ����. UIGroup_BuffTimer�� �ش� Ŭ���� ��ü�� ��� ������ ���� ������

class CUI_BuffTimer final : public CUI
{
public:
	typedef struct UI_BuffTimer_Desc : public UI_DESC
	{
		_uint		iBuffTimerIdx;
		wstring		wstrTextureName;
	}UI_BUFFTIMER_DESC;

private:
	CUI_BuffTimer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BuffTimer(const CUI_BuffTimer& rhs);
	virtual ~CUI_BuffTimer() = default;

public:
	void			Set_Index(_uint iBuffTimerIdx) { m_iBuffTimerIdx = iBuffTimerIdx; }
	wstring			Get_TextureName() { return m_wstrTextureName; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint				m_iBuffTimerIdx = { 0 };
	wstring				m_wstrTextureName;

	vector<class CUI*>	m_vecUI;

private:
	HRESULT	Create_UI(wstring wstrTextureName);

	void	Setting_UIPosition();

public:
	static CUI_BuffTimer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END