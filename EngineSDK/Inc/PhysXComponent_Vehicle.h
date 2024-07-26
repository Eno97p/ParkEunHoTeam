#pragma once

#include "PhysXComponent.h"




//�ش� Ŭ������ static ������Ʈ�� ���� ���� ��ŷ�� ���� Ŭ����

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_Vehicle final : public CPhysXComponent
{
public:
	/* �ڵ� �극��ũ �̻��*/
	struct  VEHICLE_COMMAND
	{
		_float4x4 initTransform;

		PxTransform transform;

		/*  0~1���̿��� �� 
		0�� ������ �� ���� ����
		1�� ������ ���� ����*/
		PxReal throttle;	

		/*  -1~1���̿��� ��
		-1�� �������� ������ ���� ����
		1�� ���������� ������ ���� ����
		*/
		PxReal steer;

	
		/*  0~1���̿��� ��
		0�� ������ �� ���� ����
		1�� ������ ���� ����
		*/
		PxReal brake;

		/* ��� ����*/
		PxVehicleDirectDriveTransmissionCommandState::Enum gear = PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL;

		void Reset()
		{
			throttle = 0.0f;
			steer = 0.0f;
			brake = 0.0f;
			gear = PxVehicleDirectDriveTransmissionCommandState::Enum::eNEUTRAL;
		}
	};


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


	void GetCommand(VEHICLE_COMMAND*& const command) { command = &m_Command; }
private:
	HRESULT CreateActor();
	
	void initMaterialFrictionTable();
private:
	//CVehicleDefault* m_pVehicle = nullptr;
	PxVehiclePhysXSimulationContext m_pVehicleSimulationContext;

	PhysXActorVehicle* m_pPhysXActorVehicle = nullptr;

	VEHICLE_COMMAND m_Command;
	
public:
	static CPhysXComponent_Vehicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END