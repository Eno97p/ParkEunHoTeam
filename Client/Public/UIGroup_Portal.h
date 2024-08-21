#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Portal final : public CUIGroup
{
private:
	CUIGroup_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Portal(const CUIGroup_Portal& rhs);
	virtual ~CUIGroup_Portal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CUI_PortalPic* m_pPic = { nullptr };

private:
	HRESULT					Create_UI();

public:
	static CUIGroup_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END