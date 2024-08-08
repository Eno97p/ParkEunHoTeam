#include "stdafx.h"
#include "EffectManager.h"
#include "GameInstance.h"
#include "Particle_Rect.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"
#include "FireEffect.h"

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
	if (FAILED(Load_Trails()))
	{
		MSG_BOX("FAILED_Load_Trail");
		return E_FAIL;
	}
	if (FAILED(Load_SwordTrails()))
	{
		MSG_BOX("FAILED_Load_SwordTrail");
		return E_FAIL;
	}
	if (FAILED(Load_Particles()))
	{
		MSG_BOX("FAILED_Load_SwordTrail");
		return E_FAIL;
	}
	if (FAILED(Load_Distortions()))
	{
		MSG_BOX("FAILED_Load_Distortion");
		return E_FAIL;
	}
	if (FAILED(Load_Lightnings()))
	{
		MSG_BOX("FAILED_Load_Lightnings");
		return E_FAIL;
	}
	if (FAILED(Load_Tornados()))
	{
		MSG_BOX("FAILED_Load_Tornado");
		return E_FAIL;
	}
	if (FAILED(Load_Heals()))
	{
		MSG_BOX("FAILED_Load_Heal");
		return E_FAIL;
	}
	if (FAILED(Load_Lazers()))
	{
		MSG_BOX("FAILED_Load_Lazer");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Trail(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= Trailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}

	CParticle_Trail::TRAIL_DESC* traildesc = Trailes[iIndex].get();
	traildesc->traildesc.ParentMat = BindMat;
	CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(), TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail"), traildesc);
	return S_OK;
}

