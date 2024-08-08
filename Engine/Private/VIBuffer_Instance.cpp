#include "..\Public\VIBuffer_Instance.h"
#include "VIBuffer_Terrain.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
	: CVIBuffer{ rhs }
	, m_iNumInstance{ rhs.m_iNumInstance }
	, m_iInstanceStride{ rhs.m_iInstanceStride }
	, m_iIndexCountPerInstance{ rhs.m_iIndexCountPerInstance }
	, m_InstanceBufferDesc{ rhs.m_InstanceBufferDesc }
	, m_pSpeeds{ rhs.m_pSpeeds }
	, m_pOriginalSpeed{ rhs.m_pOriginalSpeed }
	, m_pOriginalPositions{ rhs.m_pOriginalPositions }
	, m_pOriginalGravity{ rhs.m_pOriginalGravity }
	, m_InstanceDesc{ rhs.m_InstanceDesc }
	, m_bInstanceDead{ rhs.m_bInstanceDead }
{
	
	//m_pContext->CopyResource(m_pVBInstance, rhs.m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype(const INSTANCE_DESC& InstanceDesc)
{
	m_iNumInstance = InstanceDesc.iNumInstance;

	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
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
HRESULT CVIBuffer_Instance::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}


void CVIBuffer_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * 0.1f;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;
		
		pVertices[i].vGravity = m_pOriginalGravity[i] * (pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x);
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		vDir -= XMVectorSet(0.f, pVertices[i].vGravity, 0.f, 0.f);

	/*	pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;*/
	
	
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	
		_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(360) * fTimeDelta);
		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);
	
	
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		
	
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}
	
	m_pContext->Unmap(m_pVBInstance, 0);
	
	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.y += fTimeDelta;



		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.01f * m_pSpeeds[i];

		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spread_Size_Up(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.1f * m_pSpeeds[i];
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		/*pVertices[i].vGravity += fTimeDelta;*/

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(360) * fTimeDelta);
		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}

		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}

	}


	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}



}

