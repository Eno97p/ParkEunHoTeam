#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_MonsterHP final : public CUI
{
private:
	CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHP(const CUI_MonsterHP& rhs);
	virtual ~CUI_MonsterHP() = default;

public:
	void			Set_Ratio(_float fRatio) { m_fCurrentRatio = fRatio; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

	void			Update_Pos(_vector vMonsterPos);

private:
	class CMonster*	m_pMonster = { nullptr }; // Monster가 해당 UI를 가지고 있고 변수로 값을 넘겨주는 식으로 해도 ㄱㅊ을듯
	_float			m_fCurrentRatio = 1.f;
	_float			m_fPastRatio = 1.f;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_MonsterHP*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END