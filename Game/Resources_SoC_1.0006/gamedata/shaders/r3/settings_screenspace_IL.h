// [ SETTINGS ] [ INDIRECT LIGHT ]

#define G_IL_QUALITY 1 // Quality of the IL [ 0 = Very Low | 1 = Low | 2 = Normal | 3 = High ]

#define G_IL_INTENSITY 0.6f // Intensity of the indirect ilumination
#define G_IL_SKYLIGHT_INTENSITY 0.5f // Intensity of the sky light
#define G_IL_COLOR_VIBRANCE 2.5f // Vibrance of the indirect ilumination

#define G_IL_RANGE 0.6f // Radius of the IL sampling [ Recommended 0.3f ~ 1.0f ]
#define G_IL_MAX_DIFFERENCE 10.0f // Maximum distance the sampled color can travel

#define G_IL_WEAPON_LENGTH 1.5f // Maximum lenght of the weapon
#define G_IL_WEAPON_RANGE 0.015f // Weapons radius for the IL sampling

#define G_IL_DISCARD_SAMPLE_AT \
    0.2f // If the intensity of the bounce is lower or equal to this value, discard the sample. ( 1.0f = 100% intensity )
         // The intensity of the bounce will vary depending on the angle of the surfaces and the distance.

//#define G_IL_DEBUG_MODE					// Uncomment if you want to check what the IL is doing