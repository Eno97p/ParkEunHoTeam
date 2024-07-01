#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateSoul final : public CUI
{
public:
	typedef struct UI_Sous_Desc : public UI_DESC
	{
		_bool		isInv = { false };
	}UI_SOUL_DESC;
private:
	CUI_StateSoul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateSoul(const CUI_StateSoul& rhs);
	virtual ~CUI_StateSoul() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_bool			m_isInv = { false };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_Data();

public:
	static CUI_StateSoul*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END