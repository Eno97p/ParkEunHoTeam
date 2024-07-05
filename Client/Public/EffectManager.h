#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Particle_Trail.h"
#include "Particle_Rect.h"
#include "ParticleMesh.h"
#include "Particle_Point.h"

BEGIN(Client)
class CEffectManager final : public CBase
{
private:
	CEffectManager();
	virtual ~CEffectManager() = default;


	vector<pair<PARTICLETYPE, void*>>		m_Particles;
	vector<shared_ptr<CParticle_Trail::TRAIL_DESC>> Trailes;
	vector<string> ParticleNames;
	vector<string> TrailEffectsNames;

public:
	virtual void Free() override;
};
END
