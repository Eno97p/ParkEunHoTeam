#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer{ rhs }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{
	m_TrailDescription = *((TRAILDESC*)pArg);
	m_iNumInstance = m_TrailDescription.iNumInstance;
	//m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iNumVertexBuffers = 2;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumVertices = 4;

	m_iIndexStride = 2;
	m_iIndexCountPerInstance = 6;
	m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
	m_iInstanceStride = sizeof(VTXMATRIX);

#pragma region VERTEX_BUFFER 

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

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
	_uint			iNumIndices = { 0 };
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}
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

	VTXMATRIX* pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	m_pOriginalSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pOriginalSpeeds, sizeof(_float) * m_iNumInstance);

	m_pSize = new _float3[m_iNumInstance];
	ZeroMemory(m_pSize, sizeof(_float3) * m_iNumInstance);

	m_pOriginalSize = new _float3[m_iNumInstance];
	ZeroMemory(m_pOriginalSize, sizeof(_float3) * m_iNumInstance);

	m_pPivotPos = m_TrailDescription.vPivotPos;

	_matrix ParentMat = XMLoadFloat4x4(m_TrailDescription.ParentMat);
	XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDescription.vPivotPos), ParentMat);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pOriginalSize[i] = m_pSize[i] = m_TrailDescription.vSize;
		pInstanceVertices[i].vRight = _float4(m_pSize[i].x, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, m_pSize[i].y, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, m_pSize[i].z, 0.f);
		XMStoreFloat4(&pInstanceVertices[i].vTranslation, vPos);
		pInstanceVertices[i].vLifeTime = _float2(m_TrailDescription.fLifeTime, 0.f);
		m_pOriginalSpeeds[i] = m_pSpeeds[i] = m_TrailDescription.vSpeed;
	}
	m_InitialData.pSysMem = pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InitialData, &m_pVBInstance)))
		return E_FAIL;
	Safe_Delete_Array(pInstanceVertices);



#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Trail::Bind_Buffers()
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

HRESULT CVIBuffer_Trail::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);
	return S_OK;
}



void CVIBuffer_Trail::ExtinctTrail(_float fDelta)
{
	//생성 시점을 나눠서 잡아줘야함

	//XMMATRIX ParentMat = XMLoadFloat4x4(m_pDesc->ParentMat);
	//XMVECTOR vUp = XMVector4Normalize(ParentMat.r[1]);
	//
	//XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_pDesc->vOffsetPos), ParentMat);
	_matrix ParentMat = XMLoadFloat4x4(m_TrailDescription.ParentMat);
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fDelta;
		m_pSpeeds[i] += fDelta;

		if (i > 0)  // 첫 번째 인스턴스가 아니면 이전 인스턴스의 위치를 따라감
		{
			_vector vPos = XMLoadFloat4(&pVertices[i].vTranslation);
			_vector Up = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vUp), XMLoadFloat4(&pVertices[i - 1].vUp), m_pSpeeds[i] * fDelta));
			_vector Right = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vRight), XMLoadFloat4(&pVertices[i - 1].vRight), m_pSpeeds[i] * fDelta));
			_vector Look = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vLook), XMLoadFloat4(&pVertices[i - 1].vLook), m_pSpeeds[i] * fDelta));

			vPos = XMVectorLerp(vPos, XMLoadFloat4(&pVertices[i - 1].vTranslation), m_pSpeeds[i] * fDelta);

			XMStoreFloat4(&pVertices[i].vTranslation, vPos);
			XMStoreFloat4(&pVertices[i].vRight, Right * m_pSize[i].x);
			XMStoreFloat4(&pVertices[i].vUp, Up * m_pSize[i].y);
			XMStoreFloat4(&pVertices[i].vLook, Look * m_pSize[i].z);
		}
		else  // 첫 번째 인스턴스는 고정된 위치
		{
			_vector vRight = XMVector4Normalize(ParentMat.r[0]);
			_vector vUp = XMVector4Normalize(ParentMat.r[1]);
			_vector vLook = XMVector4Normalize(ParentMat.r[2]);
			XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDescription.vPivotPos), ParentMat);

			XMStoreFloat4(&pVertices[i].vTranslation, vPos);
			XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i].x);
			XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i].y);
			XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i].z);
		}

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	if (allInstancesDead)
		m_bInstanceDead = true;
	else
		m_bInstanceDead = false;
}

void CVIBuffer_Trail::EternalTrail(_float fDelta)
{
	_matrix ParentMat = XMLoadFloat4x4(m_TrailDescription.ParentMat);
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		
		if (i > 0)  // 첫 번째 인스턴스가 아니면 이전 인스턴스의 위치를 따라감
		{
			_vector vPos = XMLoadFloat4(&pVertices[i].vTranslation);
			_vector Up = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vUp), XMLoadFloat4(&pVertices[i - 1].vUp), m_pSpeeds[i] * fDelta));
			_vector Right = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vRight), XMLoadFloat4(&pVertices[i - 1].vRight), m_pSpeeds[i] * fDelta));
			_vector Look = XMVector3Normalize(XMVectorLerp(XMLoadFloat4(&pVertices[i].vLook), XMLoadFloat4(&pVertices[i - 1].vLook), m_pSpeeds[i] * fDelta));

			vPos = XMVectorLerp(vPos, XMLoadFloat4(&pVertices[i - 1].vTranslation), m_pSpeeds[i] * fDelta);
			
			XMStoreFloat4(&pVertices[i].vTranslation, vPos);
			XMStoreFloat4(&pVertices[i].vRight, Right * m_pSize[i].x);
			XMStoreFloat4(&pVertices[i].vUp, Up * m_pSize[i].y);
			XMStoreFloat4(&pVertices[i].vLook, Look * m_pSize[i].z);

			float ratio = static_cast<float>(i) / static_cast<float>(m_iNumInstance - 1);
			pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x * ratio;
		}
		else  // 첫 번째 인스턴스는 고정된 위치
		{
			_vector vRight = XMVector4Normalize(ParentMat.r[0]);
			_vector vUp = XMVector4Normalize(ParentMat.r[1]);
			_vector vLook = XMVector4Normalize(ParentMat.r[2]);
			pVertices[i].vLifeTime.y = 0.f;
			XMVECTOR vPos = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDescription.vPivotPos), ParentMat);

			XMStoreFloat4(&pVertices[i].vTranslation, vPos);
			XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i].x);
			XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i].y);
			XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i].z);
		}

	}
	m_pContext->Unmap(m_pVBInstance, 0);
}



XMVECTOR CVIBuffer_Trail::CatmullRom(const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	XMVECTOR result = (2.0f * P1) +
		(-P0 + P2) * t +
		(2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
		(-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3;

	return result * 0.5f;
}



CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Trail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pSize);
		Safe_Delete_Array(m_pOriginalSize);
		Safe_Delete_Array(m_pOriginalSpeeds);
	}
	Safe_Release(m_pVBInstance);



}
