#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup abstract : public CGameObject
{
public:
	typedef struct UIGroup_Desc : public GAMEOBJECT_DESC
	{
		LEVEL	eLevel;
	}UIGROUP_DESC;

protected:
	CUIGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup(const CUIGroup& rhs);
	virtual ~CUIGroup() = default;

public:
	_bool			Get_Rend() { return m_isRend; }
	void			Set_Rend(_bool isRend) { m_isRend = isRend; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_bool		m_isRend = { false };

	LEVEL		m_eLevel = { LEVEL_END };

public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END