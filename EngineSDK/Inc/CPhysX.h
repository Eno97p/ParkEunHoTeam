#pragma once
#include"Base.h"




BEGIN(Engine)
class CPhysX final : public CBase
{


private:
	CPhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPhysX() = default;



public:
	HRESULT Initialize();


	void Tick(_float fTimeDelta);


public:
	HRESULT AddActor(PxActor* pActor);

public:
	PxPhysics* GetPhysics()const  { return m_pPhysics; }
	PxScene* GetScene()const { return m_pScene; }
	PxControllerManager* GetControllerManager()const { return m_pControllerManager; }

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };



private:
	PxDefaultAllocator		m_Allocator;
	PxDefaultErrorCallback	m_ErrorCallback;
	PxMaterial*				m_pMaterial = { nullptr };
	PxFoundation*			m_pFoundation = { nullptr };
	PxPhysics*				m_pPhysics = { nullptr };
	PxDefaultCpuDispatcher*	m_pCPUDispatcher = { nullptr };
	PxScene*				m_pScene = { nullptr };
	PxPvd*					m_pPvd = { nullptr };
	PxCudaContextManager*	m_pCudaContextManager = { nullptr };
	PxControllerManager*	m_pControllerManager = { nullptr };


public:
	static CPhysX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

template<typename T>
void Safe_physX_Release(T& pInstance)
{

	if (pInstance)
	{
		pInstance->release();
		pInstance = nullptr;
	}

	

}

END
