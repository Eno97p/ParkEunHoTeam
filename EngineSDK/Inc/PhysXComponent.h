#pragma once

#include "Component.h"



BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent : public CComponent
{
public:
	typedef struct PHYSX_DESC
	{

		_float3 fMatterial;
		_float4x4 fWorldMatrix = {};
		CComponent* pComponent = nullptr;
		PxGeometryType::Enum eGeometryType = PxGeometryType::eINVALID;
		const char* pName = nullptr;
		_float2 fCapsuleProperty = { 0.f, 0.f };
		_float3 fBoxProperty = { 0.f, 0.f, 0.f };
		PxFilterData filterData = {};
	}PHYSX_DESC;



protected:
	CPhysXComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXComponent(const CPhysXComponent& rhs);
	virtual ~CPhysXComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;


#ifdef _DEBUG
	virtual HRESULT  Init_Buffer();
	virtual HRESULT Render();
	virtual void* GetData() override;
#endif

public:
	virtual void Tick(const _float4x4* pWorldMatrix);
	virtual void Late_Tick(_float4x4* pWorldMatrix);
	virtual void SetFilterData(PxFilterData  filterData);

protected:
	virtual tuple<vector<PxVec3>, vector<PxU32>> CreateTriangleMeshDesc(void* pvoid);

public:
	vector<_float3> CreateCapsuleVertices(float radius, float halfHeight, int segments = 32, int rings = 32);
	HRESULT CreateActor(PxGeometryType::Enum eGeometryType, const PxTransform pxTrans);
	PxActor* Get_Actor() { return m_pActor; }













protected:
#ifdef _DEBUG
	class CShader*								m_pShader = { nullptr };	
#endif
	vector <class CVIBuffer_PhysXBuffer*>		m_pBuffer = {};
	PxMaterial* m_pMaterial = { nullptr };
	PxRigidActor* m_pActor = { nullptr };
	PxShape* m_pShape = { nullptr };
	_float4x4 m_WorldMatrix = {};
	
private:
	class CModel* m_pModelCom = { nullptr };
	vector<_float3> m_vecVertices;

	_float3 m_fBoxProperty = { 0.f, 0.f, 0.f };
	_float2 m_fCapsuleProperty = { 0.f, 0.f };

public:
	static CPhysXComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();



public:
	static PxTransform Convert_DxMat_To_PxTrans(const _float4x4& pWorldMatrix);


};

END