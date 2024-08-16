#include "CCuda.h"
#include"TestCuda.cuh"



CCuda::CCuda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}


HRESULT CCuda::Initialize()
{

	int iDeviceCount = 0;
	cudaError_t cudaStatus = cudaGetDeviceCount(&iDeviceCount);
	if(cudaStatus != cudaSuccess || iDeviceCount == 0)		//PCIe에 그래픽카드가 장착되어 있지 않다면
	{
		MSG_BOX("cudaGetDeviceCount failed!");
		return E_FAIL;
	}

	cudaDeviceProp prop;

	for(int i = 0; i < iDeviceCount; ++i)		//현재 장착되어 있는 그래픽카드의 정보를 가져옴 
	{
		if (cudaSuccess != cudaGetDeviceProperties(&prop, i))
		{
			__debugbreak();
		}
		
	}
	if(cudaSuccess != cudaSetDevice(0))		//0번째 그래픽카드를 사용할 거임 
	{
		MSG_BOX("cudaSetDevice failed!");
		return E_FAIL;
	}

	/*cudaStatus = cudaStreamCreate(&m_cudaStream);
	if (cudaStatus != cudaSuccess)
	{
		MSG_BOX("Failed to create CUDA stream");
		return E_FAIL;
	}*/

	
	
	return S_OK;
}

HRESULT CCuda::LaunchKernel_CullingInstance(VTXMATRIX* d_instanceData, int numInstances, float3 cameraPos, float maxRenderDistance, int* d_visibleCount)
{
	float maxDistanceSquared = maxRenderDistance * maxRenderDistance;
	Cu_VTXMATRIX* cu_instanceData = reinterpret_cast<Cu_VTXMATRIX*>(d_instanceData);
	cudaError_t cudaStatus = LaunchCullingKernel(cu_instanceData, numInstances, cameraPos, maxDistanceSquared, d_visibleCount);

	if (cudaStatus != cudaSuccess)
	{
		// 오류 처리
		return E_FAIL;
	}

	cudaDeviceSynchronize();
	return S_OK;
}




CCuda* CCuda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCuda* pInstance = new CCuda(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CCuda");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCuda::Free()
{

	cudaDeviceReset();
	

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

HRESULT CCuda::InitCuda()
{

	int iDeviceCount = 0;
	cudaError_t cudaStatus = cudaGetDeviceCount(&iDeviceCount);
	if (cudaStatus != cudaSuccess || iDeviceCount == 0)		//PCIe에 그래픽카드가 장착되어 있지 않다면
	{
		MSG_BOX("cudaGetDeviceCount failed!");
		return E_FAIL;
	}

	cudaDeviceProp prop;

	for (int i = 0; i < iDeviceCount; ++i)		//현재 장착되어 있는 그래픽카드의 정보를 가져옴 
	{
		if (cudaSuccess != cudaGetDeviceProperties(&prop, i))
		{
			__debugbreak();
		}

	}
	if (cudaSuccess != cudaSetDevice(0))		//0번째 그래픽카드를 사용할 거임 
	{
		MSG_BOX("cudaSetDevice failed!");
		return E_FAIL;
	}


	return S_OK;
}
