#include "CHitReport.h"






CHitReport::CHitReport()
	:PxUserControllerHitReport()
{
}

CHitReport::CHitReport(const CHitReport& rhs)
	:PxUserControllerHitReport(rhs)
{
}

HRESULT CHitReport::Initialize_Prototype()
{
	return S_OK;
}

void CHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
}

void CHitReport::onControllerHit(const PxControllersHit& hit)
{
}

void CHitReport::onObstacleHit(const PxControllerObstacleHit& hit)
{
}

CHitReport* CHitReport::Create()
{
	CHitReport* pInstance = new CHitReport();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHitReport");
		return nullptr;
	}

	return pInstance;
}