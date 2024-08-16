#pragma once
#include "BlendObject.h"
#include "Effect_Define.h"
BEGIN(Effect)
class CHedgehog final : public CGameObject
{
public:
	typedef struct HEDGEHOG
	{
		_float fLifeTime = 0.f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float4 vEndPos = { 0.f,0.f,0.f,1.f };
	};

private:
	CHedgehog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHedgehog(const CHedgehog& rhs);
	virtual ~CHedgehog() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();

public:
	static CHedgehog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END