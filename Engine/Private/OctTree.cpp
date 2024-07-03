#include "OctTree.h"
#include "GameInstance.h"

COctTree::COctTree()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT COctTree::Initialize(const _float3& vMin, const _float3& vMax)
{
    m_vMin = vMin;
    m_vMax = vMax;

    if ((m_vMax.x - m_vMin.x) <= 1.0f)
        return S_OK;

    Float3Add add;
    Float3Multiply mul;
    _float3 vCenter = mul(add(m_vMin, m_vMax), 0.5f);

    m_pChildren[TOP_LEFT_FRONT] = COctTree::Create(
        _float3(m_vMin.x, vCenter.y, vCenter.z),
        _float3(vCenter.x, m_vMax.y, m_vMax.z));

    m_pChildren[TOP_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, vCenter.y, vCenter.z), _float3(vCenter.x, m_vMax.y, m_vMax.z));
    m_pChildren[TOP_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, vCenter.y, vCenter.z), _float3(m_vMax.x, m_vMax.y, m_vMax.z));
    m_pChildren[BOTTOM_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, m_vMin.y, vCenter.z), _float3(m_vMax.x, vCenter.y, m_vMax.z));
    m_pChildren[BOTTOM_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, vCenter.z), _float3(vCenter.x, vCenter.y, m_vMax.z));
    m_pChildren[TOP_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, vCenter.y, m_vMin.z), _float3(vCenter.x, m_vMax.y, vCenter.z));
    m_pChildren[TOP_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, vCenter.y, m_vMin.z), _float3(m_vMax.x, m_vMax.y, vCenter.z));
    m_pChildren[BOTTOM_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, m_vMin.y, m_vMin.z), _float3(m_vMax.x, vCenter.y, vCenter.z));
    m_pChildren[BOTTOM_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, m_vMin.z), _float3(vCenter.x, vCenter.y, vCenter.z));

    return S_OK;
}

void COctTree::Culling(const _float4* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
    // 현재 노드가 리프 노드이거나 충분히 작아서 더 이상 분할할 필요가 없는 경우
    if (nullptr == m_pChildren[TOP_LEFT_FRONT] || isDraw(pVertexPositions))
    {
        _bool isDraw[NEIGHBOR_END] = { true, true, true, true };

        // 이웃 노드들의 그리기 여부 확인
        for (size_t i = 0; i < NEIGHBOR_END; i++)
        {
            if (nullptr != m_pNeighbors[i])
                isDraw[i] = m_pNeighbors[i]->isDraw(pVertexPositions);
        }

        // 현재 노드의 8개 코너 점들이 프러스텀 내에 있는지 확인
        _bool isIn[8] = {
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[TOP_LEFT_FRONT]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[TOP_RIGHT_FRONT]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[BOTTOM_RIGHT_FRONT]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[BOTTOM_LEFT_FRONT]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[TOP_LEFT_BACK]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[TOP_RIGHT_BACK]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[BOTTOM_RIGHT_BACK]])),
            m_pGameInstance->isIn_LocalFrustum(XMLoadFloat4(&pVertexPositions[m_iCorners[BOTTOM_LEFT_BACK]]))
        };

        // 모든 이웃들이 그려져야 하는 경우
        if (isDraw[NEIGHBOR_LEFT] && isDraw[NEIGHBOR_RIGHT] && isDraw[NEIGHBOR_TOP] &&
            isDraw[NEIGHBOR_BOTTOM] && isDraw[NEIGHBOR_FRONT] && isDraw[NEIGHBOR_BACK])
        {
            // 6개의 면에 대해 삼각형을 생성
            // Front face
            if (isIn[TOP_LEFT_FRONT] || isIn[TOP_RIGHT_FRONT] || isIn[BOTTOM_RIGHT_FRONT] || isIn[BOTTOM_LEFT_FRONT])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_FRONT];

                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_FRONT];
            }

            // Back face
            if (isIn[TOP_LEFT_BACK] || isIn[TOP_RIGHT_BACK] || isIn[BOTTOM_RIGHT_BACK] || isIn[BOTTOM_LEFT_BACK])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_BACK];

                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];
            }

            // Left face
            if (isIn[TOP_LEFT_FRONT] || isIn[TOP_LEFT_BACK] || isIn[BOTTOM_LEFT_BACK] || isIn[BOTTOM_LEFT_FRONT])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_BACK];

                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_BACK];
            }

            // Right face
            if (isIn[TOP_RIGHT_FRONT] || isIn[TOP_RIGHT_BACK] || isIn[BOTTOM_RIGHT_BACK] || isIn[BOTTOM_RIGHT_FRONT])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];

                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_FRONT];
            }

            // Top face
            if (isIn[TOP_LEFT_FRONT] || isIn[TOP_RIGHT_FRONT] || isIn[TOP_RIGHT_BACK] || isIn[TOP_LEFT_BACK])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_FRONT];

                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_LEFT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[TOP_RIGHT_BACK];
            }

            // Bottom face
            if (isIn[BOTTOM_LEFT_FRONT] || isIn[BOTTOM_RIGHT_FRONT] || isIn[BOTTOM_RIGHT_BACK] || isIn[BOTTOM_LEFT_BACK])
            {
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];

                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_FRONT];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_RIGHT_BACK];
                pIndices[(*pNumIndices)++] = m_iCorners[BOTTOM_LEFT_BACK];
            }
        }
        else
        {
            // 이웃 노드들과의 연결을 고려한 더 복잡한 삼각형 생성 로직
            // (이 부분은 프로젝트의 구체적인 요구사항에 따라 구현해야 합니다)
        }

        return;
    }

    // 자식 노드들에 대한 컬링 수행
   // _float3 vCenter = (_float3(pVertexPositions[m_iCorners[TOP_LEFT_FRONT]]) + _float3(pVertexPositions[m_iCorners[BOTTOM_RIGHT_BACK]])) * 0.5f;
    //_float fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_float3(pVertexPositions[m_iCorners[TOP_LEFT_FRONT]])) - XMLoadFloat3(&vCenter)));

 /*   if (m_pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&vCenter), fRadius))
    {
        for (size_t i = 0; i < CORNER_END; i++)
        {
            m_pChildren[i]->Culling(pVertexPositions, pIndices, pNumIndices);
        }
    }*/
}


