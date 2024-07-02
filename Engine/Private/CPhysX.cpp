#include "CPhysX.h"

CPhysX::CPhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	
}



HRESULT CPhysX::Initialize()
{
	m_pFoundation= PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
	if (!m_pFoundation)
	{
		MSG_BOX("Failed To Create : Physx_Foundation");
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif // _DEBUG
	


 

	PxTolerancesScale scale;
	scale.length = WORLD_METER;
	scale.speed = WORLD_SPEED;
	m_pPhysics= PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, scale, true, m_pPvd == nullptr ? 0 : m_pPvd);
	if(!m_pPhysics)
		return E_FAIL;

	PxCudaContextManagerDesc cudaContextManagerDesc;

	m_pCudaContextManager= PxCreateCudaContextManager(*m_pFoundation, cudaContextManagerDesc, PxGetProfilerCallback());	//Create the CUDA context manager, required for GRB to dispatch CUDA kernels.
	if(m_pCudaContextManager)
	{
		if(!m_pCudaContextManager->contextIsValid())
		{
			m_pCudaContextManager->release();
			m_pCudaContextManager = NULL;
			MSG_BOX("Failed To Create : Physx_CudaContextManager");
			return E_FAIL;
		}
	}
	//const char* deviceName;
	//deviceName = m_pCudaContextManager->getDeviceName();
	//int iDeviceVersion = m_pCudaContextManager->getDriverVersion();




	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_pCPUDispatcher = PxDefaultCpuDispatcherCreate(6);
	sceneDesc.cpuDispatcher = m_pCPUDispatcher;
	sceneDesc.filterShader = FilterShaderExample;
	
	sceneDesc.cudaContextManager = m_pCudaContextManager;
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;


	m_pScene = m_pPhysics->createScene(sceneDesc);
	if (!m_pScene)
	{
		MSG_BOX("Failed To Create : Physx_Scene");
		return E_FAIL;
	}



	PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, false);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, false);
	}



	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	m_pScene->getVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES);

	//m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	if (!m_pControllerManager)
	{
		MSG_BOX("Failed To Create : Physx_ControllerManager");
		return E_FAIL;
	}
	

	


	return S_OK;
}

void CPhysX::Tick(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);
	m_pScene->fetchResults(true);



}

HRESULT CPhysX::AddActor(PxActor* pActor)
{
	if(m_pScene->addActor(*pActor))
		return E_FAIL;
	
	return S_OK;
}

PxFilterFlags CPhysX::FilterShaderExample(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eSUPPRESS;
}



CPhysX* CPhysX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	CPhysX* pInstance = new CPhysX(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CPhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysX::Free()
{
	Safe_physX_Release(m_pControllerManager);
	Safe_physX_Release(m_pScene);
	Safe_physX_Release(m_pCPUDispatcher);
	Safe_physX_Release(m_pPhysics);


	if (m_pPvd)
	{
			PxPvdTransport* transport = m_pPvd->getTransport();
			m_pPvd->release();
			m_pPvd = nullptr;
			transport->release();
		
	}

	Safe_physX_Release(m_pCudaContextManager);
	Safe_physX_Release(m_pFoundation);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
