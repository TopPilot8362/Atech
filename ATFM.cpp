// ===================================================================
// ATFM.cpp
//
// Provides the Enroute Air Traffic Management service
// ===================================================================
#include "ATFM.h"
#include <algorithm>

// ---------------------------------------------------------------------------
// Called when the ETA/Entry time changes for a given callsign.
// Inserts or updates the entry in the map, then rebuilds the FIR bucket.
// ---------------------------------------------------------------------------
void CATFMClass::UpdateENT(const ATFMData& data)
{
    m_callsign[data.callsign] = data;
    RebuildATFM(data.FIR);
}

// ---------------------------------------------------------------------------
// Returns aggregate airspace data (total inbound / outbound) for the FIR.
// ---------------------------------------------------------------------------
ATFM CATFMClass::GetAirspaceData(const std::string& airspace) const
{
    ATFM result;
    result.Airspace  = airspace;
    result.TotalIn   = 0;
    result.TotalOut  = 0;

    for (const auto& [cs, entry] : m_callsign)
    {
        if (entry.FIR != airspace)
            continue;

        if (entry.EnrouteNow)
            ++result.TotalIn;   // Currently inside the FIR
        else
            ++result.TotalOut;  // Planned / not yet entered
    }

    return result;
}

// ---------------------------------------------------------------------------
// Returns all ATFMData entries for an airspace, sorted earliest EntUtc first.
// ---------------------------------------------------------------------------
std::vector<ATFMData> CATFMClass::GetCDMDataForAirspace(const std::string& airspace) const
{
    std::vector<ATFMData> results;

    for (const auto& [cs, entry] : m_callsign)
    {
        if (entry.FIR == airspace)
            results.push_back(entry);
    }

    std::sort(results.begin(), results.end(),
        [](const ATFMData& a, const ATFMData& b)
        {
            return a.EntUtc < b.EntUtc;
        });

    return results;
}

// ---------------------------------------------------------------------------
// Rebuilds internal state for the given airspace after any data change.
// Currently recalculates cumulative delay for sequencing purposes.
// ---------------------------------------------------------------------------
void CATFMClass::RebuildATFM(const std::string& airspace)
{
    // Collect pointers to every entry in this FIR
    std::vector<ATFMData*> bucket;
    for (auto& [cs, entry] : m_callsign)
    {
        if (entry.FIR == airspace)
            bucket.push_back(&entry);
    }

    // Sort by FIR entry time ascending
    std::sort(bucket.begin(), bucket.end(),
        [](const ATFMData* a, const ATFMData* b)
        {
            return a->EntUtc < b->EntUtc;
        });

    // Assign sequence-based delay in minutes (each slot = 1 minute separation)
    // Slot 0 has no delay; subsequent slots accumulate.
    for (int i = 0; i < static_cast<int>(bucket.size()); ++i)
        bucket[i]->MinutesOfDelay = i;  // Override with ATFM-calculated value
}