void COctTree::Make_Neighbors()
{
    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
        return;

    // X축 이웃 설정
    m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[TOP_RIGHT_FRONT];
    m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[TOP_LEFT_FRONT];
    m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[BOTTOM_RIGHT_FRONT];
    m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[BOTTOM_LEFT_FRONT];
    m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[TOP_RIGHT_BACK];
    m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[TOP_LEFT_BACK];
    m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[BOTTOM_RIGHT_BACK];
    m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[BOTTOM_LEFT_BACK];

    // Y축 이웃 설정
    m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[BOTTOM_LEFT_FRONT];
    m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[TOP_LEFT_FRONT];
    m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[BOTTOM_RIGHT_FRONT];
    m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[TOP_RIGHT_FRONT];
    m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[BOTTOM_LEFT_BACK];
    m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[TOP_LEFT_BACK];
    m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[BOTTOM_RIGHT_BACK];
    m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[TOP_RIGHT_BACK];

    // Z축 이웃 설정
    m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_BACK] = m_pChildren[TOP_LEFT_BACK];
    m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_FRONT] = m_pChildren[TOP_LEFT_FRONT];
    m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_BACK] = m_pChildren[TOP_RIGHT_BACK];
    m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_FRONT] = m_pChildren[TOP_RIGHT_FRONT];
    m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_BACK] = m_pChildren[BOTTOM_LEFT_BACK];
    m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_FRONT] = m_pChildren[BOTTOM_LEFT_FRONT];
    m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_BACK] = m_pChildren[BOTTOM_RIGHT_BACK];
    m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_FRONT] = m_pChildren[BOTTOM_RIGHT_FRONT];

    // RIGHT 이웃
    if (m_pNeighbors[NEIGHBOR_RIGHT])
    {
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[BOTTOM_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[BOTTOM_LEFT_BACK];
    }

    // LEFT 이웃
    if (m_pNeighbors[NEIGHBOR_LEFT])
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[BOTTOM_RIGHT_FRONT];
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[TOP_RIGHT_BACK];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[BOTTOM_RIGHT_BACK];
    }

    // TOP 이웃
    if (m_pNeighbors[NEIGHBOR_TOP])
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_RIGHT_FRONT];
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_LEFT_BACK];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[BOTTOM_RIGHT_BACK];
    }

    // BOTTOM 이웃
    if (m_pNeighbors[NEIGHBOR_BOTTOM])
    {
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[TOP_RIGHT_BACK];
    }

    // FRONT 이웃
    if (m_pNeighbors[NEIGHBOR_FRONT])
    {
        m_pChildren[TOP_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[TOP_LEFT_BACK];
        m_pChildren[TOP_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[TOP_RIGHT_BACK];
        m_pChildren[BOTTOM_LEFT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[BOTTOM_LEFT_BACK];
        m_pChildren[BOTTOM_RIGHT_FRONT]->m_pNeighbors[NEIGHBOR_FRONT] = m_pNeighbors[NEIGHBOR_FRONT]->m_pChildren[BOTTOM_RIGHT_BACK];
    }

    // BACK 이웃
    if (m_pNeighbors[NEIGHBOR_BACK])
    {
        m_pChildren[TOP_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[TOP_LEFT_FRONT];
        m_pChildren[TOP_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[TOP_RIGHT_FRONT];
        m_pChildren[BOTTOM_LEFT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[BOTTOM_LEFT_FRONT];
        m_pChildren[BOTTOM_RIGHT_BACK]->m_pNeighbors[NEIGHBOR_BACK] = m_pNeighbors[NEIGHBOR_BACK]->m_pChildren[BOTTOM_RIGHT_FRONT];
    }

    // 다른 외부 이웃들에 대해서도 유사하게 설정 (LEFT, TOP, BOTTOM, FRONT, BACK)

    // 자식 노드들에 대해 재귀적으로 이웃 설정
    for (size_t i = 0; i < CORNER_END; i++)
        m_pChildren[i]->Make_Neighbors();
}

_bool COctTree::isDraw(const _float4* pVertexPositions)
{
    _vector vCamPosition = m_pGameInstance->Get_CamPosition();

    Float3Add add;
    Float3Multiply mul;
    _float3 vCenter = mul(add(m_vMin, m_vMax), 0.5f);
    float fDistance = XMVectorGetX(XMVector3Length(vCamPosition - XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&vCenter))));

    float fNodeSize = m_vMax.x - m_vMin.x;

    if (fDistance * 0.1f > fNodeSize)
        return true;

    return false;
}

COctTree* COctTree::Create(const _float3& vMin, const _float3& vMax)
{
    COctTree* pInstance = new COctTree();

    if (FAILED(pInstance->Initialize(vMin, vMax)))
    {
        MSG_BOX("Failed To Created : COctTree");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COctTree::Free()
{
    for (size_t i = 0; i < CORNER_END; i++)
        Safe_Release(m_pChildren[i]);

    Safe_Release(m_pGameInstance);
}