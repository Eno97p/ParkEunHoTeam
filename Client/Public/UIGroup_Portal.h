#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Portal final : public CUIGroup
{
public:
	// ���� ���� �б�ó���ؼ� �ϴ� �ͺ��ٴ�... �׳� �̰� ������ �� �� �޾ƿ��� ���� ���� �Ф� ��ġ�� �ؽ�Ʈ ���� ��~ �װ� ���� ����ϰڱ���!
	// �׷� UI Manager���� �Լ� ȣ���Ҷ�... UIGroup_Portal�� ����ü(Desc)�� ���ڷ� �޾ƿ��ų� �ϴ� ������ �ؾ߰ڳ�!
	typedef struct UIGroup_Portal_Desc : public UIGROUP_DESC
	{
		_bool		isPic;
		_uint		iPicNum;
		_float		fAngle;
		_vector		vPos;
		LEVEL		ePortalLevel;
	}UIGROUP_PORTAL_DESC;

private:
	CUIGroup_Portal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIGroup_Portal(const CUIGroup_Portal& rhs);
	virtual ~CUIGroup_Portal() = default;

public:
	_bool			Get_isPic();
	void			Set_isDeadDissolve();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CUI_PortalPic*	m_pPic = { nullptr };
	class CUI_PortalText*	m_pText = { nullptr };

private:
	HRESULT					Create_UI(_bool isPic, _uint iPicNum, _float fAngle, _vector vPos);

public:
	static CUIGroup_Portal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END