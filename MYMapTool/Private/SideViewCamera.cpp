#include "..\Public\SideViewCamera.h"
#include "GameInstance.h"
#include "Player.h"
#include "Monster.h"

#include "ToolObj.h"

CSideViewCamera::CSideViewCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CSideViewCamera::CSideViewCamera(const CSideViewCamera& rhs)
    : CCamera{ rhs }
{
}

HRESULT CSideViewCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSideViewCamera::Initialize(void* pArg)
{
    SIDEVIEWCAMERA_DESC* pDesc = (SIDEVIEWCAMERA_DESC*)pArg;
    m_fSensor = pDesc->fSensor;
    m_pPlayerTrans = pDesc->pPlayerTrans;
    Safe_AddRef(m_pPlayerTrans);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    __super::Activate();

    // �ʱ� ī�޶� ��ġ ����
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vInitialCameraPosition, vInitialLookAtPosition;
    Get_SideViewCamPos(vPlayerPosition, &vInitialCameraPosition, &vInitialLookAtPosition);

    m_vCameraPosition = vInitialCameraPosition;
    m_vLookAtPosition = vInitialLookAtPosition;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    return S_OK;
}

void CSideViewCamera::Priority_Tick(_float fTimeDelta)
{
    // Priority_Tick �Լ� ������ �״�� ����
}

void CSideViewCamera::Tick(_float fTimeDelta)
{
    if (m_pPlayerTrans == nullptr)
        return;

    //// �÷��̾� ��ġ ���
    //_vector vPlayerPos = m_pPlayerTrans->Get_State(CTransform::STATE_POSITION);
    //_vector vCamPos = m_pGameInstance->Get_CamPosition();

    // �� ������Ʈ ���� �ڵ�� �״�� ����

    {
        Update_SideViewCam(fTimeDelta);
    }

    // ����ŷ ���� �ڵ�� �״�� ����

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vCameraPosition));
    m_pTransformCom->LookAt(XMLoadFloat4(&m_vLookAtPosition));

    __super::Tick(fTimeDelta);
}

void CSideViewCamera::Late_Tick(_float fTimeDelta)
{
    if (!m_bCamActivated) return;
    Key_Input(fTimeDelta);
}

HRESULT CSideViewCamera::Render()
{
    return S_OK;
}

void CSideViewCamera::Get_SideViewCamPos(const _float4& vPlayerPosition, _float4* pOutCameraPosition, _float4* pOutLookAtPosition)
{
    // ī�޶� ��ġ ���
    pOutCameraPosition->x = vPlayerPosition.x;
    pOutCameraPosition->y = vPlayerPosition.y + m_fHeightOffset;
    pOutCameraPosition->z = vPlayerPosition.z - m_fFixedZPosition; // �÷��̾�� Z������ 10��ŭ �տ� ��ġ
    pOutCameraPosition->w = 1.f;

    // ī�޶� �ٶ󺸴� ��ġ ��� (�÷��̾� ��ġ)

    *pOutLookAtPosition = vPlayerPosition;
    pOutLookAtPosition->y = vPlayerPosition.y + m_fHeightOffset * 0.5f;
    pOutLookAtPosition->w = 1.f;
}




void CSideViewCamera::Update_SideViewCam(_float fTimeDelta)
{
    _float4 vPlayerPosition;
    XMStoreFloat4(&vPlayerPosition, m_pPlayerTrans->Get_State(CTransform::STATE_POSITION));

    _float4 vTargetCameraPosition, vTargetLookAtPosition;
    Get_SideViewCamPos(vPlayerPosition, &vTargetCameraPosition, &vTargetLookAtPosition);

    // �ε巯�� ī�޶� �̵��� ���� ����
    XMStoreFloat4(&m_vCameraPosition, XMVectorLerp(
        XMLoadFloat4(&m_vCameraPosition),
        XMLoadFloat4(&vTargetCameraPosition),
        1.0f - exp(-m_fFollowSpeed * fTimeDelta)
    ));

    XMStoreFloat4(&m_vLookAtPosition, XMVectorLerp(
        XMLoadFloat4(&m_vLookAtPosition),
        XMLoadFloat4(&vTargetLookAtPosition),
        1.0f - exp(-m_fFollowSpeed * fTimeDelta)
    ));
}

void CSideViewCamera::Update_TransitionCam(_float fTimeDelta)
{
    // Update_TransitionCam �Լ� ������ �״�� ����
}

void CSideViewCamera::Shaking()
{
    // Shaking �Լ� ������ �״�� ����
}

void CSideViewCamera::Key_Input(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_TAB))
    {
        m_pGameInstance->Set_MainCamera(0);
    }
    // ȸ�� �� �Ÿ� ���� ���� �ڵ�� ���ŵǾ����ϴ�.
}

CSideViewCamera* CSideViewCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSideViewCamera* pInstance = new CSideViewCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CSideViewCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSideViewCamera::Clone(void* pArg)
{
    CSideViewCamera* pInstance = new CSideViewCamera(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CSideViewCamera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSideViewCamera::Free()
{
    Safe_Release(m_pPlayerTrans);

    __super::Free();
}