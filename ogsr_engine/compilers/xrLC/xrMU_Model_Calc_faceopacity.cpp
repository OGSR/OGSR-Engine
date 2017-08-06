#include "stdafx.h"

void xrMU_Model::calc_faceopacity()
{
	for (v_faces_it I=m_faces.begin(); I!=m_faces.end(); I++) (*I)->CacheOpacity();
}
