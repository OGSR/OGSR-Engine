#ifndef models_detector_cfg
#define models_detector_cfg

// ДЕТЕКТОР "ВЕЛЕС"
#define ID_DETECTOR_3_DETECT_RADIUS float(35.0) // радиус детектирования артефактов. Менять одновременно вместе со значением в конфиге
#define ID_DETECTOR_3_COLOR float4(0.1, 1.0, 0.0, 1.0) // цвет индикации
#define ID_DETECTOR_3_POWER float(6.0) // яркость индикации
#define ID_DETECTOR_3_DOT_RADIUS float(0.01) // радиус точки артефакта на экране
#define ID_DETECTOR_3_SCREEN_CORNERS float4(0.4668, 0.8398, 0.1035, 0.2891) // текстурные координаты углов экрана в формате (max u,max v,min u,min v). Менять при смене текстуры
#define USE_ANOMALY_DETECTION // включение режима индикации аномалий
#define ID_DETECTOR_3_AN_COLOR float4(1.0, 0.0, 0.0, 1.0) // цвет индикации аномалий
#define ID_DETECTOR_3_AN_DOT_RADIUS float(0.02) // радиус точки аномалии на экране
#define ID_DETECTOR_3_NUM_COLOR float4(1.0, 0.0, 0.0, 1.0) // цвет индикации цифр на экране

#endif
