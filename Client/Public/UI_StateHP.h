#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_StateHP final : public CUI
{
private:
	CUI_StateHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StateHP(const CUI_StateHP& rhs);
	virtual ~CUI_StateHP() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_StateHP*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END