void CVIBuffer_Instance::Spread_Non_Rotation(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);
		/*pVertices[i].vGravity += fTimeDelta;*/

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::CreateSwirlEffect(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Use vRange.z as the base radius and scale it with the y position
		float fBaseRadius = m_InstanceDesc.vRange.z;
		float currentRadius = fBaseRadius + pVertices[i].vTranslation.y * 0.2f; // Adjust the factor as needed

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on gravity
		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime
		pVertices[i].vLifeTime.y += fTimeDelta;
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spread_Speed_Down(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSpeeds[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSpeeds[i] < 0.f)
			m_pSpeeds[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		//pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				//pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::SlashEffect(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSize[i] += fTimeDelta * m_pSpeeds[i];

		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pOriginalSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pOriginalSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		//XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				//pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Spread_Speed_Down_SizeUp(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		m_pSize[i] += fTimeDelta *  pVertices[i].vGravity;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;


		m_pSpeeds[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSpeeds[i] < 0.f)
			m_pSpeeds[i] = 0.f;

		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		//pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;


		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Gather(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;


		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity;
		if (m_pSize[i] < 0.f)
			m_pSize[i] = 0.f;


		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);

		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Extinction(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity;

		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::GrowOutY(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * 0.01f * m_pSpeeds[i];

		pVertices[i].vUp.y = m_pSize[i];


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut_Speed_Down(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;

		_vector			vDir = XMVectorSet(0.f, 0.f, 0.f, 1.f) - XMLoadFloat4(&pVertices[i].vTranslation);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);


		//pVertices[i].vRight.x = m_pSize[i];
		//pVertices[i].vUp.y = m_pSize[i];
		//pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::GrowOut_Speed_Down_Texture(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;


		pVertices[i].vRight.x = m_pSize[i];
		pVertices[i].vUp.y = m_pSize[i];
		pVertices[i].vLook.z = m_pSize[i];

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Lenz_Flare(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * m_pSpeeds[i];
		m_pSpeeds[i] -= fTimeDelta * 0.1f;

		_vector vRight, vUp, vLook;
		vRight = XMLoadFloat4(&pVertices[i].vRight);
		vUp = XMLoadFloat4(&pVertices[i].vUp);
		vLook = XMLoadFloat4(&pVertices[i].vLook);

		_vector    vQuternion = XMQuaternionRotationAxis(XMVector4Normalize(vLook), fTimeDelta * pVertices[i].vGravity);
		_matrix    QuternionMatrix = XMMatrixRotationQuaternion(vQuternion);

		vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
		vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
		vLook = XMVector3TransformNormal(vLook, QuternionMatrix);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * m_pSize[i]);

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Spiral_Extinction(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Calculate the distance from the center
		XMVECTOR vDist = XMVectorSet(
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			0.0f
		);
		float distance = XMVectorGetX(XMVector3Length(vDist));

		// Gradually decrease the radius using m_pSpeeds
		float radiusDecreaseFactor = m_pSpeeds[i] * 0.01f; // Adjust this value to control the spiral tightness
		float currentRadius = distance * (1.0f - radiusDecreaseFactor * pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x);

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on gravity
		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime and size
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] -= fTimeDelta * pVertices[i].vGravity * 0.05f; // Gradually decrease size

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				// Reset position with some randomness
				float randomAngle = (float)rand() / RAND_MAX * XM_2PI;
				float randomRadius = ((float)rand() / RAND_MAX) * m_InstanceDesc.vRange.z;
				pVertices[i].vTranslation = _float4(
					m_InstanceDesc.vOffsetPos.x + cos(randomAngle) * randomRadius,
					m_InstanceDesc.vOffsetPos.y + ((float)rand() / RAND_MAX - 0.5f) * m_InstanceDesc.vRange.y,
					m_InstanceDesc.vOffsetPos.z + sin(randomAngle) * randomRadius,
					1.f
				);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i] + ((float)rand() / RAND_MAX - 0.5f) * 0.5f; // Add some randomness to speed
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}

void CVIBuffer_Instance::Spiral_Expansion(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	bool allInstancesExpanded = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// Calculate the current angle
		float angle = atan2(pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x);

		// Increase the angle over time to create a swirl effect
		angle += m_pSpeeds[i] * fTimeDelta;

		// Calculate the distance from the center
		XMVECTOR vDist = XMVectorSet(
			pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z,
			0.0f
		);
		float distance = XMVectorGetX(XMVector3Length(vDist));

		// Gradually increase the radius using m_pSpeeds
		float radiusIncreaseFactor = m_pSpeeds[i] * 0.05f; // Adjust this value to control the expansion speed
		float currentRadius = distance + radiusIncreaseFactor * pVertices[i].vLifeTime.y;

		// Calculate the new position
		float x = m_InstanceDesc.vOffsetPos.x + currentRadius * cos(angle);
		float z = m_InstanceDesc.vOffsetPos.z + currentRadius * sin(angle);

		// Update the position
		pVertices[i].vTranslation.x = x;
		pVertices[i].vTranslation.z = z;

		// Update the height based on an upward force (opposite of gravity)
		pVertices[i].vTranslation.y += pVertices[i].vGravity * fTimeDelta * 0.5f;

		// Update direction vectors (Right, Up, Look)
		XMVECTOR vDir = XMVectorSet(pVertices[i].vTranslation.x - m_InstanceDesc.vOffsetPos.x,
			pVertices[i].vTranslation.y - m_InstanceDesc.vOffsetPos.y,
			pVertices[i].vTranslation.z - m_InstanceDesc.vOffsetPos.z, 0.0f);
		XMVECTOR vLook = XMVector3Normalize(vDir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vLook, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vRight, vLook));

		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, vLook * m_pSize[i]);

		// Update lifetime and size
		pVertices[i].vLifeTime.y += fTimeDelta;
		m_pSize[i] += fTimeDelta * pVertices[i].vGravity * 0.02f; // Gradually increase size

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (m_InstanceDesc.isLoop)
			{
				// Reset position to center
				pVertices[i].vTranslation = _float4(m_InstanceDesc.vOffsetPos.x, m_InstanceDesc.vOffsetPos.y, m_InstanceDesc.vOffsetPos.z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
				m_pSize[i] = m_pOriginalSize[i] * 0.5f; // Start with smaller size
				m_pSpeeds[i] = m_pOriginalSpeed[i] + ((float)rand() / RAND_MAX - 0.5f) * 0.5f; // Add some randomness to speed
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesExpanded = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesExpanded)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}
}


void CVIBuffer_Instance::Leaf_Fall(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		pVertices[i].vLifeTime.y += fTimeDelta;

		_float fRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;
		_vector vRight, vUp, vLook, vPos;
		vRight = XMLoadFloat4(&pVertices[i].vRight);
		vUp = XMLoadFloat4(&pVertices[i].vUp);
		vLook = XMLoadFloat4(&pVertices[i].vLook);
		vPos = XMLoadFloat4(&pVertices[i].vTranslation);

		if (fRatio <= 0.2f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.2f && fRatio <= 0.4f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.4f && fRatio <= 0.6f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else if (fRatio > 0.6f && fRatio <= 0.8f)
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}
		else
		{
			_vector Axis = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(m_pSpeeds[i]));
			_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
			vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
			vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
			vLook = XMVector3TransformNormal(vLook, QuternionMatrix);
		}

		_vector vDir = XMVector4Normalize(vLook);
		vPos += vDir * m_pSpeeds[i] * fTimeDelta;




		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp)* m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vTranslation, vPos);

		pVertices[i].vTranslation.y -= pVertices[i].vGravity * fTimeDelta;

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}


}

