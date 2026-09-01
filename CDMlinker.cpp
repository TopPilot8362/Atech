// ============================================================================
// CDMlinker.cpp - Links CDM to Atech
// Uses CDM and vIFF sectors to attain data
// ============================================================================
#include "CDMlinker.h"
#include <algorithm>

// ---------------------------------------------------------------------------
// Called by plugin when TOBT changes for a given callsign.
// Inserts or updates the entry in the map, then rebuilds the airport bucket.
// ---------------------------------------------------------------------------
void CCDMLinker::UpdateTOBT(const CDMData& data)
{
    m_callsign[data.callsign] = data;
    RebuildCDM(data.departure);
}

// ---------------------------------------------------------------------------
// Returns the aggregate CDM summary for a given airport.
// ---------------------------------------------------------------------------
AirportCDM CCDMLinker::GetCDM(const std::string& airport) const
{
    AirportCDM result;
    result.airport  = airport;
    result.totalOut = 0;

    for (const auto& [cs, entry] : m_callsign)
    {
        if (entry.departure == airport)
            ++result.totalOut;
    }

    return result;
}

// ---------------------------------------------------------------------------
// Returns all CDMData entries for an airport, sorted earliest TOBT first.
// ---------------------------------------------------------------------------
std::vector<CDMData> CCDMLinker::GetCDMDataForAirport(const std::string& airport) const
{
    std::vector<CDMData> results;

    for (const auto& [cs, entry] : m_callsign)
    {
        if (entry.departure == airport)
            results.push_back(entry);
    }

    std::sort(results.begin(), results.end(),
        [](const CDMData& a, const CDMData& b)
        {
            return a.tobtUtc < b.tobtUtc;
        });

    return results;
}

// ---------------------------------------------------------------------------
// Rebuilds push-sequence numbers for every aircraft departing the airport,
// ordered by TOBT ascending.  Sequence starts at 1.
// ---------------------------------------------------------------------------
void CCDMLinker::RebuildCDM(const std::string& airport)
{
    // Collect pointers to all entries for this airport
    std::vector<CDMData*> bucket;
    for (auto& [cs, entry] : m_callsign)
    {
        if (entry.departure == airport)
            bucket.push_back(&entry);
    }

    // Sort by TOBT ascending
    std::sort(bucket.begin(), bucket.end(),
        [](const CDMData* a, const CDMData* b)
        {
            return a->tobtUtc < b->tobtUtc;
        });

    // Assign sequence positions
    int seq = 1;
    for (CDMData* entry : bucket)
        entry->PushSequence = seq++;
}
