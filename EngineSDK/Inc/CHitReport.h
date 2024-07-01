#pragma once
#include"Base.h"




//직접 물리적인 충돌을 제어해야 할까?? 한다면 해당 클래스를 사용하면 될 듯.
BEGIN(Engine)
class  CHitReport final:public PxUserControllerHitReport
{
private:
	explicit CHitReport();
	explicit CHitReport(const CHitReport& rhs);
	virtual ~CHitReport() = default;


public:
	HRESULT Initialize_Prototype();


public:
	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;


public:
	static CHitReport* Create();

};

END
