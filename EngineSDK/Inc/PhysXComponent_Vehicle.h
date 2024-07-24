#pragma once

#include "PhysXComponent.h"




//�ش� Ŭ������ static ������Ʈ�� ���� ���� ��ŷ�� ���� Ŭ����

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_Vehicle final : public CPhysXComponent
{

private:
	explicit CPhysXComponent_Vehicle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_Vehicle(const CPhysXComponent_Vehicle& rhs);
	virtual ~CPhysXComponent_Vehicle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif

public:
	virtual void Tick(const _float fTimeDelta);
	virtual void Late_Tick(const _float fTimeDelta);

private:
	HRESULT CreateActor();
	
	void initMaterialFrictionTable();
private:
	CVehicleDefault* m_pVehicle = nullptr;
	PxVehiclePhysXSimulationContext m_pVehicleSimulationContext;

	PhysXActorVehicle m_pPhysXActorVehicle;

	

public:
	static CPhysXComponent_Vehicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END