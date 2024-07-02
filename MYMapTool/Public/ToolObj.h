#pragma once

#include "GameObject.h"
#include "MYMapTool_Defines.h"

#include "Model.h"

// IMGUI에서 생성할 모든 Obj들을 담당하는 클래스
// 애니메이션 여부, 

BEGIN(Engine)
class CShader;

END

BEGIN(MYMapTool)

class CToolObj : public CGameObject
{
public:
	// Client에 객체 생성 시 들고 가야 할 정보를 담을 구조체
	typedef struct ToolObj_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_char szObjName[MAX_PATH] = ""; // 객체 이름
		_char szLayer[MAX_PATH] = ""; // Layer 이름
		_char szModelName[MAX_PATH] = ""; // Model 이름
		_float4x4 mWorldMatrix{};
		CModel::MODELTYPE eModelType; // Anim 여부

		_float4 f4Color = {0.f, 0.f, 0.f, 0.f};
	}TOOLOBJ_DESC;

protected:
	CToolObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolObj(const CToolObj& rhs);
	virtual ~CToolObj() = default;

public:
	_char* Get_Name() { return m_szName; }								//이름
	_char* Get_Layer() { return m_szLayer; }
	_char* Get_ModelName() { return m_szModelName; }
	_matrix			Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }		// 월드 행렬
	const _float4x4* Get_WorldFloat4x4() { return m_pTransformCom->Get_WorldFloat4x4(); }
	CModel::MODELTYPE	Get_ModelType() { return m_eModelType; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Render_LightDepth();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
protected:
	_char				m_szName[MAX_PATH] = "";
	_char				m_szLayer[MAX_PATH] = "";
	_char				m_szModelName[MAX_PATH] = "";
	_vector				m_vPosition;
	CModel::MODELTYPE	m_eModelType;

	_char				m_szListName[MAX_PATH] = "";  // Imgui의 생성 목록을 위한 이름... 별론데
	_float4				m_vColorOffset = { 0.f, 0.f, 0.f, 0.f };
	_uint				m_iShaderPath = 0;

protected:
	_bool				m_bIsChild = false;
	//for Wind Move Grass
private:
	XMFLOAT3 m_WindDirection;
	float m_WindStrength;
	float m_WindFrequency;
	float m_WindGustiness;
	_float				m_fTimeDelta = 0.f;

public:
	HRESULT	Add_Component();
	HRESULT	Bind_ShaderResources();

	HRESULT	Check_AnimType();
	void	Setting_WorldMatrix(void* pArg);

public:
	static CToolObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;

};

END