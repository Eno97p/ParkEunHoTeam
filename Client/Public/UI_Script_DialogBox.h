#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Script_DialogBox final : public CUI
{
public:
	enum NPC_TYPE { NPC_RLYA, NPC_END };
	typedef struct UI_Script_DialogBox_Desc : public UI_DESC
	{
		NPC_TYPE	eNpcType;
	}UI_SCRIPT_DIALOGBOX_DESC;

private:
	CUI_Script_DialogBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Script_DialogBox(const CUI_Script_DialogBox& rhs);
	virtual ~CUI_Script_DialogBox() = default;

	void			Set_DialogCnt(_uint iDialogCnt) { m_iDialogCnt = iDialogCnt; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void	Priority_Tick(_float fTimeDelta) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	_uint			m_iDialogCnt = { 0 };
	wstring			m_wstrDialogText;

	NPC_TYPE		m_eNpcType = { NPC_END };

private:
	HRESULT	Add_Components();
	HRESULT	Bind_ShaderResources();

	void	Setting_Data();
	void	Setting_Text();

public:
	static CUI_Script_DialogBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END