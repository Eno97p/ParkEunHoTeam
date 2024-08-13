#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_QTE_Ring final : public CUI
{
public:
	enum RING_TYPE { RING_RED = 0, RING_PINK, RING_BLUE, RING_GREEN, RING_END };
	typedef struct UI_Ring_Desc : public UI_DESC
	{
		RING_TYPE	eRingType;
	}UI_RING_DESC;

private:
	CUI_QTE_Ring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_QTE_Ring(const CUI_QTE_Ring& rhs);
	virtual ~CUI_QTE_Ring() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	RING_TYPE		m_eRingType = { RING_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_QTE_Ring*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END