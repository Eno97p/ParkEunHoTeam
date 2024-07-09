#include "VIBuffer_SwordTrail.h"

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_SwordTrail::CVIBuffer_SwordTrail(const CVIBuffer_SwordTrail& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_SwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Initialize(void* pArg)
{
	m_pDesc =  make_shared<SwordTrailDesc>(*((SwordTrailDesc*)pArg));
	m_iNumInstance = m_pDesc->iNumInstance;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 1;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(SwordTrailVertex);

#pragma region VERTEX_BUFFER 

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


#pragma endregion
#pragma region INDEX_BUFFER 
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_InitialData.pSysMem = pIndices;
	
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
	Safe_Delete_Array(pIndices);

#pragma endregion
#pragma region INSTANCE_BUFFER

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	SwordTrailVertex* pInstanceVertices = new SwordTrailVertex[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(SwordTrailVertex) * m_iNumInstance);

	_matrix ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	_vector vRight = XMVector4Normalize(ParentMat.r[0]);
	_vector vUp = XMVector4Normalize(ParentMat.r[1]);
	_vector vLook = XMVector4Normalize(ParentMat.r[2]);
	_vector vPos = ParentMat.r[3];
	_float3 Offset = m_pDesc->vOffsetPos;

	vPos += vRight * m_pDesc->vOffsetPos.x;
	vPos += vUp * m_pDesc->vOffsetPos.y;
	vPos += vLook * m_pDesc->vOffsetPos.z;
	ParentMat.r[3] = vPos;
	_vector Top = vPos + vUp * m_pDesc->vSize.y;
	_vector Bottom = vPos - vUp * m_pDesc->vSize.y;

	//Zero = 왼쪽 위, One 왼쪽 아래, Two 오른쪽 아래, Three 오른쪽 위 
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat3(&pInstanceVertices[i].Zero, Top);
		XMStoreFloat3(&pInstanceVertices[i].One, Bottom);
		XMStoreFloat3(&pInstanceVertices[i].Two, Bottom);
		XMStoreFloat3(&pInstanceVertices[i].Three, Top);

		pInstanceVertices[i].texCoord0 = _float2(_float(i) / _float(m_iNumInstance), 0.f);
		pInstanceVertices[i].texCoord1 = _float2(_float(i) / _float(m_iNumInstance), 1.f);
		pInstanceVertices[i].texCoord2 = _float2(_float(i+1) / _float(m_iNumInstance),1.f);
		pInstanceVertices[i].texCoord3 = _float2(_float(i+1) / _float(m_iNumInstance), 0.f);

		pInstanceVertices[i].lifetime.x = m_pDesc->fLifeTime;
		pInstanceVertices[i].lifetime.y = 0.f;
	}

	m_InitialData.pSysMem = pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);

#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance
	};

	_uint					iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride
	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_iIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_SwordTrail::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CVIBuffer_SwordTrail::Tick(_float fDelta)
{
	_matrix ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	_vector vRight = XMVector4Normalize(ParentMat.r[0]);
	_vector vUp = XMVector4Normalize(ParentMat.r[1]);
	_vector vLook = XMVector4Normalize(ParentMat.r[2]);
	_vector vPos = ParentMat.r[3];
	_float3 Offset = m_pDesc->vOffsetPos;

	vPos += vRight * Offset.x;
	vPos += vUp * Offset.y;
	vPos += vLook * Offset.z;
	ParentMat.r[3] = vPos;

	_vector Top = vPos + vUp * m_pDesc->vSize.y;
	_vector Bottom = vPos - vUp * m_pDesc->vSize.y;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	SwordTrailVertex* pVertices = (SwordTrailVertex*)SubResource.pData;
	for (_int i = m_iNumInstance-1; i >= 0; i--)
	{
		if (i == 0)
		{
			pVertices[i].Three = pVertices[i].Zero;
			pVertices[i].Two = pVertices[i].One;
			XMStoreFloat3(&pVertices[i].Zero, Top);
			XMStoreFloat3(&pVertices[i].One, Bottom);

			
		}
		else
		{
			pVertices[i].Three = pVertices[i].Zero;
			pVertices[i].Two = pVertices[i].One;
			pVertices[i].Zero = pVertices[i - 1].Three;
			pVertices[i].One = pVertices[i - 1].Two;

			_float2 a= pVertices[i].texCoord0;
			_float2 b=pVertices[i].texCoord1;
			_float2 c=pVertices[i].texCoord2;
			_float2 d=pVertices[i].texCoord3;
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);
}



XMVECTOR CVIBuffer_SwordTrail::CatmullRom(const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	XMVECTOR result = (2.0f * P1) +
		(-P0 + P2) * t +
		(2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
		(-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3;

	return result * 0.5f;
}


CVIBuffer_SwordTrail* CVIBuffer_SwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_SwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_SwordTrail::Clone(void* pArg)
{
	CVIBuffer_SwordTrail* pInstance = new CVIBuffer_SwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_SwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
