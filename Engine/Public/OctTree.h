#pragma once

#include "Base.h"

BEGIN(Engine)

class COctTree final : public CBase
{
public:
	enum CORNER {
		TOP_LEFT_FRONT, TOP_RIGHT_FRONT, BOTTOM_RIGHT_FRONT, BOTTOM_LEFT_FRONT,
		TOP_LEFT_BACK, TOP_RIGHT_BACK, BOTTOM_RIGHT_BACK, BOTTOM_LEFT_BACK,
		CORNER_END
	};

	enum NEIGHBOR {
		NEIGHBOR_LEFT, NEIGHBOR_RIGHT,
		NEIGHBOR_TOP, NEIGHBOR_BOTTOM,
		NEIGHBOR_FRONT, NEIGHBOR_BACK,
		NEIGHBOR_END
	};
private:
	COctTree();
	virtual ~COctTree() = default;

public:
	HRESULT Initialize(const _float3& vMin, const _float3& vMax);
	void Culling(const _float4* pVerticesPositions, _uint* pIndices, _uint* iNumIndices);
	void Make_Neighbors();


private:
	COctTree* m_pChildren[CORNER_END] = { nullptr };
	COctTree* m_pNeighbors[NEIGHBOR_END] = { nullptr, nullptr, nullptr, nullptr };
	_uint					m_iCenterIndex = { 0 };
	_uint					m_iCorners[CORNER_END] = { 0 };
	class CGameInstance* m_pGameInstance = { nullptr };

	_float3     m_vMin, m_vMax;  // 경계 박스

private:
	_bool isDraw(const _float4* pVertexPositions);

public:
	static COctTree* Create(const _float3& vMin, const _float3& vMax);
	virtual void Free() override;
};

END