void CVIBuffer_Instance::Blow(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;

		//파티클 움직임 로직
		_vector			vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_InstanceDesc.vOffsetPos), 0.f);

		_vector vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector vLook = XMLoadFloat4(&pVertices[i].vLook);
		_vector vPos = XMLoadFloat4(&pVertices[i].vTranslation);

		_vector Quat1 = XMQuaternionRotationAxis(XMVector4Normalize(vRight), XMConvertToRadians(pVertices[i].vGravity));
		_vector Quat2 = XMQuaternionRotationAxis(XMVector4Normalize(vUp), XMConvertToRadians(pVertices[i].vGravity));
		_vector Axis = XMQuaternionMultiply(Quat1, Quat2);

		_matrix QuternionMatrix = XMMatrixRotationQuaternion(Axis);
		vRight = XMVector3TransformNormal(vRight, QuternionMatrix);
		vUp = XMVector3TransformNormal(vUp, QuternionMatrix);
		vLook = XMVector3TransformNormal(vLook, QuternionMatrix);

		vPos += vDir * m_pSpeeds[i] * fTimeDelta;

		XMStoreFloat4(&pVertices[i].vRight, XMVector4Normalize(vRight) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, XMVector4Normalize(vUp) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, XMVector4Normalize(vLook) * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vTranslation, vPos);


		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vRight.x = m_pOriginalSize[i];
				pVertices[i].vUp.y = m_pOriginalSize[i];
				pVertices[i].vLook.z = m_pOriginalSize[i];
				m_pSize[i] = m_pOriginalSize[i];
				m_pSpeeds[i] = m_pOriginalSpeed[i];
			}
			else
			{
				pVertices[i].vLifeTime.y = pVertices[i].vLifeTime.x;
			}
		}
		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
		{
			allInstancesDead = false;
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	if (!m_InstanceDesc.isLoop && allInstancesDead)
	{
		m_bInstanceDead = true;
	}
	else
	{
		m_bInstanceDead = false;
	}

}

void CVIBuffer_Instance::Up_To_Stop(_float fTimeDelta)
{
	bool allInstancesDead = true;
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.y += fTimeDelta;
		pVertices[i].vGravity -= fTimeDelta; //진짜 라이프타임으로 쓰는거임

		pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;

		_vector vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		XMStoreFloat4(&pVertices[i].vLook, vDir * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vRight, vRight * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, vUp * m_pSize[i]);

		if (pVertices[i].vGravity < 0.f)
		{
			m_pSpeeds[i] += pVertices[i].vGravity * 0.05f;
			if (m_pSpeeds[i] < 0.f)
			{
				m_pSpeeds[i] = 0.f;
				allInstancesDead = true;
			}
			else
				allInstancesDead = false;
		}
		else
			allInstancesDead = false;

		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x && !allInstancesDead)
		{
			pVertices[i].vLifeTime.y = 0.f;
			pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
		}
	}
	m_pContext->Unmap(m_pVBInstance, 0);

	m_bInstanceDead = allInstancesDead;
}







void CVIBuffer_Instance::Initial_RotateY()
{
	D3D11_MAPPED_SUBRESOURCE      SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_matrix      RotationMatrix = XMMatrixRotationAxis(XMLoadFloat4(&pVertices[i].vUp),
			XMConvertToRadians(RandomFloat(0.f, 360.f)));

		_vector Right = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vRight));
		_vector Up = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vUp));
		_vector Look = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vLook));

		Right = XMVector3TransformNormal(Right, RotationMatrix);
		Up = XMVector3TransformNormal(Up, RotationMatrix);
		Look = XMVector3TransformNormal(Look, RotationMatrix);

		XMStoreFloat4(&pVertices[i].vRight, Right * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vUp, Up * m_pSize[i]);
		XMStoreFloat4(&pVertices[i].vLook, Look * m_pSize[i]);


	}
	m_pContext->Unmap(m_pVBInstance, 0);
}

vector<_float4x4*> CVIBuffer_Instance::Get_VtxMatrices()
{
	vector<_float4x4*> worldMats;
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
	{
		return worldMats; // 맵핑 실패시 빈 벡터 반환
	}
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4x4* mat = new _float4x4();  // 동적 할당
		// 정규화된 방향 벡터 계산
		_vector Right = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vRight));
		_vector Up = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vUp));
		_vector Look = XMVector4Normalize(XMLoadFloat4(&pVertices[i].vLook));
		// 행렬 구성
		XMStoreFloat4x4(mat, XMMatrixSet(
			Right.m128_f32[0], Right.m128_f32[1], Right.m128_f32[2], 0.0f,
			Up.m128_f32[0], Up.m128_f32[1], Up.m128_f32[2], 0.0f,
			Look.m128_f32[0], Look.m128_f32[1], Look.m128_f32[2], 0.0f,
			pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pVertices[i].vTranslation.z, 1.0f
		));
		worldMats.push_back(mat);
	}
	m_pContext->Unmap(m_pVBInstance, 0);
	return worldMats;
}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pOriginalSpeed);
		Safe_Delete_Array(m_pOriginalPositions);
		Safe_Delete_Array(m_pOriginalGravity);
		Safe_Delete_Array(m_pSize);
		Safe_Delete_Array(m_pOriginalSize);
	}

	Safe_Release(m_pVBInstance);
}
