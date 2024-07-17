#include "..\Public\VIBuffer_Instance.h"

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

		m_pSize[i] += fTimeDelta * 0.1f * pVertices[i].vGravity;
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







void CVIBuffer_Instance::Initial_RotateY()
{
	D3D11_MAPPED_SUBRESOURCE      SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_matrix      RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f),
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

void CVIBuffer_Instance::Excute_Trail(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	bool allInstancesDead = true;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXMATRIX* pVertices = (VTXMATRIX*)SubResource.pData;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float RatioLength = i / m_iNumInstance;
		pVertices[i].vLifeTime.y += fTimeDelta;
		
		if (i == 0)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMVectorSet(0.f, 0.f, 0.f, 0.f));
		}
		else
		{


		}
		_vector			vDir = XMVectorSubtract(XMLoadFloat4(&pVertices[i].vTranslation), XMVectorSet(0.f, 0.f, 0.f, 1.f));

		_float LifeTimeRatio = pVertices[i].vLifeTime.y / pVertices[i].vLifeTime.x;

		if (LifeTimeRatio < pVertices[i].vGravity)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeeds[i] * fTimeDelta);
		}
	
		if (pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			if (true == m_InstanceDesc.isLoop)
			{
				pVertices[i].vTranslation = _float4(m_pOriginalPositions[i].x, m_pOriginalPositions[i].y, m_pOriginalPositions[i].z, 1.f);
				pVertices[i].vLifeTime.y = 0.f;
				pVertices[i].vGravity = m_pOriginalGravity[i];
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
