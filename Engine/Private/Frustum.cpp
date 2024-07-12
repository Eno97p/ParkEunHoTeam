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
    // �Ÿ� ��� �ø�
    _vector vCamPos = m_pGameInstance->Get_CamPosition();
    float distanceToCamera = XMVector3Length(XMVectorSubtract(vPos, vCamPos)).m128_f32[0];
    if (distanceToCamera > m_maxVisibleDistance)
    {
        return false;
    }

    // �ٿ�� �ڽ� ���� ���
    PxBounds3 bounds = actor->getWorldBounds();
    PxVec3 center = bounds.getCenter();
    PxVec3 extents = bounds.getExtents();

    // �������� �ø�
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

    // ���⼭���� ��Ŭ���� �ø� ����
  // �ٿ�� �ڽ��� �� �������� �������� ��ȯ
    _matrix viewProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) *
        m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);
    _vector vCenter = XMVector3TransformCoord(XMLoadFloat3(&XMFLOAT3(center.x, center.y, center.z)), viewProjMatrix);
    _vector vExtents = XMVector3TransformNormal(XMLoadFloat3(&XMFLOAT3(extents.x, extents.y, extents.z)), viewProjMatrix);

    // ȭ�� ���� ��ǥ ���
    float centerW = XMVectorGetW(vCenter);
    float centerX = XMVectorGetX(vCenter) / centerW;
    float centerY = XMVectorGetY(vCenter) / centerW;
    float centerZ = XMVectorGetZ(vCenter) / centerW;

    // ȭ�� ���������� �ٿ�� �ڽ� ũ�� ��� (������ �κ�)
    float screenWidth = 1280.0f;
    float screenHeight = 720.0f;
    float extentX = XMVectorGetX(vExtents) / 3000.f;
    float extentY = XMVectorGetY(vExtents) / 3000.f;
    float boxWidth = extentX * screenWidth;
    float boxHeight = extentY * screenHeight;

    // HZB �Ӹ� ���� ����
    float boxSize = max(boxWidth, boxHeight);
    UINT mipLevel = static_cast<UINT>(min(16, max(0, log2(boxSize))));

    // ȭ�� ���� ��ǥ�� UV ��ǥ�� ��ȯ
    _float2 uv;
    uv.x = min(1.f, (centerX + 1.0f) * 0.5f);
    uv.y = min(1.f, (1.0f - centerY) * 0.5f);

    // HZB ���ø�
    float hzbDepth = m_pGameInstance->Sample_HZB(uv, mipLevel);

    // ���� �� (�ణ�� ���� �� �߰�)
    const float depthBias = 0.001f; // �ʿ信 ���� ����
    if (centerZ > hzbDepth + depthBias)
    {
        // ��Ŭ����
        return false;
    }

    // ��� �׽�Ʈ�� ����ϸ� ������
    return true;



    //// ��Ŭ���� �ø�
    //PxVec3 cameraPosition(XMVectorGetX(vCamPos), XMVectorGetY(vCamPos), XMVectorGetZ(vCamPos));

    //// ������ ���ø�: �Ÿ��� ���� ���� �� ����
    //int numSamples = (distanceToCamera < 100.f) ? 8 : (distanceToCamera < 500.0f) ? 4 : 1;

    //// ������ ����Ʈ ����
    //std::vector<PxVec3> samplePoints;
    //samplePoints.push_back(center); // �߽���
    //if (numSamples > 1)
    //{
    //    // �ٿ�� �ڽ��� �ֿ� �� �������� ����Ʈ �߰�
    //    samplePoints.push_back(PxVec3(center.x + extents.x, center.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x - extents.x, center.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y + extents.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y - extents.y, center.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y, center.z + extents.z));
    //    samplePoints.push_back(PxVec3(center.x, center.y, center.z - extents.z));
    //}

    //// ����ĳ��Ʈ ����
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
    //        // �ϳ��� ���̸� visible�� ����
    //        return true;
    //    }
    //}

    //// ��� ���� ����Ʈ�� ������ ������ not visible
    //return false;
}

float CFrustum::SampleHZB(ID3D11ShaderResourceView* pHZBSRV, float x, float y, UINT level)
{
    // �� �Լ��� HZB�� ���ø��ϴ� ������ �����Դϴ�.
    // ���� ���������� GPU�� ����Ͽ� �� ȿ�������� ���ø��ؾ� �մϴ�.

    // �ؽ�ó���� ���� ���� �о���� �ڵ带 ���⿡ �����ؾ� �մϴ�.
    // �� ���������� ������ ������ ���� ��ȯ�մϴ�.
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

