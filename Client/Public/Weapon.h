#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon : public CPartObject
{

public:
	typedef struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;

	}WEAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	void Set_Active(_bool isActive = true)
	{
		if (m_bIsActive == false && isActive == true)
		{
			m_GenerateTrail = true;
		}
		m_bIsActive = isActive;
	}
	void Generate_Trail(_int iIndex);

	_bool Get_Active() { return m_bIsActive; }

protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };
	_bool							m_bIsActive = false;



protected:
	_bool							m_GenerateTrail = false;
public:
	virtual void Free() override;
};

END