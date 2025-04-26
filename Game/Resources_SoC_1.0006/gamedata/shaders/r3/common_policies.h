#ifndef common_policies_h_included
#define common_policies_h_included

// Define default sample index for MSAA
#ifndef ISAMPLE
#define ISAMPLE 0
#endif // ISAMPLE

/////////////////////////////////////////////////////////////////////////////
// GLD_P - gbuffer_load_data
#define GLD_P(_tc, _pos2d, _iSample) _tc, _pos2d, _iSample

// GLDO_P - gbuffer_load_data_offset
#define GLDO_P(_tc, _tc2, _pos2d, _iSample) _tc, _tc2, _pos2d, _iSample

/////////////////////////////////////////////////////////////////////////////
// CS_P
#define CS_P(_P, _N, _tc0, _tcJ, _pos2d, _iSample) _P, _N, _tc0, _tcJ, _pos2d

#endif // common_policies_h_included