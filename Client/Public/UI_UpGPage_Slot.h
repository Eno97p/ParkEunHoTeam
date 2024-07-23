#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_UpGPage_Slot final : public CUI_Interaction
{
private:
	CUI_UpGPage_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpGPage_Slot(const CUI_UpGPage_Slot& rhs);
	virtual ~CUI_UpGPage_Slot() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CUI_UpGPage_SelectSlot*		m_pSelectUI = { nullptr };
	class CUI_UpGPage_ItemSlot*			m_pItemSlot = { nullptr };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	HRESULT	Create_UI();


public:
	static CUI_UpGPage_Slot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END