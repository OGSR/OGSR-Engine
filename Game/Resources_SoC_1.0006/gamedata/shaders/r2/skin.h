#ifndef	_SKIN_H
#define _SKIN_H

/*
KRodin: дефайн QUATERNION_SKINNING раскомментировать ТОЛЬКО для экспериментальной поддержки до 107 костей в моделях.
!!!При этом необходимо запускать движок с ключём -enable_qt_skinning
Работает пока глючно, поэтому так.
А по умолчанию лимит по костям - вроде как 71
*/
//#define QUATERNION_SKINNING

#ifdef QUATERNION_SKINNING
#	include "skin_q.h"
#else
#	include "skin_m.h"
#endif

#endif
