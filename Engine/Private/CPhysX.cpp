#include "CPhysX.h"

#include"CSimulationCallBack.h"
#include "CHitReport.h"
#include"foundation/PxThread.h"

#include"CUserErrorCallBack.h"


PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
CSimulationCallBack g_SimulationCallBack;
CUserErrorCallBack g_ErrorCallBack;


CPhysX::CPhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	
}



HRESULT CPhysX::Initialize()
{
	m_pFoundation= PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, g_ErrorCallBack);
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
	if (!m_pPhysics)
	{
		MSG_BOX("Failed To Create : Physx_Physics");
		return E_FAIL;
	}
	
	







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
	PxU32 numCores = PxThread::getNbPhysicalCores();
	m_pCPUDispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
	if(!m_pCPUDispatcher)
	{
		MSG_BOX("Failed To Create : Physx_CPUDispatcher");
		return E_FAIL;
	}
	sceneDesc.cpuDispatcher = m_pCPUDispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = FilterShaderExample;
	sceneDesc.simulationEventCallback = &g_SimulationCallBack;

	sceneDesc.cudaContextManager = m_pCudaContextManager;


	//sceneDesc.flags |= PxSceneFlag::eENABLE_DIRECT_GPU_API;			//https://nvidia-omniverse.github.io/PhysX/physx/5.4.0/docs/DirectGPUAPI.html#direct-gpu-api-limitations
	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	
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

#ifdef _DEBUG
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
	m_pScene->getVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES);
#endif // _DEBUG
	
	//m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	m_pControllerManager = PxCreateControllerManager(*m_pScene);
	if (!m_pControllerManager)
	{
		MSG_BOX("Failed To Create : Physx_ControllerManager");
		return E_FAIL;
	}


	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.f);
	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *m_pMaterial);
	m_pScene->addActor(*groundPlane);


	if (!PxInitExtensions(*m_pPhysics, m_pPvd))
	{
		MSG_BOX("PxInitExtensions failed!");
		return E_FAIL;
	
	}

	PxInitVehicleExtension(*m_pFoundation);


	return S_OK;
}

void CPhysX::Tick(_float fTimeDelta)
{
	m_pScene->simulate(fTimeDelta);


	PxU32 errorState = 0;

	bool resultFetched = m_pScene->fetchResults(true,&errorState);
	if (!resultFetched)
	{
		MSG_BOX("Failed To Fetch Result");
		return;
	}

	PxSimulationStatistics stats;
	m_pScene->getSimulationStatistics(stats);

	PxU32 temp = stats.nbActiveDynamicBodies;
	PxU32 temp2 = stats.nbActiveKinematicBodies;
	PxU32 temp3 = stats.nbStaticBodies;

	
	
}

HRESULT CPhysX::AddActor(PxActor* pActor)
{
	if(!m_pScene->addActor(*pActor))
		return E_FAIL;
	
	return S_OK;
}

PxFilterFlags CPhysX::FilterShaderExample(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{

	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);



	if ((filterData0.word0 & GROUP_PLAYER && filterData1.word0 & GROUP_WEAPON) ||
		(filterData0.word0 & GROUP_WEAPON && filterData1.word0 & GROUP_PLAYER))
	{
		return PxFilterFlag::eSUPPRESS; // 충돌 억제
	}



	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;
	return PxFilterFlag::eDEFAULT;



	//if ((filterData0.word0 & filterData1.word1) & (Engine::CollisionGropuID::GROUP_PLAYER | Engine::CollisionGropuID::GROUP_WEAPON))
	//{
	//	return PxFilterFlag::eSUPPRESS; // 충돌 억제
	//}
	//
	//// 기본적인 충돌 설정 (필요에 따라 추가적인 설정 가능)
	//pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	//
	//return PxFilterFlag::eDEFAULT;
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
	PxCloseExtensions();
	PxCloseVehicleExtension();



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


	CHitReport::DestroyInstance();
}
