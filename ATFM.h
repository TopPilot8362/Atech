#pragma once
// ===================================================================
// ATFM.h 
//
// Provides the Enroute Air traffic management service
//
//=====================================================================
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <chrono>

// ATFM Panel data
struct ATFMData
{
    std::string callsign;
    std::string FIR;
    std::string Destination;
    bool        EnrouteNow = true;

    // Expected Arrival time
    std::time_t ETAUtc = 0;

    // Sequence for FIR Entry (applies to FACA and FAJA entries only, external flights covered but not FIR domestic)
    std::time_t EntUtc = 0;

    // Delay minutes
    int MinutesOfDelay = 0;
};

// Airspace Notices
struct ATFM
{
    std::string Airspace;
    int         TotalIn = 0;
    int         TotalOut = 0;
};

// ====================================================================
class CATFMClass
{
    public:

    // Called when ETA of Entry changes
    void UpdateENT(const ATFMData& data);

     // Return ETA Data
    ATFM GetAirspaceData(const std::string& airspace) const;

    // Returns ATFM by timing (earliest to latest)
    std::vector<ATFMData> GetCDMDataForAirspace(
        const std::string& airspace) const;

    private:
     void RebuildATFM(const std::string& airspace);

     // Callsign linked with TOBT
    std::unordered_map<std::string, ATFMData> m_callsign;
};