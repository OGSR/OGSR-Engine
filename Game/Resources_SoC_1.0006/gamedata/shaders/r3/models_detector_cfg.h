#ifndef models_detector_cfg
#define models_detector_cfg

// Интерактивные детекторы

// ДЕТЕКТОР "ОТКЛИК"
#define ID_DETECTOR_1_DETECT_RADIUS float(20.0)			// радиус детектирования артефактов. Менять одновременно вместе со значением в конфиге
#define ID_DETECTOR_1_COLOR float4(1.0,1.0,1.0,1.0)		// цвет индикации
#define ID_DETECTOR_1_POWER float(6.0)					// яркость индикации

// ДЕТЕКТОР "МЕДВЕДЬ"
#define ID_DETECTOR_2_DETECT_RADIUS float(20.0)			// радиус детектирования артефактов. Менять одновременно вместе со значением в конфиге
#define ID_DETECTOR_2_COLOR float4(0.1,1.0,0.0,1.0)		// цвет индикации
#define ID_DETECTOR_2_POWER float(5.0)					// яркость индикации
#define ID_DETECTOR_2_CENTER float2(0.2559, 0.2305)		// текстурные координаты центра экрана детектора. Менять при смене текстуры
#define ID_DETECTOR_2_SECTOR float(0.7)					// сектор индикации на экране. Чем больше значение, тем меньше сектор. Ровно 1.0 не ставить!

// ДЕТЕКТОР "ВЕЛЕС"
#define ID_DETECTOR_3_DETECT_RADIUS float(35.0)			// радиус детектирования артефактов. Менять одновременно вместе со значением в конфиге
#define ID_DETECTOR_3_COLOR float4(0.1,1.0,0.0,1.0)		// цвет индикации
#define ID_DETECTOR_3_POWER float(6.0)					// яркость индикации
#define ID_DETECTOR_3_DOT_RADIUS float(0.01)			// радиус точки артефакта на экране
#define ID_DETECTOR_3_SCREEN_CORNERS float4(0.4668, 0.8398, 0.1035, 0.2891)		// текстурные координаты углов экрана в формате (max u,max v,min u,min v). Менять при смене текстуры
#define USE_ANOMALY_DETECTION							// включение режима индикации аномалий
#define ID_DETECTOR_3_AN_COLOR float4(1.0,0.0,0.0,1.0)	// цвет индикации аномалий
#define ID_DETECTOR_3_AN_DOT_RADIUS float(0.02)			// радиус точки аномалии на экране
#define ID_DETECTOR_3_NUM_COLOR float4(1.0,0.0,0.0,1.0)	// цвет индикации цифр на экране
//#define USE_WAVE //Недоделанный эффект волны? Надо к нему сделать текстуру ogse\\detector_wave

#endif