CGameObject* CEffectManager::Member_Trail(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= Trailes.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	CParticle_Trail::TRAIL_DESC* traildesc = Trailes[iIndex].get();
	traildesc->traildesc.ParentMat = BindMat;
	return CGameInstance::GetInstance()->Clone_Object(TEXT("Prototype_GameObject_Trail"), traildesc);
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

HRESULT CEffectManager::Generate_Particle(const _int iIndex,
	const _float4 vStartpos,
	CGameObject* pTarget,
	const _vector vAxis,
	const _float fRadians,
	const _vector vLook)
{
	if (iIndex >= m_Particles.size())
	{
		MSG_BOX("인덱스 사이즈 초과, 파티클 생성 실패");
		return S_OK;
	}

	PARTICLETYPE eType = m_Particles[iIndex].first;

	switch (eType)
	{
	case Client::PART_POINT:
	{
		((CParticle_Point::PARTICLEPOINT*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticle_Point* pPoint = static_cast<CParticle_Point*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticlePoint"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pPoint->Set_Target(pTarget);

		if(!XMVector4Equal(vAxis, XMVectorZero()))
			pPoint->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pPoint->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Particle"), pPoint);
		break;
	}
	case Client::PART_MESH:
	{
		((CParticleMesh::PARTICLEMESH*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticleMesh* pMesh = static_cast<CParticleMesh*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticleMesh"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pMesh->Set_Target(pTarget);
		if (!XMVector4Equal(vAxis, XMVectorZero()))
			pMesh->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pMesh->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Particle"), pMesh);
		break;
	}
	case Client::PART_RECT:
	{
		((CParticle_Rect::PARTICLERECT*)m_Particles[iIndex].second)->SuperDesc.vStartPos = vStartpos;
		CParticle_Rect* pRect = static_cast<CParticle_Rect*>(CGameInstance::GetInstance()->Clone_Object(
			TEXT("Prototype_GameObject_ParticleRect"),
			m_Particles[iIndex].second));
		if (pTarget != nullptr)
			pRect->Set_Target(pTarget);
		if (!XMVector4Equal(vAxis, XMVectorZero()))
			pRect->Set_Rotation(fRadians, vAxis);
		if (!XMVector4Equal(vLook, XMVectorZero()))
			pRect->AdJustLook(vLook);
		CGameInstance::GetInstance()->CreateObject_Self(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Particle"), pRect);
		break;
	}
	case Client::PART_END:
		return E_FAIL;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffectManager::Generate_Distortion(const _int iIndex, const _float4 vStartpos)
{
	if (iIndex >= m_pDistortions.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CDistortionEffect::DISTORTIONEFFECT* Distortion = m_pDistortions[iIndex].get();
		Distortion->vStartpos = vStartpos;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Distortion"), TEXT("Prototype_GameObject_Distortion_Effect"), Distortion);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Lightning(const _int iIndex, const _float4 vStartpos)
{
	if (iIndex >= m_pLightnings.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CElectronic::ELECTRONICDESC* Lightning = m_pLightnings[iIndex].get();
		Lightning->vStartPos = vStartpos;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effects"), TEXT("Prototype_GameObject_Electronic_Effect"), Lightning);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Tornado(const _int iIndex, const _float4 vStartpos, CGameObject* pTarget)
{
	if (iIndex >= m_Tornados.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CTornadoEffect::TORNADODESC* pDesc = m_Tornados[iIndex].get();
		pDesc->vStartPos = vStartpos;
		pDesc->pTarget = pTarget;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effects"), TEXT("Prototype_GameObject_Tornado"), pDesc);

	}
	return S_OK;
}

HRESULT CEffectManager::Generate_HealEffect(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_Heals.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CHealEffect::HEALEFFECT* pDesc = m_Heals[iIndex].get();
		pDesc->ParentMat = BindMat;
		CGameInstance::GetInstance()->CreateObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("Layer_Effects"), TEXT("Prototype_GameObject_HealEffect"), pDesc);
	}
	return S_OK;
}

HRESULT CEffectManager::Generate_Lazer(const _int iIndex, const _float4x4* BindMat)
{
	if (iIndex >= m_Lazers.size())
	{
		MSG_BOX("없는 인덱스임");
		return S_OK;
	}
	else
	{
		CAndrasLazer::ANDRAS_LAZER_TOTALDESC* Desc = m_Lazers[iIndex].get();
		Desc->ShooterMat = BindMat;
		CGameInstance::GetInstance()->Add_CloneObject(CGameInstance::GetInstance()->Get_CurrentLevel(),
			TEXT("LayerLazer"), TEXT("Prototype_GameObject_AndrasLazerSpawner"), Desc);
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
		Load.read((char*)&readFile->Bloom, sizeof(_bool));
		Load.read((char*)&readFile->eFuncType, sizeof(TRAILFUNCTYPE));
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
		inFile.read((char*)&readFile->isDestortion, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vColor, sizeof(_float3));
		inFile.read((char*)&readFile->vBloomColor, sizeof(_float3));
		inFile.read((char*)&readFile->fBloomPower, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_pSwordTrailes.emplace_back(readFile);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Particles()
{
	string finalPath = "../Bin/BinaryFile/Effect/Particles.Bin";
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
		PARTICLETYPE type;
		inFile.read((char*)&type, sizeof(PARTICLETYPE));
		switch (type)
		{
		case Client::PART_POINT:
		{
			CParticle_Point::PARTICLEPOINT* Arg = new CParticle_Point::PARTICLEPOINT;
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_POINT;
			m_Particles.emplace_back(make_pair(PART_POINT, Arg));
			break;
		}
		case Client::PART_MESH:
		{
			CParticleMesh::PARTICLEMESH* Arg = new CParticleMesh::PARTICLEMESH();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			inFile.read((char*)&Arg->eModelType, sizeof(EFFECTMODELTYPE));
			Arg->particleType = PART_MESH;
			m_Particles.emplace_back(make_pair(PART_MESH, Arg));
			break;
		}
		case Client::PART_RECT:
		{
			CParticle_Rect::PARTICLERECT* Arg = new CParticle_Rect::PARTICLERECT();
			inFile.read((char*)&Arg->SuperDesc, sizeof(CParticle::PARTICLEDESC));
			Arg->SuperDesc.vStartPos = { 0.f,0.f,0.f,1.f };
			Arg->Texture = load_wstring_from_stream(inFile);
			Arg->TexturePath = load_wstring_from_stream(inFile);
			Add_Texture_Prototype(Arg->TexturePath, Arg->Texture);
			Arg->particleType = PART_RECT;
			m_Particles.emplace_back(make_pair(PART_RECT, Arg));
			break;
		}
		default:
			break;
		}
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Distortions()
{
	string finalPath = "../Bin/BinaryFile/Effect/Distortion.Bin";
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
		shared_ptr<CDistortionEffect::DISTORTIONEFFECT> readFile = make_shared<CDistortionEffect::DISTORTIONEFFECT>();
		inFile.read((char*)&readFile->bFuncType, sizeof(_bool));
		inFile.read((char*)&readFile->bDisolve, sizeof(_bool));
		inFile.read((char*)&readFile->iDesolveNum, sizeof(_int));
		inFile.read((char*)&readFile->vStartScale, sizeof(_float2));
		inFile.read((char*)&readFile->vStartpos, sizeof(_float4));
		inFile.read((char*)&readFile->fSpeed, sizeof(_float));
		inFile.read((char*)&readFile->fLifeTime, sizeof(_float));
		readFile->Texture = load_wstring_from_stream(inFile);
		readFile->TexturePath = load_wstring_from_stream(inFile);
		Add_Texture_Prototype(readFile->TexturePath, readFile->Texture);
		m_pDistortions.emplace_back(readFile);
	}
	inFile.close();

	return S_OK;
}

HRESULT CEffectManager::Load_Lightnings()
{
	string finalPath = "../Bin/BinaryFile/Effect/Lightning.Bin";
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
		CElectronic::ELECTRONICDESC readFile{};
		inFile.read((char*)&readFile, sizeof(CElectronic::ELECTRONICDESC));
		shared_ptr<CElectronic::ELECTRONICDESC> StockValue = make_shared<CElectronic::ELECTRONICDESC>(readFile);
		m_pLightnings.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Tornados()
{
	string finalPath = "../Bin/BinaryFile/Effect/Tornados.Bin";
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
		CTornadoEffect::TORNADODESC readFile{};
		inFile.read((char*)&readFile, sizeof(CTornadoEffect::TORNADODESC));
		readFile.pTarget = nullptr;
		shared_ptr<CTornadoEffect::TORNADODESC> StockValue = make_shared<CTornadoEffect::TORNADODESC>(readFile);
		m_Tornados.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Heals()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Heals.Bin";
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
		CHealEffect::HEALEFFECT readFile{};
		inFile.read((char*)&readFile, sizeof(CHealEffect::HEALEFFECT));
		readFile.ParentMat = nullptr;
		shared_ptr<CHealEffect::HEALEFFECT> StockValue = make_shared<CHealEffect::HEALEFFECT>(readFile);
		m_Heals.emplace_back(StockValue);
	}
	inFile.close();
	return S_OK;
}

HRESULT CEffectManager::Load_Lazers()
{
	string finalPath = "../../Client/Bin/BinaryFile/Effect/Lazer.Bin";
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
		CAndrasLazer::ANDRAS_LAZER_TOTALDESC readFile{};
		inFile.read((char*)&readFile, sizeof(CAndrasLazer::ANDRAS_LAZER_TOTALDESC));
		readFile.ShooterMat = nullptr;
		shared_ptr<CAndrasLazer::ANDRAS_LAZER_TOTALDESC> StockValue = make_shared<CAndrasLazer::ANDRAS_LAZER_TOTALDESC>(readFile);
		m_Lazers.emplace_back(StockValue);
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

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Distortion_Effect"),
		CDistortionEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Fire_Effect"),
		CFireEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Electronic_Effect"),
		CElectronic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado"),
		CTornadoEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Wind"),
		CTornado_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Root"),
		CTornado_Root::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Tornado_Ring"),
		CTornado_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Heal
	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect"),
		CHealEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Ribbon"),
		CHeal_Ribbon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Spiral"),
		CHeal_Spiral::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_HealEffect_Line"),
		CHeal_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_LazerBase"),
		CAndrasLazerBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_LazerCylinder"),
		CAndrasLazerCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_Andras_Screw"),
		CAndrasScrew::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ElectricCylinder"),
		CElectricCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasRain"),
		CAndrasRain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_AndrasLazerSpawner"),
		CAndrasLazer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_LazerCast"),
		CLazerCast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CGameInstance::GetInstance()->Add_Prototype(TEXT("Prototype_GameObject_ShieldShpere"),
		CShieldSphere::Create(m_pDevice, m_pContext))))
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
