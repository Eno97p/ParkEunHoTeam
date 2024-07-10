#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)
class CUI;

class CUIGroup_MonsterHP final : public CUIGroup
{
private:
	CUIGroup_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_MonsterHP(const CUIGroup_MonsterHP& rhs);
	virtual ~CUIGroup_MonsterHP() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void			Update_Pos(_vector vMonsterPos);

	// 몬스터의 위치, HP등을 받아와서 관리?
	// HP는 피격 당할 때마다 세팅

private:
	_vector					m_vMonsterPos;

	vector<CUI*>			m_vecUI;

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_MonsterHP*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END