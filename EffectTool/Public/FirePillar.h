#pragma once
#include "GameObject.h"
#include "Effect_Define.h"
BEGIN(Effect)
class CFirePillar : public CGameObject
{
public:
	typedef struct FIREPILLAR
	{
		_float fLifeTime = 0.f;
		_float4 vStartPos = { 0.f,0.f,0.f,1.f };
		_float3 vScale = { 1.f,1.f,1.f };
	};

private:
	CFirePillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFirePillar(const CFirePillar& rhs);
	virtual ~CFirePillar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	HRESULT Add_Components();
	HRESULT Add_Child_Effects();

private:
	shared_ptr<FIREPILLAR> m_OwnDesc;

public:
	static CFirePillar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END