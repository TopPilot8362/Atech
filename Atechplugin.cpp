// =============================================================================
// AtechPlugin.cpp - Atech plugin for Euroscope
// Version 1.0.0
// =============================================================================
#include "AtechPlugin.h"

static std::string GetPluginDirectlyModifiedData(
    const EuroScopePlugIn::CFlightPlan& fp);

CAtechPlugin::CAtechPlugin()
    : EuroScopePlugIn::CPlugIn(
        EuroScopePlugIn::COMPATIBILITY_CODE,
        PLUGIN_NAME,
        PLUGIN_VERSION,
        PLUGIN_AUTHOR,
        PLUGIN_COPYRIGHT)
{
}

CAtechPlugin::~CAtechPlugin() = default;

void CAtechPlugin::OnGetTagItem(
    EuroScopePlugIn::CFlightPlan,
    EuroScopePlugIn::CRadarTarget,
    int,
    int,
    char*,
    int*,
    COLORREF*,
    double*)
{
}

void CAtechPlugin::OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget rt)
{
    auto fp = rt.GetCorrelatedFlightPlan();
    if (!fp.IsValid())
        return;

    const std::string cs          = fp.GetCallsign();
    const std::string destination = fp.GetFlightPlanData().GetDestination();

    CDMData cdm;
    cdm.callsign     = cs;
    cdm.departure    = fp.GetFlightPlanData().GetOrigin();
    cdm.destination  = destination;
    cdm.DepartureNow = fp.GetClearenceFlag();
    m_Cdm.UpdateTOBT(cdm);

    ATFMData atfm;
    atfm.callsign    = cs;
    atfm.FIR         = GetPluginDirectlyModifiedData(fp);
    atfm.Destination = destination;
    atfm.EnrouteNow  = rt.GetPosition().IsValid();
    m_Atfm.UpdateENT(atfm);
}

void CAtechPlugin::OnFlightPlanFlightPlanDataUpdate(
    EuroScopePlugIn::CFlightPlan fp)
{
}

void CAtechPlugin::OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan fp)
{
}

void CAtechPlugin::OnFunctionCall(int functionId,
                                  const char* sItemString,
                                  POINT pt,
                                  RECT area)
{
    switch (functionId)
    {
        default:
            break;
    }
}

static std::string GetPluginDirectlyModifiedData(
    const EuroScopePlugIn::CFlightPlan& fp)
{
    return "UNKNOWN";
}

CATFMClass& CAtechPlugin::GetAirspaceData()
{
    return m_Atfm;
}

CCDMLinker& CAtechPlugin::GetCDM()
{
    return m_Cdm;
}

EuroScopePlugIn::CRadarScreen* CAtechPlugin::OnRadarScreenCreated(
    const char* sDisplayName,
    bool NeedRadarContent,
    bool GeoReferenced,
    bool CanBeSaved,
    bool CanBeCreated)
{
    return nullptr;
}

CAtechPlugin* g_plugin = nullptr;

void __declspec(dllexport) EuroScopePlugInInit(
    EuroScopePlugIn::CPlugIn** ppPlugin)
{
    *ppPlugin = g_plugin = new CAtechPlugin();
}

void __declspec(dllexport) EuroScopePlugInExit()
{
    delete g_plugin;
    g_plugin = nullptr;
}
