// [ SETTINGS ] [ AMBIENT OCCLUSION ]

#define G_SSDO_RENDER_DIST 150.0f // Max rendering distance.

#define G_SSDO_RADIUS 0.2f // AO radius, higher values means more occlusion coverage with less detail and intensity. ( Recommended 0.1 ~ 0.5 )
#define G_SSDO_INTENSITY 8.0f // General AO intensity.

#define G_SSDO_MAX_OCCLUSION 0.1f // Maximum obscurance for a pixel. 0 = full black
#define G_SSDO_SMOOTH 1.3f // AO softer. Highly occluded pixels will be more affected than low occluded ones.

#define G_SSDO_WEAPON_LENGTH 1.5f // Maximum distance to apply G_SSDO_WEAPON_RADIUS, G_SSDO_WEAPON_INTENSITY, etc.
#define G_SSDO_WEAPON_RADIUS 0.02f // Weapon radius, higher values means more occlusion coverage with less details.
#define G_SSDO_WEAPON_INTENSITY 0.4f // Weapon intensity. [ 1.0f = 100% ]

//#define G_SSDO_DETAILED_SEARCH					// Add an extra check to the AO search. By default more detailed. ( 20% of G_SSDO_RADIUS for Scenary and 50% for weapons )
#define G_SSDO_DETAILED_RADIUS 0.2f // Detailed search radius. [ 1.0f = 100% ]
#define G_SSDO_DETAILED_WEAPON_RADIUS 0.5f // Detailed serach radius for weapons. [ 1.0f = 100% ]