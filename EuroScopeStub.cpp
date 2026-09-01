#ifndef DllSpecEuroScope
#define DllSpecEuroScope
#endif

#include "EuroScopePlugIn.h"

namespace EuroScopePlugIn {

const char* CFlightPlan::GetCallsign() const { return ""; }
bool CFlightPlan::GetClearenceFlag() const { return false; }
CFlightPlanData CFlightPlan::GetFlightPlanData() const { return {}; }

const char* CFlightPlanData::GetOrigin() const { return ""; }
const char* CFlightPlanData::GetDestination() const { return ""; }

CRadarTargetPositionData CRadarTarget::GetPosition() const { return {}; }
CFlightPlan CRadarTarget::GetCorrelatedFlightPlan() const { return {}; }

CPlugIn::CPlugIn(int, const char*, const char*, const char*, const char*) {}
CPlugIn::~CPlugIn() = default;

} // namespace EuroScopePlugIn
