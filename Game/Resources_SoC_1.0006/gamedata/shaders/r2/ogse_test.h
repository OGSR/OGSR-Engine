/*	Screen-Space Directional Occlusion.
Based on Tobias Ritschel tech paper.
Implemented by K.D. (OGSE team)
Last Changed: 17.11.2012
*/
#ifndef OGSE_TEST_H
#define OGSE_TEST_H
const float st = 1.f - 1.f/8.f;
   float n = 0;
   const float fScale = 0.025f; 
static const float3 arr1[8] = 
{
/*	float3(	-0.840144	,	-0.07358, 0),
	float3(	-0.691559	,	-0.59771, 0),
	float3(	-0.5		,	0.457137, 0),
	float3(	-0.426212	,	-0.40581, 0),
	float3(	-0.32194	,	-0.932615, 0),
	float3(	-0.203345	,	0.620716, 0),
	float3(	-0.1		,	-0.4, 0),
	float3(	0.185461	,	-0.893124, 0),
	float3(	0.37		,	-0.480026, 0),
	float3(	0.507431	,	0.064425, 0),
	float3(	0.61		,	0.767022, 0),
	float3(	0.89642		,	0.412458, 0),
	float3(	0.96234		,	-0.194983, 0)*/
		normalize(float3( 1, 1, 1))*fScale*(n+=st),
      normalize(float3(-1,-1,-1))*fScale*(n+=st),
      normalize(float3(-1,-1, 1))*fScale*(n+=st),
      normalize(float3(-1, 1,-1))*fScale*(n+=st),
      normalize(float3(-1, 1 ,1))*fScale*(n+=st),
      normalize(float3( 1,-1,-1))*fScale*(n+=st),
      normalize(float3( 1,-1, 1))*fScale*(n+=st),
      normalize(float3( 1, 1,-1))*fScale*(n+=st),
};
#define AO_TEST_QUALITY int(1)
#define AO_TEST_RADIUS float(0.05)
#define AO_TEST_BLEND_FACTOR float(1)
#define AO_FADE_DIST float(100)
float3 calc_test(float3 P, float3 N, float2 tc)
{
	N = normalize(N);
	int quality = AO_TEST_QUALITY + 1;
	float3 occ = float3(0,0,0);
	float c = 1;
	float scale = AO_TEST_RADIUS;//*(1 + P.z/AO_FADE_DIST);
/*	float3 rot = normalize(cross(float3(0,0,-1), N));	// векторное произведение между float(0,0,-1) и нормалью
	float cs = dot(float3(0,0,-1), N);								// скалярное произведение между float(0,0,-1) и нормалью
	float inv_cs = 1 - cs;
	float sn = sqrt(1 - cs*cs);
	float3x3 m_N = float3x3(      cs + inv_cs*rot.x*rot.x, inv_cs*rot.x*rot.y - sn*rot.z, inv_cs*rot.x*rot.z + sn*rot.y,
							inv_cs*rot.x*rot.y + sn*rot.z,       cs + inv_cs*rot.y*rot.y, inv_cs*rot.y*rot.z - sn*rot.x,
							inv_cs*rot.x*rot.z - sn*rot.y, inv_cs*rot.y*rot.z + sn*rot.x,       cs + inv_cs*rot.z*rot.z);*/
/*	float3x3 m_N = float3x3(cs + inv_cs*rot.x*rot.x,      inv_cs*rot.x*rot.y,  sn*rot.y,
							     inv_cs*rot.x*rot.y, cs + inv_cs*rot.y*rot.y, -sn*rot.x,
										  -sn*rot.y, 				sn*rot.x,        cs);	*/						
	for (int a=1; a<quality; ++a)
	{
		scale *= a;
		for (int i=0; i<8; i++)
		{
			// координата точки в view space. Точки лежат на плоскости
			//float3 s = mul(m_N, arr1[i]);
//			float3 s = arr1[i];
//			float3 occ_pos_view = P + s * scale;
			// проецируем точку на экран
//			float4 occ_pos_screen = proj_to_screen(mul(m_P, float4(occ_pos_view, 1.0)));
			// берем поверхностный семпл точки
			float3 screen_occ = tex2D(s_position, tc + arr1[i]* scale);
			// определяем разницу между истинной и поверхностной точкой
			float delta = (P.z - screen_occ.z)*1000;
			/*Возможные варианты:
			1) delta > 0. Основной случай, имеем в точке геометрию, затеняющую центр. 
			2) delta = 0. Точка на плоскости, нет затенения
			3) delta < 0. Точка в углублении относительно центра, нет затенения.
			Чтобы поддержать все эти случаи, будем руководствоваться не яркость, а затенением. 0 - светло, 1 - темно
			*/
			// отбрасываем случаи точки на плоскости или ухода вглубь (occ_coeff = 0)
			float occ_coeff = saturate(sign(delta));
			// уберем затенение от неба
			occ_coeff *= saturate(sign(screen_occ.z - 0.001));
			// учтем затенение точки, определяемое как косинус угла между нормалью и вектором PD.
//			occ_coeff *= saturate(dot(N, normalize(screen_occ - P)));
			// переходим к яркости
			occ_coeff = 1 - occ_coeff;
			occ += float3(occ_coeff, occ_coeff, occ_coeff);
		}
	}
	occ /= (8 * AO_TEST_QUALITY);
	occ = saturate(occ);
	return (occ + (1 - occ)*(1 - AO_TEST_BLEND_FACTOR));
}
#endif