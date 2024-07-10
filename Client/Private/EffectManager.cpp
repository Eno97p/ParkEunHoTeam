#include "stdafx.h"
#include "EffectManager.h"
#include "GameInstance.h"
#include "Particle_Rect.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
{
}

HRESULT CEffectManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);

	if (FAILED(Ready_GameObjects()))
	{
		MSG_BOX("Failed_Ready_Prototype");
		return E_FAIL;
	}
	//if (FAILED(Load_Trails()))
	//{
	//	MSG_BOX("FAILED_Load_Trail");
	//	return E_FAIL;
	//}
	if (FAILED(Load_SwordTrails()))
	{
		MSG_BOX("FAILED_Load_SwordTrail");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffectManager::Generate_Trail(const _int iIndex, const _float4x4* BindMat)
{
	CParticle_Trail::TRAIL_DESC* traildesc = Trailes[iIndex].get();
	traildesc->traildesc.ParentMat = BindMat;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
		TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail"), traildesc);

	return S_OK;
}

HRESULT CEffectManager::Generate_SwordTrail(const _int iIndex, const _float4x4* Swordmat)
{
	if (iIndex >= m_pSwordTrailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CSTrail::STRAIL_DESC* SwordTrail = m_pSwordTrailes[iIndex].get();
		SwordTrail->traildesc.ParentMat = Swordmat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_SwordTrail"), TEXT("Prototype_GameObject_SwordTrail"), SwordTrail);
	}
	return S_OK;
}

HRESULT CEffectManager::Load_Trails()
{
	string FilePath = "../Bin/BinaryFile/Effect/Trail.Bin";
	ifstream Load(FilePath, std::ios::binary);
	if (!Load.good())
		return E_FAIL;
	if (!Load.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	Load.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CParticle_Trail::TRAIL_DESC> readFile = make_shared<CParticle_Trail::TRAIL_DESC>();
		Load.read((char*)&readFile->traildesc, sizeof(CVIBuffer_Trail::TRAILDESC));
		readFile->traildesc.ParentMat = nullptr;
		Load.read((char*)&readFile->vStartColor, sizeof(_float3));
		Load.read((char*)&readFile->vEndColor, sizeof(_float3));
		Load.read((char*)&readFile->vBloomColor, sizeof(_float3));
		Load.read((char*)&readFile->fBloompower, sizeof(_float));
		Load.read((char*)&readFile->Desolve, sizeof(_bool));
		Load.read((char*)&readFile->Blur, sizeof(_bool));
		Load.read((char*)&readFile->Alpha, sizeof(_bool));
		Load.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
		Load.read((char*)&readFile->eUsage, sizeof(TRAIL_USAGE));
		Load.read((char*)&readFile->DesolveNum, sizeof(_int));
		readFile->Texture = load_wstring_from_stream(Load);
		readFile->TexturePath = load_wstring_from_stream(Load);
		if (FAILED(Add_Texture_Prototype(readFile->TexturePath, readFile->Texture)))
			return E_FAIL;
		Trailes.emplace_back(readFile);
	}
	Load.close();


	return S_OK;
}

HRESULT CEffectManager::Load_SwordTrails()
{
	string finalPath = "../Bin/BinaryFile/Effect/SwordTrail.Bin";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())
		return E_FAIL;
	if (!inFile.is_open()) {
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}

	_uint iSize = 0;
	inFile.read((char*)&iSize, sizeof(_uint));
	for (int i = 0; i < iSize; ++i)
	{
		shared_ptr<CSTrail::STRAIL_DESC> readFile = make_shared<CSTrail::STRAIL_DESC>();
		inFile.read((char*)&readFile->traildesc, sizeof(CVIBuffer_SwordTrail::SwordTrailDesc));
		readFile->traildesc.ParentMat = nullptr;
		inFile.read((char*)&readFile->isBloom, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vColor, sizeof(_float3));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_pSwordTrailes.emplace_back(readFile);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Ready_GameObjects()
{
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticleMesh"),
		CParticleMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticlePoint"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ParticleRect"),
		CParticle_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Trail"),
		CParticle_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_SwordTrail"),
		CSTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffectManager::Add_Texture_Prototype(const wstring& path, const wstring& name)
{
	if (CGameInstance::GetInstance()->IsPrototype(LEVEL_STATIC, name) == true)
		return S_OK;
	else
	{
		if (FAILED(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, name,
			CTexture::Create(m_pDevice, m_pContext, path, 1))))
			return E_FAIL;
	}
	return S_OK;
}

void CEffectManager::Dynamic_Deallocation()
{
	for (auto& iter : m_Particles)
	{
		switch (iter.first)
		{
		case PARTICLETYPE::PART_POINT:
			delete ((CParticle_Point::PARTICLEPOINT*)iter.second);
			iter.second = nullptr;
			break;
		case PARTICLETYPE::PART_MESH:
			delete ((CParticleMesh::PARTICLEMESH*)iter.second);
			iter.second = nullptr;
			break;
		case PARTICLETYPE::PART_RECT:
			delete ((CParticle_Rect::PARTICLERECT*)iter.second);
			iter.second = nullptr;
			break;
		default:
			break;
		}
	}
	
}

void CEffectManager::Free()
{
	Dynamic_Deallocation();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
