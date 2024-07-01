#pragma once

#include "Base.h"

/* ���� �Ҵ�Ǿ��ִ� �������� ����ִ´�. */
/* ����ִ� ������ �ݺ����� ������ �����Ѵ�. */
/* ������ ��ü �۾��� �����Ѵ�. */
/* ���� ��ü ��, ���� ������ �ڿ��帪 Ŭ�����Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	/* ���ο� ������ ��ü�Ѵ�. */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevel() { return m_iLevelIndex; }

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iLevelIndex = { 0 };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

END