#pragma once

struct SectorPortalStructure
{
    xr_vector<CPortal*> Portals;
    xr_vector<CSector*> Sectors;

    void load(const xr_vector<CSector::level_sector_data_t>& sectors, const xr_vector<CPortal::level_portal_data_t>& portals);
    void unload();
};
