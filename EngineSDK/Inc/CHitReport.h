#pragma once
#include"Base.h"




//���� �������� �浹�� �����ؾ� �ұ�?? �Ѵٸ� �ش� Ŭ������ ����ϸ� �� ��.
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
