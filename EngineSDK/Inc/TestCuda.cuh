#pragma once
#include"CudaDefines.h"

__global__ void cullingKernel(Cu_VTXMATRIX* instances, int numInstances, float3 cameraPos, float maxDistanceSquared, int* visibleCount);


cudaError_t LaunchCullingKernel(Cu_VTXMATRIX* d_instanceData, int numInstances, float3 cameraPos, float maxDistanceSquared, int* d_visibleCount);