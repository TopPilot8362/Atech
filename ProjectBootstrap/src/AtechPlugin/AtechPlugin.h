#pragma once

#ifndef DllSpecEuroScope
#define DllSpecEuroScope
#endif

// =============================================================================
// AtechPlugin.h - Atech plugin for Euroscope
// Version 1.0.0
//
// Changes to be made:
//
// ==============================================================================
#include <Windows.h>
#include <EuroScopePlugIn.h>
#include <CDMlinker.h>
#include <ATFM.h>

#include <string>
#include <unordered_map>
#include <vector>

// -- Function IDs -----------------------------------------









// ==========================================================

// - Screen Object -------------------------------------------------
static constexpr int SCREEN_OBJ_BTN_TOGGLE = 1000; // Atech Toggle
static constexpr int SCREEN_OBJ_PANEL_DRAG = 1001; // Dragging the panel
static constexpr int SCREEN_OBJ_CDM_STATUS = 1002; // CDM Status 
static constexpr int SCREEN_OBJ_ATFM_STATUS = 1003; // Flow Status
static constexpr int SCREEN_OBJ_AIRSPACE_EFFIENCY = 1004; // Airspace Effiency

// - MetaData --------------------------------------------------------
static constexpr char PLUGIN_NAME[]     = "Atech ";
static constexpr char PLUGIN_VERSION[]  = "1.0.0";
static constexpr char PLUGIN_AUTHOR[]   = "Liam Garland";
static constexpr char PLUGIN_COPYRIGHT[] = "GPL V2";

// Callsigns status within the FIR
struct CallSignATFMStatus
{
    std::string        callsign;
    std::string        FIR;
    std::string        Destination;
    bool               EnrouteNow = true;
    int                MinutesOfDelay = 0;
    std::time_t EntUtc = 0;
    std::time_t ETAUtc = 0;
};

// Declaration
class CAtechPlugin;

// =======================================================================
// CAtechScreen - owns the gui 
// ========================================================================
class CAtechScreen : public EuroScopePlugIn::CRadarScreen
{
public:
    explicit CAtechScreen(CAtechPlugin* plugin);
    virtual ~CAtechScreen() = default;

    // CRadar overrides
    void OnRefresh(HDC hDC, int phase) override;
    void OnButtonDownScreenObject(int objectType, const char* sObjectId,
                                   POINT pt, RECT area, int button) override;
    void OnButtonUpScreenObject(int objectType, const char* sObjectId,
                                 POINT pt, RECT area, int button) override;
    void OnClickScreenObject(int objectType, const char* sObjectId,
                             POINT pt, RECT area, int button) override;
    void OnMoveScreenObject(int objectType, const char* sObjectId,
                            POINT pt, RECT area, bool released) override;
    // Required pure virtual - called when the ASR is being closed
    void OnAsrContentToBeClosed() override {}

    private:
        CAtechPlugin* m_Plugin;                    //pointer non owned

        // State
        bool          m_PanelVis    = false;
        int           m_ActiveTab   = 0;
        
        // Atech button
        int      m_BtnX = 10;
        int      m_BtnY = 10;
        bool     m_BtnDragging = false;
        int      m_BtnDragOffsetX = 0;
        int      m_BtnDragOffsetY = 0;

        // helpers
         void DrawToggleButton(HDC hDC);
         void DrawPanel(HDC hDC);
         void DrawTitleBar(HDC hDC, RECT r);
         void DrawTabBar(HDC hDC, RECT r);
         void DrawStatusTab(HDC hDC, RECT r);
         void DrawAirportsTab(HDC hDC, RECT r);
         void DrawDemandTab(HDC hDC, RECT r);
         void DrawDashboardTab(HDC hDC, RECT r);

         // Filed rect
         RECT DrawButton(HDC hDC, int x, int y, int w, int h,
                    const char* label, bool active,
                    COLORREF bg, COLORREF fg, int objectId, const char* oid);
         
        // Thin wrapper: registers rect as draggable or clickable screen object
         void AddClickable(int objectId, const char* oid, RECT area, bool moveable = false);
};

// ===============================================================================
// CAtechPlugin
// ===============================================================================
class CAtechPlugin : public EuroScopePlugIn::CPlugIn
{
public:
    CAtechPlugin();
    virtual ~CAtechPlugin();

         // ES Overrides
          void OnGetTagItem(
        EuroScopePlugIn::CFlightPlan   fp,
        EuroScopePlugIn::CRadarTarget  rt,
        int                            itemCode,
        int                            tagData,
        char                          *sItemString,
        int                           *pColorCode,
        COLORREF                      *pRGB,
        double                        *pFontSize) override;

    void OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget rt) override;

    void OnFunctionCall(
        int                            functionId,
        const char                    *sItemString,
        POINT                          pt,
        RECT                           area) override;

    void OnFlightPlanFlightPlanDataUpdate(
        EuroScopePlugIn::CFlightPlan   fp) override;

    void OnFlightPlanDisconnect(
        EuroScopePlugIn::CFlightPlan   fp) override;

    // NOTE: OnCompileCommand has been intentionally removed.
    
    EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(
        const char* sDisplayName,
        bool        NeedRadarContent,
        bool        GeoReferenced,
        bool        CanBeSaved,
        bool        CanBeCreated) override;
    
    // Public Accessors used by CAtechScreen
    CATFMClass&                        GetAirspaceData();
    CCDMLinker&                        GetCDM();

private:
    // -- Sub -----------------------------------------
    CCDMLinker       m_Cdm;
    CATFMClass       m_Atfm;

    std::unordered_map<std::string, ATFMData> m_callsign;

     // Non owning pointers
     std::vector<CAtechScreen*>  m_Screens;

     // -- Helpers ---------------------------------------
     std::string                  BuildTagString(const std::string& callsign);
};

// ── DLL entry points ──────────────────────────────────────────
// extern "C" is NOT needed here — EuroScopePlugIn.h already
// declares these with C linkage internally. Adding it again
// causes C2732 "linkage specification contradicts earlier spec".
void __declspec(dllexport) EuroScopePlugInInit(
    EuroScopePlugIn::CPlugIn **ppPlugInInstance);
void __declspec(dllexport) EuroScopePlugInExit();