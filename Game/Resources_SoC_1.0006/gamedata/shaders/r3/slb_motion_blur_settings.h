#ifndef SLB_MOTION_BLUR_SETTINGS_H
#define SLB_MOTION_BLUR_SETTINGS_H

#define SLB_MBLUR_SAMPLES int(4) /// Blur samples     /// default int(4)      /// vanilla int(6)
#define SLB_MBLUR_CLAMP float(0.1) /// Max blur length  /// default float(0.1)  /// vanilla float(0.012)
#define SLB_MBLUR_LENGTH float(120) /// Blur length      /// default float(120)  /// vanilla float(12)

#define SLB_MBLUR_ANIMATED_DITHER /// Comment to disable
#define SLB_MBLUR_CONE_DITHER float(0.0) /// Cone like dithering aka blurriness // default float(0.0)

#define SLB_MBLUR_DUAL /// Blur in both directions. Comment to disable
#define SLB_MBLUR_REVERSED /// Reverse blur direction.  Comment to disable

#define SLB_MBLUR_WPN /// Disabled motion blur for weapon and hud. Comment to disable
#define SLB_MBLUR_WPN_RADIUS float(1.3) /// default float(1.3)
#define SLB_MBLUR_WPN_RADIUS_SMOOTHING float(1.0) /// default float(1.0)

#define SLB_MBLUR_SIGHT_MASK /// Try its best to detect reticle but too many false positives
#define SLB_MBLUR_SIGHT_MASK_SIZE float(5.0) /// Radius for searching   /// default float(5.0)

#define SLB_MBLUR_SIGHT_MASK_COLOR /// Color based sight detection
#define SLB_MBLUR_SIGHT_MASK_COLOR_THRESHOLD float(0.5) /// Threshold for masking  /// default float(0.5)

// #define SLB_MBLUR_SIGHT_MASK_COLOR_SLOWER  /// Slower and maybe better
#define SLB_MBLUR_SIGHT_MASK_COLOR_SLOWER_THRESHOLD float(0.8) /// Threshold for masking  /// default float(0.8)

#endif /// SLB_MOTION_BLUR_SETTINGS_H
