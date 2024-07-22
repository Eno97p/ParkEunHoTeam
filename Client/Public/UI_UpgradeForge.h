#pragma once

#include "UI_Interaction.h"

BEGIN(Client)

class CUI_UpgradeForge final : public CUI_Interaction
{
private:
	CUI_UpgradeForge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_UpgradeForge(const CUI_UpgradeForge& rhs);
	virtual ~CUI_UpgradeForge() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CUI_UpgradeForge*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END