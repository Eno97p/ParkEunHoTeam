#pragma once

#include "PhysXComponent.h"




//해당 클래스는 static 오브젝트에 대한 사전 쿠킹을 위한 클래스

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CPhysXComponent_Vehicle final : public CPhysXComponent
{

private:
	explicit CPhysXComponent_Vehicle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysXComponent_Vehicle(const CPhysXComponent_Vehicle& rhs);
	virtual ~CPhysXComponent_Vehicle() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, const wstring& FilePath);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
	virtual HRESULT Render()override;
#endif



private:
	HRESULT CreateActor();


private:

	float m_fHoverHeight = 0.5f;
	float m_fHoverForce = 500.0f;
	float m_fStabilizeForce = 100.0f;
	float m_fMaxTiltAngle = 30.0f; // 최대 기울기 각도 (도)


public:
	static CPhysXComponent_Vehicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const wstring& FilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END