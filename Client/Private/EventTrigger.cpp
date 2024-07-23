#include "EventTrigger.h"
#include "Map_Element.h"
#include "GameInstance.h"
#include "Player.h"
#include "Elevator.h"
#include "Level_Loading.h"

#include "SideViewCamera.h"
#include "TransitionCamera.h"
#include "UI_FadeInOut.h"
#include "LandObject.h"

CEventTrigger::CEventTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMap_Element(pDevice, pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
	: CMap_Element(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventTrigger::Initialize(void* pArg)
{


	if (FAILED(CGameObject::Initialize(nullptr)))
		return E_FAIL;

	if (nullptr != pArg)
	{

		CMap_Element::MAP_ELEMENT_DESC* pDesc = (CMap_Element::MAP_ELEMENT_DESC*)pArg;

		//_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&pDesc->mWorldMatrix));
		//m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

		_vector vPos = XMLoadFloat4x4(&pDesc->mWorldMatrix).r[3];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		//TRIGGER STATE SET
		m_eTRIGState = (TRIGGER_TYPE)pDesc->TriggerType;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CEventTrigger::Priority_Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CEventTrigger::Tick(_float fTimeDelta)
{

	

//	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CEventTrigger::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Collider")));
	if (nullptr != pPlayerCollider)
	{
		if (CCollider::COLL_START == m_pColliderCom->Intersect(pPlayerCollider))
		{
			if (m_eTRIGState == TRIG_SCENE_CHANGE)
			{
				m_bSceneChange = true;
				CUI_FadeInOut::UI_FADEINOUT_DESC pDesc{};

				pDesc.isFadeIn = false;
					pDesc.eFadeType = CUI_FadeInOut::TYPE_ALPHA;

					list<CGameObject*> objs = m_pGameInstance->Get_GameObjects_Ref(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
					if (objs.empty())
					{
						{
							if (FAILED(m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_FadeInOut"), &pDesc)))
								return;
						}
					}
			}
			else
			{
				switch (m_eTRIGState)
				{
				case TRIG_TUTORIAL_BOSSENCOUNTER:
				{
					CMap_Element::MAP_ELEMENT_DESC pDesc = {};
					_matrix vMat = { 1.4f, 0.f, 0.f, 0.f,
					0.f, 10.f, 0.f, 0.f,
					0.f, 0.f, 1.4f, 0.f,
					154.009f, 531.828f, 96.989f, 1.f };
					XMStoreFloat4x4(&pDesc.mWorldMatrix, vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_BasicDonut");
					m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_FakeWall"), &pDesc);
				}
				break;
				case TRIG_JUGGLAS_SPAWNSECONDROOM:
				{
					CMap_Element::MAP_ELEMENT_DESC pDesc = {};
					_float4x4 vMat;
					XMStoreFloat4x4(&vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
					pDesc.iInstanceCount = 1;
					pDesc.WorldMats.emplace_back(&vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle2");
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);
				}
				break;
				case TRIG_JUGGLAS_SPAWNTHIRDROOM:
				{
					//m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Passive_Element"), 0));`
					m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), 6));
					CMap_Element::MAP_ELEMENT_DESC pDesc = {};
					_float4x4 vMat;
					XMStoreFloat4x4(&vMat, XMMatrixIdentity() * XMMatrixScaling(0.8f, 0.8f, 0.8f));
					pDesc.iInstanceCount = 1;

					pDesc.WorldMats.emplace_back(&vMat);
					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle3");
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);

					pDesc.wstrModelName = TEXT("Prototype_Component_Model_RasSamrahCastle4");
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), TEXT("Prototype_GameObject_Passive_Element"), &pDesc);
				}
				break;
				case TRIG_VIEWCHANGE_TTOS:
				{
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_SIDEVIEW;
					pTCDesc.fTransitionTime = 1.f;
					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}
					//m_pGameInstance->Set_MainCamera(3);
				}
				break;
				case TRIG_VIEWCHANGE_STOT:
				{
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					//pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
					//pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_SIDEVIEW;
					pTCDesc.iEndCam = CAM_THIRDPERSON;
					pTCDesc.fTransitionTime = 1.f;

					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					//m_pGameInstance->Set_MainCamera(1);
				}
				break;
				case TRIG_ASCEND_ELEVATOR:
				{
					m_pGameInstance->Erase(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Passive_Element"), 8));
					
					//보스 소환
					dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0))->Ascend(XMVectorSet(-310.f, 69.f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
				
					CLandObject::LANDOBJ_DESC desc{};
					desc.mWorldMatrix._41 = -8.3f;
					desc.mWorldMatrix._42 = 3.5f;
					desc.mWorldMatrix._43 = -2.4f;
					desc.mWorldMatrix._44 = 1.f;

					if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Juggulus"), &desc)))
						return;

					//보스 석상 소환
					CMap_Element::MAP_ELEMENT_DESC StatueDesc = {};
					_matrix vMat = { 0.f, 0.f, -1.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					1.f, 0.f, 0.f, 0.f,
					-410.189f, 67.966f, -2.195f, 1.f };
					XMStoreFloat4x4(&StatueDesc.mWorldMatrix, vMat);
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_BossStatue"), &StatueDesc);

					ZeroMemory(&StatueDesc, sizeof(StatueDesc));
					 vMat = { -0.91f, 0.f, -0.415f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.415f, 0.f, -0.91f, 0.f,
					-420.326f, 67.976f, -17.686f, 1.f };
					XMStoreFloat4x4(&StatueDesc.mWorldMatrix, vMat);
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_BossStatue"), &StatueDesc);

					ZeroMemory(&StatueDesc, sizeof(StatueDesc));
					 vMat = { -0.905f, 0.f, -0.426f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.426f, 0.f, -0.905f, 0.f,
					-420.068f, 67.932f, 13.209f, 1.f };
					XMStoreFloat4x4(&StatueDesc.mWorldMatrix, vMat);
					m_pGameInstance->Add_CloneObject(LEVEL_JUGGLAS, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_BossStatue"), &StatueDesc);

				}
				break;
				case TRIG_DESCEND_ELEVATOR:
				{
					dynamic_cast<CElevator*>(m_pGameInstance->Get_Object(LEVEL_JUGGLAS, TEXT("Layer_Active_Element"), 0))->Descend(XMVectorSet(-310.f, 5.6f, -1.5f, 1.f)); //LEVEL_JUGGLAS로 변경
				}
				break;
				case TRIG_VIEWCHANGE_TTOBS:
				{
					CTransitionCamera::TRANSITIONCAMERA_DESC pTCDesc = {};

					//pTCDesc.vEye = _float4(10.f, 10.f, -10.f, 1.f);
					//pTCDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

					pTCDesc.fFovy = XMConvertToRadians(60.f);
					pTCDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
					pTCDesc.fNear = 0.1f;
					pTCDesc.fFar = 3000.f;

					pTCDesc.fSpeedPerSec = 40.f;
					pTCDesc.fRotationPerSec = XMConvertToRadians(90.f);

					pTCDesc.iStartCam = CAM_THIRDPERSON;
					pTCDesc.iEndCam = CAM_SIDEVIEW;
					pTCDesc.fTransitionTime = 1.f;

					if (FAILED(m_pGameInstance->Add_Camera(LEVEL_JUGGLAS, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_TransitionCamera"), &pTCDesc)))
					{
						MSG_BOX("FAILED");
						return;
					}

					//m_pGameInstance->Set_MainCamera(2);
					dynamic_cast<CSideViewCamera*>(m_pGameInstance->Get_Cameras()[CAM_SIDEVIEW])->Set_BossScene(true);

				}
				break;
				default:
					break;
				}

				m_pGameInstance->Erase(this);
			}

		}
	}




}

HRESULT CEventTrigger::Render()
{
	return S_OK;
}

HRESULT CEventTrigger::Add_Components(void* pArg)
{

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
	ColliderDesc.eType = CCollider::TYPE_AABB;

	// 월드 매트릭스에서 스케일 추출
	_float3 vScale;
	vScale.x = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	vScale.y = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP)));
	vScale.z = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	// 기본 크기에 스케일 적용
	ColliderDesc.vExtents = _float3(3.f * vScale.x, 3.f * vScale.y, 3.f * vScale.z);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEventTrigger::Bind_ShaderResources()
{


	return S_OK;
}

CEventTrigger* CEventTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventTrigger* pInstance = new CEventTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
	CEventTrigger* pInstance = new CEventTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEventTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
