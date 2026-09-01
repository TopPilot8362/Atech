#pragma once
// ============================================================================
// CDMlinker.h - Links CDM to Atech
// Uses CDM and vIFF sectors to attain data
// ============================================================================
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <unordered_map>

// CDM data to the panel
struct CDMData
{
    std::string callsign;
    std::string departure;
    std::string destination;
    bool        DepartureNow = false;

    // Expected TOBT
    std::time_t tobtUtc = 0;

    // Sequence position for the push
    int PushSequence = 0;

    // Delay due to ATNS or CDM data
    int MinutesOfDelay = 0;
};

// Airport CDM 
struct AirportCDM
{
    std::string airport;
    int         totalOut = 0;
};

// ======================================================================================
class CCDMLinker
{
    public:

    // Called by plugin when TOBT changes
    void UpdateTOBT(const CDMData& data);

    // Return CDM Data
    AirportCDM GetCDM(const std::string& airport) const;

    // Returns CDM by timing (earliest to latest)
    std::vector<CDMData> GetCDMDataForAirport(
        const std::string& airport) const;
    
    private:
     void RebuildCDM(const std::string& airport);

     // Callsign linked with TOBT
     std::unordered_map<std::string, CDMData> m_callsign;

};