#include "OctTree.h"
#include "GameInstance.h"

COctTree::COctTree()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT COctTree::Initialize(const _float3& vMin, const _float3& vMax, _int depth)
{
    const int MAX_DEPTH = 4; // 적절한 값으로 조정
    if (depth >= MAX_DEPTH)
        return S_OK;

    m_vMin = vMin;
    m_vMax = vMax;

    if ((m_vMax.x - m_vMin.x) <= 1.0f)
        return S_OK;

    Float3Add add;
    Float3Multiply mul;
    _float3 vCenter = mul(add(m_vMin, m_vMax), 0.5f);

    m_pChildren[TOP_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, vCenter.y, vCenter.z), _float3(vCenter.x, m_vMax.y, m_vMax.z), depth + 1);
    m_pChildren[TOP_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, vCenter.y, vCenter.z), _float3(m_vMax.x, m_vMax.y, m_vMax.z), depth + 1);
    m_pChildren[BOTTOM_RIGHT_FRONT] = COctTree::Create(_float3(vCenter.x, m_vMin.y, vCenter.z), _float3(m_vMax.x, vCenter.y, m_vMax.z), depth + 1);
    m_pChildren[BOTTOM_LEFT_FRONT] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, vCenter.z), _float3(vCenter.x, vCenter.y, m_vMax.z), depth + 1);
    m_pChildren[TOP_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, vCenter.y, m_vMin.z), _float3(vCenter.x, m_vMax.y, vCenter.z), depth + 1);
    m_pChildren[TOP_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, vCenter.y, m_vMin.z), _float3(m_vMax.x, m_vMax.y, vCenter.z), depth + 1);
    m_pChildren[BOTTOM_RIGHT_BACK] = COctTree::Create(_float3(vCenter.x, m_vMin.y, m_vMin.z), _float3(m_vMax.x, vCenter.y, vCenter.z), depth + 1);
    m_pChildren[BOTTOM_LEFT_BACK] = COctTree::Create(_float3(m_vMin.x, m_vMin.y, m_vMin.z), _float3(vCenter.x, vCenter.y, vCenter.z), depth + 1);

    return S_OK;
}

void COctTree::Update_OctTree()
{
    const _float4 cameraPosition = *m_pGameInstance->Get_CamPosition_float4();
    m_CamPos = { cameraPosition.x, cameraPosition.y, cameraPosition.z };

    UpdateNodeVisibility();
}

void COctTree::UpdateNodeVisibility()
{
    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
    {
        return;
    }

    for (int i = 0; i < 8; ++i)
    {
        m_pChildren[i]->UpdateNodeVisibility();
    }
}

void COctTree::AddObject(CGameObject* pObject, PxActor* pActor)
{
    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
    {
        m_Objects.push_back(pObject);
        Safe_AddRef(pObject);
        return;
    }

    // PhysX Actor에서 바운딩 박스 정보 가져오기
    physx::PxBounds3 bounds = pActor->getWorldBounds();
    _float3 objMin = _float3(bounds.minimum.x, bounds.minimum.y, bounds.minimum.z);
    _float3 objMax = _float3(bounds.maximum.x, bounds.maximum.y, bounds.maximum.z);

    // 바운딩 박스가 현재 노드의 영역을 완전히 포함하는 경우, 현재 노드에 객체 추가
    if (objMin.x >= m_vMin.x && objMin.y >= m_vMin.y && objMin.z >= m_vMin.z &&
        objMax.x <= m_vMax.x && objMax.y <= m_vMax.y && objMax.z <= m_vMax.z)
    {
        m_Objects.push_back(pObject);
        Safe_AddRef(pObject);
        return;
    }

    // 바운딩 박스와 겹치는 자식 노드에만 객체 추가
    for (int i = 0; i < 8; ++i)
    {
        if (m_pChildren[i]->IsOverlapping(objMin, objMax))
        {
            m_pChildren[i]->AddObject(pObject, pActor);
        }
    }
}

bool COctTree::IsOverlapping(const _float3& min, const _float3& max)
{
    return (min.x <= m_vMax.x && max.x >= m_vMin.x &&
        min.y <= m_vMax.y && max.y >= m_vMin.y &&
        min.z <= m_vMax.z && max.z >= m_vMin.z);
}

bool COctTree::IsObjectVisible(CGameObject* pObject)
{
    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
    {
        for (auto* obj : m_Objects)
        {
            if (obj == pObject)
            {
                return IsNodeVisible();
            }
        }
        return false;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (m_pChildren[i]->IsObjectVisible(pObject))
        {
            return true;
        }
    }

    return false;
}

bool COctTree::IsNodeVisible()
{
    if (IsNodeOccluded())
        return false;

    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
    {
        return true;
    }

    for (int i = 0; i < 8; ++i)
    {
        if (m_pChildren[i]->IsNodeVisible())
            return true;
    }

    return false;
}

bool COctTree::IsNodeOccluded()
{
    if (nullptr == m_pChildren[TOP_LEFT_FRONT])
    {
        PxVec3 nodeCenter((m_vMin.x + m_vMax.x) * 0.5f, (m_vMin.y + m_vMax.y) * 0.5f, (m_vMin.z + m_vMax.z) * 0.5f);
        PxVec3 direction = nodeCenter - m_CamPos;
        float distance = direction.magnitude();
        direction.normalize();

        PxRaycastBuffer hit;
        PxQueryFilterData filterData;
        filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;

        bool isHit = m_pGameInstance->GetScene()->raycast(m_CamPos, direction, distance, hit, PxHitFlag::eDEFAULT, filterData);
        return isHit;
    }

    bool isOccluded = true;
    for (int i = 0; i < 8; ++i)
    {
        if (!m_pChildren[i]->IsNodeOccluded())
        {
            isOccluded = false;
            break;
        }
    }
    return isOccluded;
}

COctTree* COctTree::Create(const _float3& vMin, const _float3& vMax, _int depth)
{
    COctTree* pInstance = new COctTree();

    if (FAILED(pInstance->Initialize(vMin, vMax, depth)))
    {
        MSG_BOX("Failed To Created : COctTree");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COctTree::Free()
{
    for (auto& obj : m_Objects)
    {
        Safe_Release(obj);
    }
    m_Objects.clear();

    for (size_t i = 0; i < CORNER_END; i++)
        Safe_Release(m_pChildren[i]);

    Safe_Release(m_pGameInstance);
}