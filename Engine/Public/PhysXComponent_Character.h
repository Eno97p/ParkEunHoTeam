#pragma once

#include "PhysXComponent.h"




BEGIN(Engine)
class CGameObject;
class CTransform;
class ENGINE_DLL CPhysXComponent_Character final : public CPhysXComponent
{
public:
	typedef	struct ControllerDesc :public PxCapsuleControllerDesc
	{
		_float3 fMatterial;
		CTransform* pTransform = nullptr;
		_float fJumpSpeed = 0.f;
		const char* pName = nullptr;
		PxFilterData filterData = {};
	}ControllerDesc;

	

private:
	explicit CPhysXComponent_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_Character(const CPhysXComponent_Character& rhs);
	virtual ~CPhysXComponent_Character() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void SetFilterData(PxFilterData  filterData)override;


#ifdef _DEBUG
	virtual HRESULT  Init_Buffer() override;
	virtual HRESULT Render() override;
	virtual void* GetData() override;
#endif

public:
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);


	HRESULT Go_Straight(_float fTimeDelta);
	HRESULT Go_BackWard(_float fTimeDelta);
	HRESULT Go_Jump(_float fTimeDelta, _float fJumpSpeed = 0.f);
	void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	_bool Get_IsJump() { return m_bIsJump; }



private:
	//해제할 필요 없음 컨트롤러 매니저가 한 번에 지워줌
	PxController* m_pController = { nullptr };
	CTransform* m_pTransform = { nullptr };

	PxVec3 m_vGravity = { 0.f, 0.0f, 0.f };
	_bool m_bIsJump = false;
	_float m_fJumpSpeed = 0.f;
	_float m_fCurrentY_Velocity = 0.f;
	const _float m_fGravity = -20.f;
	_float m_fSpeed = 3.f;


	
public:
	static CPhysXComponent_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END