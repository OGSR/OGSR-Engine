#ifndef _AL_EAXPRESETS_H_
#define _AL_EAXPRESETS_H_

#include "AL/al.h"

#ifdef __cplusplus
extern "C" {
#endif

EAXLISTENERPROPERTIES EAX20Preset[] =
{
	{
		// environment 0: default/generic
		-1000,	// Room
		-100,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.83f,	// Decay HF Ratio
		-2602,	// Reflections
		0.007f,	// Reflections Delay
		200,	// Reverb
		0.011f,	// Reverb Delay
		0,		// Environment
		7.5f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 1: padded cell
		-1000,	// Room
		-6000,	// Room HF
		0.0f,	// Room RolloffFactor
		0.17f,	// Decay Time
		0.10f,	// Decay HF Ratio
		-1204,	// Reflections
		0.001f,	// Reflections Delay
		207,	// Reverb
		0.002f,	// Reverb Delay
		1,		// Environment
		1.4f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 2: room
		-1000,	// Room
		-454,	// Room HF
		0.0f,	// Room RolloffFactor
		0.40f,	// Decay Time
		0.83f,	// Decay HF Ratio
		-1646,	// Reflections
		0.002f,	// Reflections Delay
		53,		// Reverb
		0.003f,	// Reverb Delay
		2,		// Environment
		1.9f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 3: bathroom
		-1000,	// Room
		-1200,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.54f,	// Decay HF Ratio
		-370,	// Reflections
		0.007f,	// Reflections Delay
		1030,	// Reverb
		0.011f,	// Reverb Delay
		3,		// Environment
		1.4f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 4: living room
		-1000,	// Room
		-6000,	// Room HF
		0.0f,	// Room RolloffFactor
		0.50f,	// Decay Time
		0.10f,	// Decay HF Ratio
		-1376,	// Reflections
		0.003f,	// Reflections Delay
		-1104,	// Reverb
		0.004f,	// Reverb Delay
		4,		// Environment
		2.5f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 5: stone room
		-1000,	// Room
		-300,	// Room HF
		0.0f,	// Room RolloffFactor
		2.31f,	// Decay Time
		0.64f,	// Decay HF Ratio
		-711,	// Reflections
		0.012f,	// Reflections Delay
		83,		// Reverb
		0.017f,	// Reverb Delay
		5,		// Environment
		11.6f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 6: auditorium
		-1000,	// Room
		-476,	// Room HF
		0.0f,	// Room RolloffFactor
		4.32f,	// Decay Time
		0.59f,	// Decay HF Ratio
		-789,	// Reflections
		0.020f,	// Reflections Delay
		-289,	// Reverb
		0.030f,	// Reverb Delay
		6,		// Environment
		21.6f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 7: concert hall
		-1000,	// Room
		-500,	// Room HF
		0.0f,	// Room RolloffFactor
		3.92f,	// Decay Time
		0.70f,	// Decay HF Ratio
		-1230,	// Reflections
		0.020f,	// Reflections Delay
		-2,		// Reverb
		0.029f,	// Reverb Delay
		7,		// Environment
		19.6f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 8: cave
		-1000,	// Room
		0,		// Room HF
		0.0f,	// Room RolloffFactor
		2.91f,	// Decay Time
		1.30f,	// Decay HF Ratio
		-602,	// Reflections
		0.015f,	// Reflections Delay
		-302,	// Reverb
		0.022f,	// Reverb Delay
		8,		// Environment
		14.6f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	},
	{
		// environment 9: arena
		-1000,	// Room
		-698,	// Room HF
		0.0f,	// Room RolloffFactor
		7.24f,	// Decay Time
		0.33f,	// Decay HF Ratio
		-1166,	// Reflections
		0.020f,	// Reflections Delay
		16,		// Reverb
		0.030f,	// Reverb Delay
		9,		// Environment
		36.2f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 10: hangar
		-1000,	// Room
		-1000,	// Room HF
		0.0f,	// Room RolloffFactor
		10.05f,	// Decay Time
		0.23f,	// Decay HF Ratio
		-602,	// Reflections
		0.020f,	// Reflections Delay
		198,	// Reverb
		0.030f,	// Reverb Delay
		10,		// Environment
		50.3f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 11: carpeted hallway
		-1000,	// Room
		-4000,	// Room HF
		0.0f,	// Room RolloffFactor
		0.30f,	// Decay Time
		0.10f,	// Decay HF Ratio
		-1831,	// Reflections
		0.002f,	// Reflections Delay
		-1630,	// Reverb
		0.030f,	// Reverb Delay
		11,		// Environment
		1.9f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 12: hallway
		-1000,	// Room
		-300,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.59f,	// Decay HF Ratio
		-1219,	// Reflections
		0.007f,	// Reflections Delay
		441,	// Reverb
		0.011f,	// Reverb Delay
		12,		// Environment
		1.8f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 13: stone corridor
		-1000,	// Room
		-237,	// Room HF
		0.0f,	// Room RolloffFactor
		2.70f,	// Decay Time
		0.79f,	// Decay HF Ratio
		-1214,	// Reflections
		0.013f,	// Reflections Delay
		395,	// Reverb
		0.020f,	// Reverb Delay
		13,		// Environment
		13.5f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 14: alley
		-1000,	// Room
		-270,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.86f,	// Decay HF Ratio
		-1204,	// Reflections
		0.007f,	// Reflections Delay
		-4,		// Reverb
		0.011f,	// Reverb Delay
		14,		// Environment
		7.5f,	// Environment Size
		0.30f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 15: forest
		-1000,	// Room
		-3300,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.54f,	// Decay HF Ratio
		-2560,	// Reflections
		0.162f,	// Reflections Delay
		-229,	// Reverb
		0.088f,	// Reverb Delay
		15,		// Environment
		38.0f,	// Environment Size
		0.30f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 16: city
		-1000,	// Room
		-800,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.67f,	// Decay HF Ratio
		-2273,	// Reflections
		0.007f,	// Reflections Delay
		-1691,	// Reverb
		0.011f,	// Reverb Delay
		16,		// Environment
		7.5f,	// Environment Size
		0.50f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 17: mountains
		-1000,	// Room
		-2500,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.21f,	// Decay HF Ratio
		-2780,	// Reflections
		0.300f,	// Reflections Delay
		-1434,	// Reverb
		0.100f,	// Reverb Delay
		17,		// Environment
		100.0f,	// Environment Size
		0.27f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	},
	{
		// environment 18: quarry
		-1000,	// Room
		-1000,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.83f,	// Decay HF Ratio
		-10000,	// Reflections
		0.061f,	// Reflections Delay
		500,	// Reverb
		0.025f,	// Reverb Delay
		18,		// Environment
		17.5f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 19: plain
		-1000,	// Room
		-2000,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.50f,	// Decay HF Ratio
		-2466,	// Reflections
		0.179f,	// Reflections Delay
		-1926,	// Reverb
		0.100f,	// Reverb Delay
		19,		// Environment
		42.5f,	// Environment Size
		0.21f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 20: parking lot
		-1000,	// Room
		0,		// Room HF
		0.0f,	// Room RolloffFactor
		1.65f,	// Decay Time
		1.50f,	// Decay HF Ratio
		-1363,	// Reflections
		0.008f,	// Reflections Delay
		-1153,	// Reverb
		0.012f,	// Reverb Delay
		20,		// Environment
		8.3f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	},
	{
		// environment 21: sewerpipe
		-1000,	// Room
		-1000,	// Room HF
		0.0f,	// Room RolloffFactor
		2.81f,	// Decay Time
		0.14f,	// Decay HF Ratio
		429,	// Reflections
		0.014f,	// Reflections Delay
		1023,	// Reverb
		0.021f,	// Reverb Delay
		21,		// Environment
		1.7f,	// Environment Size
		0.80f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 22: underwater
		-1000,	// Room
		-4000,	// Room HF
		0.0f,	// Room RolloffFactor
		1.49f,	// Decay Time
		0.10f,	// Decay HF Ratio
		-449,	// Reflections
		0.007f,	// Reflections Delay
		1700,	// Reverb
		0.011f,	// Reverb Delay
		22,		// Environment
		1.8f,	// Environment Size
		1.00f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x3F,	// Flags
	},
	{
		// environment 23: drugged
		-1000,	// Room
		0,		// Room HF
		0.0f,	// Room RolloffFactor
		8.39f,	// Decay Time
		1.39f,	// Decay HF Ratio
		-115,	// Reflections
		0.002f,	// Reflections Delay
		985,	// Reverb
		0.030f,	// Reverb Delay
		23,		// Environment
		1.9f,	// Environment Size
		0.50f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	},
	{
		// environment 24: dizzy
		-1000,	// Room
		-400,	// Room HF
		0.0f,	// Room RolloffFactor
		17.23f,	// Decay Time
		0.56f,	// Decay HF Ratio
		-1713,	// Reflections
		0.020f,	// Reflections Delay
		-613,	// Reverb
		0.030f,	// Reverb Delay
		24,		// Environment
		1.8f,	// Environment Size
		0.60f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	},
	{
		// environment 25: psychotic
		-1000,	// Room
		-151,	// Room HF
		0.0f,	// Room RolloffFactor
		7.56f,	// Decay Time
		0.91f,	// Decay HF Ratio
		-626,	// Reflections
		0.020f,	// Reflections Delay
		774,	// Reverb
		0.030f,	// Reverb Delay
		25,		// Environment
		1.0f,	// Environment Size
		0.50f,	// Environment Diffusion
		-5.00f,	// Air Absorption HF
		0x1F,	// Flags
	}
};

#ifdef __cplusplus
}
#endif

#endif
