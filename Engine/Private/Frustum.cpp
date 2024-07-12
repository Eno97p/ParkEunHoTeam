#include "..\Public\Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	_matrix ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	_float4	vPoints[8] = {};

	for (size_t i = 0; i < 8; i++)
	{
		XMStoreFloat4(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInverse));		
	}
	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&m_vWorldPoints[i], XMVector3Transform(XMLoadFloat4(&vPoints[i]), ViewMatrixInverse));

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);

	m_fTotalTime += m_pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	_float4		vLocalPoints[8] = {};

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat4(&vLocalPoints[i], XMVector3Transform(XMLoadFloat4(&m_vWorldPoints[i]), WorldMatrixInv));

	Make_Planes(vLocalPoints, m_vLocalPlanes);
}


_bool CFrustum::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		_vector	vResult = XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vPosition);
		_float	fResult = XMVectorGetX(vResult);
		if (fRange < fResult)
			return false;
	}
	return true;
}


_bool CFrustum::isIn_LocalFrustum(_fvector vPosition, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{		
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[i]), vPosition)))
			return false;
	}
	return true;
}

bool CFrustum::isVisible(_vector vPos, PxActor* actor)
{
    // 거리 기반 컬링
    _vector vCamPos = m_pGameInstance->Get_CamPosition();
    float distanceToCamera = XMVector3Length(XMVectorSubtract(vPos, vCamPos)).m128_f32[0];
    if (distanceToCamera > m_maxVisibleDistance)
    {
        return false;
    }

    // 바운딩 박스 정보 얻기
    PxBounds3 bounds = actor->getWorldBounds();
    PxVec3 center = bounds.getCenter();
    PxVec3 extents = bounds.getExtents();

    // 프러스텀 컬링
    std::vector<PxVec3> corners(8);
    corners[0] = center + PxVec3(-extents.x, -extents.y, -extents.z);
    corners[1] = center + PxVec3(extents.x, -extents.y, -extents.z);
    corners[2] = center + PxVec3(extents.x, extents.y, -extents.z);
    corners[3] = center + PxVec3(-extents.x, extents.y, -extents.z);
    corners[4] = center + PxVec3(-extents.x, -extents.y, extents.z);
    corners[5] = center + PxVec3(extents.x, -extents.y, extents.z);
    corners[6] = center + PxVec3(extents.x, extents.y, extents.z);
    corners[7] = center + PxVec3(-extents.x, extents.y, extents.z);

    _bool isInFrustum = false;

    //BOUNDING SPHERE
    //_float maxExt = max(max(extents.x, extents.y), extents.z);
    //if (isIn_WorldFrustum({ center.x, center.y, center.z, 1.f }, maxExt))
    //{
    //    isInFrustum = true;
    //}
    for (const auto& corner : corners)
    {
        _vector vCorner = XMVectorSet(corner.x, corner.y, corner.z, 1.0f);
        if (isIn_WorldFrustum(vCorner, 0.0f))
        {
            isInFrustum = true;
            break;
        }
    }

    if (!isInFrustum)
    {
        return false;
    }

    // 여기서부터 오클루전 컬링 시작
  // 바운딩 박스를 뷰 프로젝션 공간으로 변환
    _matrix viewProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) *
        m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    _vector vCenter = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(center.x, center.y, center.z)), viewProjMatrix);
    _vector vExtents = XMVector3TransformNormal(XMLoadFloat3(&XMFLOAT3(extents.x, extents.y, extents.z)), viewProjMatrix);

    // 화면 공간 좌표 계산
    float centerW = XMVectorGetW(vCenter);
    float centerX = XMVectorGetX(vCenter) / centerW;
    float centerY = XMVectorGetY(vCenter) / centerW;
    float centerZ = XMVectorGetZ(vCenter) / centerW;

    // 화면 공간에서의 바운딩 박스 크기 계산 (수정된 부분)
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    float extentX = XMVectorGetX(vExtents) / 3000.f;
    float extentY = XMVectorGetY(vExtents) / 3000.f;
    float boxWidth = extentX * screenWidth;
    float boxHeight = extentY * screenHeight;

    // HZB 밉맵 레벨 선택
    float boxSize = max(boxWidth, boxHeight);
    UINT mipLevel = static_cast<UINT>(min(16, max(0, log2(boxSize))));

    // 화면 공간 좌표를 UV 좌표로 변환
    _float2 uv;
    uv.x = min(1.f, (centerX + 1.0f) * 0.5f);
    uv.y = min(1.f, (1.0f - centerY) * 0.5f);

    // HZB 샘플링
    float hzbDepth = m_pGameInstance->Sample_HZB(uv, mipLevel);

    // 깊이 비교 (약간의 여유 값 추가)
    const float depthBias = 0.001f; // 필요에 따라 조정
    if (centerZ > hzbDepth + depthBias)
    {
        // 오클루드됨
        return false;
    }

    // 모든 테스트를 통과하면 가시적
    return true;



    //// 오클루전 컬링
    //PxVec3 cameraPosition(XMVectorGetX(vCamPos), XMVectorGetY(vCamPos), XMVectorGetZ(vCamPos));

    //// 적응형 샘플링: 거리에 따라 샘플 수 조절
    //int numSamples = (distanceToCamera < 100.f) ? 8 : (distanceToCamera < 500.0f) ? 4 : 1;

    //// 전략적 포인트 선택
    //std::vector<PxVec3> samplePoints;
    //samplePoints.push_back(center); // 중심점
    //if (numSamples > 1)
    //{
    //    // 바운딩 박스의 주요 축 방향으로 포인트 추가
    //    samplePoints.push_back(PxVec3(center.x + extents.x, center.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x - extents.x, center.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y + extents.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y - extents.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y, center.z + extents.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y, center.z - extents.z));
    //}

    //// 레이캐스트 수행
    //for (const auto& point : samplePoints)
    //{
    //    PxVec3 direction = point - cameraPosition;
    //    float distance = direction.magnitude();
    //    direction.normalize();

    //    PxRaycastBuffer hit;
    //    PxQueryFilterData filterData;
    //    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;

    //    bool isHit = m_pGameInstance->GetScene()->raycast(cameraPosition, direction, distance, hit, PxHitFlag::eDEFAULT, filterData);
    //    if (!isHit || hit.block.actor == actor)
    //    {
    //        // 하나라도 보이면 visible로 간주
    //        return true;
    //    }
    //}

    //// 모든 샘플 포인트가 가려져 있으면 not visible
    //return false;
}

float CFrustum::SampleHZB(ID3D11ShaderResourceView* pHZBSRV, float x, float y, UINT level)
{
    // 이 함수는 HZB를 샘플링하는 간단한 구현입니다.
    // 실제 구현에서는 GPU를 사용하여 더 효율적으로 샘플링해야 합니다.

    // 텍스처에서 깊이 값을 읽어오는 코드를 여기에 구현해야 합니다.
    // 이 예제에서는 간단히 고정된 값을 반환합니다.
    return 1.0f;
}


void CFrustum::Make_Planes(const _float4 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat4(&pPoints[7]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[0])));
	
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[1])));

	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat4(&pPoints[3]), XMLoadFloat4(&pPoints[2]), XMLoadFloat4(&pPoints[6])));

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat4(&pPoints[5]), XMLoadFloat4(&pPoints[4]), XMLoadFloat4(&pPoints[7])));

	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat4(&pPoints[0]), XMLoadFloat4(&pPoints[1]), XMLoadFloat4(&pPoints[2])));
}

CFrustum * CFrustum::Create()
{
	CFrustum*		pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);
}

