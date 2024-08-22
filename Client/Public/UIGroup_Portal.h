#pragma once

#include "UIGroup.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUIGroup_Portal final : public CUIGroup
{
public:
	// 여섯 개를 분기처리해서 하는 것보다는... 그냥 이거 생성할 때 값 받아오게 하자 ㅋㅋ ㅠㅠ 위치랑 텍스트 같은 거~ 그게 제일 깔끔하겠구먼!
	// 그럼 UI Manager에서 함수 호출할때... UIGroup_Portal의 구조체(Desc)를 인자로 받아오거나 하는 식으로 해야겠네!
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