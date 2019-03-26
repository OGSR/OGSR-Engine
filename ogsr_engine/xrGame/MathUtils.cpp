#include "stdafx.h"
#include "Extendedgeom.h"
const float phInfinity = std::numeric_limits<float>::max();

bool ContactShotMarkGetEffectPars(dContactGeom* c, dxGeomUserData* &data, float &vel_cret, bool &b_invert_normal)
{
	dBodyID b = dGeomGetBody(c->g1);

	b_invert_normal = false;
	if (!b)
	{
		b = dGeomGetBody(c->g2);
		data = dGeomGetUserData(c->g2);
		b_invert_normal = true;
	}
	else
	{
		data = dGeomGetUserData(c->g1);
	}
	if (!b)
		return false;

	dVector3 vel;
	dMass m;
	dBodyGetMass(b, &m);
	dBodyGetPointVel(b, c->pos[0], c->pos[1], c->pos[2], vel);
	vel_cret = _abs(dDOT(vel, c->normal))* _sqrt(m.mass);
	return true;
}

inline bool RAYvsCYLINDER(const Fcylinder& c_cylinder, const Fvector &S, const Fvector &D, float &R, BOOL bCull)
{
	const float &r = c_cylinder.m_radius;
	float h = c_cylinder.m_height / 2.f;
	const Fvector& p = S;
	const Fvector& dir = D;

	const Fvector &c = c_cylinder.m_center;
	const Fvector &ax = c_cylinder.m_direction;

	//////////////////////////////////////////////////////////////
	Fvector	v;	v.sub(c, p);
	float	cs = dir.dotproduct(ax);
	float	Lc = v.dotproduct(ax);
	float	Lr = v.dotproduct(dir);
	////////////////////////////////////////////////
	float sq_cos = cs * cs;
	float sq_sin = 1 - sq_cos;
	float v_smag = v.square_magnitude();
	const float sq_r = r * r;

	if (sq_sin < EPS)//paralel
	{
		float tr1, tr2;
		float sq_dist = v_smag - Lr * Lr;//

		if (sq_dist > sq_r)
			return false;

		float r_dist = _sqrt(sq_r - sq_dist) + h;
		tr1 = Lr - r_dist;

		if (tr1 > R)
			return false;//

		if (tr1 < 0.f)
		{
			if (bCull)
				return false;
			else
			{
				tr2 = Lr + r_dist;
				if (tr2 < 0.f)
					return false;//

				if (tr2 < R)
				{
					R = tr2;
					return true;
				}

				return false;
			}
		}
		R = tr1;
		return true;
	}

	if (sq_cos < EPS)
	{
		float tr1, tr2;
		//perp//
		float abs_c_dist = _abs(Lc);

		if (abs_c_dist > h + r)
			return false;

		float sq_dist = v_smag - Lr * Lr - Lc * Lc;

		if (sq_dist > sq_r)
			return false;

		float lc_h = abs_c_dist - h;
		if (lc_h > 0.f)
		{
			float sq_sphere_dist = lc_h * lc_h + sq_dist * sq_dist;

			if (sq_sphere_dist > sq_r)
				return false;

			float diff = _sqrt(sq_r - sq_sphere_dist);
			tr1 = Lr - diff;

			if (tr1 > R)
				return false;//

			if (tr1 < 0.f)
			{
				if (bCull)
					return false;
				else
				{
					tr2 = Lr + diff;

					if (tr2 < 0.f)
						return false;//

					if (tr2 < R)
					{
						R = tr2;
						return true;
					}

					return false;
				}
			}
		}
		float diff = _sqrt(sq_r - sq_dist);
		tr1 = Lr - diff;

		if (tr1 > R)
			return false;//

		if (tr1 < 0.f)
		{
			if (bCull)
				return false;
			else
			{
				tr2 = Lr + diff;

				if (tr2 < 0.f)
					return false;//

				if (tr2 < R)
				{
					R = tr2;
					return true;
				}

				return false;
			}
		}
		R = tr1;
		return true;
	}
	//////////////////////////////////////////////////
	float tr1, tr2;
	float r_sq_sin = 1.f / sq_sin;
	float tr = (Lr - cs * Lc)*r_sq_sin;
	float tc = (cs*Lr - Lc)*r_sq_sin;

	//more frequent separation - axes dist> radius
	//v^2+tc^2+tr^2-2*(cos*tc*tr-Lc*tc+Lr*tr)

	float sq_nearest_dist = v_smag + tr * tr + tc * tc - 2 * (cs*tc*tr - Lc * tc + Lr * tr);

	if (sq_nearest_dist > sq_r)
		return false;
	//float max_c_diff=//;

	float sq_horde = (sq_r - sq_nearest_dist);

	float sq_c_diff = sq_horde * sq_cos*r_sq_sin;
	float c_diff = _sqrt(sq_c_diff);//ccc
	float cp1 = tc - c_diff;
	float cp2 = tc + c_diff;

	//cp1<cp2
	if (cp1 > h)
	{
		//sphere
		float tc_h = tc - h;//!! hi					(=)/;
		float sq_sphere_dist = sq_sin * tc_h*tc_h;

		if (sq_sphere_dist > sq_horde)
			return false;

		float tr_c = tr - tc_h * cs;//
		float diff = _sqrt(sq_horde - sq_sphere_dist);
		tr1 = tr_c - diff;

		if (tr1 > R)
			return false;//

		if (tr1 < 0.f)
		{
			if (bCull)
				return false;
			else
			{
				tr2 = tr_c + diff;

				if (tr2 < 0.f)
					return false;//

				if (tr2 < R)
				{
					R = tr2;
					return true;
				}
			}
		}
		R = tr1;
		return true;
	}

	if (cp2 < -h)
	{
		//sphere lo								/(=)
		float tc_h = tc + h;//!!
		float sq_sphere_dist = sq_sin * tc_h*tc_h;

		if (sq_sphere_dist > sq_horde)
			return false;

		float tr_c = tr - tc_h * cs;//!!
		float diff = _sqrt(sq_horde - sq_sphere_dist);
		tr1 = tr_c - diff;

		if (tr1 > R)
			return false;//

		if (tr1 < 0.f)
		{
			if (bCull)
				return false;
			else
			{
				tr2 = tr_c + diff;

				if (tr2 < 0.f)
					return false;//

				if (tr2 < R)
				{
					R = tr2;
					return true;
				}
			}
		}
		R = tr1;
		return true;
	}
	////////////////////////////////////////////////////////////////
	if (cs > 0.f)
	{
		if (cp1 > -h)
		{
			if (cp2 < h)
			{
				//cylinder							(=/=)
				float diff = c_diff / cs;
				tr1 = tr - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						tr2 = tr + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else {
				//mixed//cyl hi sphere					(=/)
				float diff = c_diff / cs;
				tr1 = tr - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						float tc_h = tc - h;
						float sq_sphere_dist = sq_sin * tc_h*tc_h;
						//if(sq_sphere_dist>sq_horde)return false	;
						float tr_c = tr - tc_h * cs;
						float diff = _sqrt(sq_horde - sq_sphere_dist);
						tr2 = tr_c + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
		else//cp1<=-h
		{
			if (cp2 < h)
			{
				//mixed//lo sphere	cyl						(/=)

				float tc_h = tc + h;//(tc-(-h))
				float sq_sphere_dist = sq_sin * tc_h*tc_h;
				//if(sq_sphere_dist>sq_horde)return false;
				float diff = _sqrt(sq_horde - sq_sphere_dist);
				float tr_c = tr - tc_h * cs;
				tr1 = tr_c - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						float diff = c_diff / cs;
						tr2 = tr + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else
			{
				//-(--)-								//sphere lo&&hi

				/////////////////////////////////////////////
				float tc_h = tc + h;
				float tr_c = tr - tc_h * cs;
				float diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
				tr1 = tr_c - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						float tc_h = tc - h;
						float tr_c = tr - tc_h * cs;
						float diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
						tr2 = tr_c + diff;
						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
	}
	else
	{
		if (cp1 > -h)
		{
			if (cp2 < h)
			{
				//cylinder
				float diff = -c_diff / cs;
				tr1 = tr - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						tr2 = tr + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else {//cp1>-h&&cp2>h
				float tc_h = tc - h;			//hi sphere/cyl
				float tr_c = tr - tc_h * cs;
				float diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
				tr1 = tr_c - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						diff = -c_diff / cs;
						tr2 = tr + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
		else//cp1<-h
		{
			if (cp2 < h)
			{
				//cyl/lo sphere
				float diff = -c_diff / cs;
				tr1 = tr - diff;

				if (tr1 > R)
					return false;//

				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						//mixed//lo
						float tc_h = tc + h;
						float tr_c = tr - tc_h * cs;
						diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
						tr2 = tr_c + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
			else//cp2>=h
			{
				//-(--)-								//sphere hi&&lo

				float tc_h = tc - h;
				float tr_c = tr - tc_h * cs;
				float diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
				tr1 = tr_c - diff;
				if (tr1 > R)
					return false;//
				/////////////////////////////////////////////
				if (tr1 < 0.f)
				{
					if (bCull)
						return false;
					else
					{
						tc_h = tc + h;
						tr_c = tr - tc_h * cs;
						diff = _sqrt(sq_horde - sq_sin * tc_h*tc_h);
						tr2 = tr_c + diff;

						if (tr2 < 0.f)
							return false;//

						if (tr2 < R)
						{
							R = tr2;
							return true;
						}
					}
				}
				R = tr1;
				return true;
			}
		}
	}
}

void capped_cylinder_ray_collision_test()
{
	Fcylinder c;
	c.m_center.set(0, 0, 0);
	c.m_direction.set(0, 0, 1);
	c.m_height = 2;
	c.m_radius = 1;
	//ray
	Fvector dir, pos; float R;
	dir.set(1, 0, 0); pos.set(0, 0, 0); R = 3;

	//inside
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 1
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false ,
	dir.set(0, 0, 1);
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 2
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false

	//outside
	pos.set(-3, 0, 0); dir.set(1, 0, 0); R = 4;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 2
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true , 2
	R = 1;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//false
	pos.set(0, 0, -3); dir.set(0, 0, 1); R = 4;
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true , 1
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true, 1

	pos.set(-3, -3, -3); dir.set(1, 1, 1); dir.normalize(); R = 10;
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//true, ?
	float ir[2];
	c.intersect(pos, dir, ir);
	//
	pos.set(0, 0, 0);
	RAYvsCYLINDER(c, pos, dir, R, FALSE);//true, ?
	c.intersect(pos, dir, ir);
	RAYvsCYLINDER(c, pos, dir, R, TRUE);//false
	CTimer t; t.Start();
	for (int i = 0; i < 1000000; i++)
	{
		Fcylinder c;
		c.m_center.random_point(Fvector().set(2, 2, 2));
		c.m_direction.random_dir();
		c.m_height = Random.randF(0.2f, 2.f);
		c.m_radius = Random.randF(0.1f, 2.f);
		//ray
		Fvector dir, pos; float R = Random.randF(0.1f, 2.f);
		dir.random_dir(); pos.random_point(Fvector().set(2, 2, 2));
		RAYvsCYLINDER(c, pos, dir, R, TRUE);
	}
	Msg("my RAYvsCYLINDE time %f ms", t.GetElapsed_sec()*1000.f);
	t.Start();
	for (int i = 0; i < 1000000; i++)
	{
		Fcylinder c;
		c.m_center.random_point(Fvector().set(2, 2, 2));
		c.m_direction.random_dir();
		c.m_height = Random.randF(0.2f, 2.f);
		c.m_radius = Random.randF(0.1f, 2.f);
		//ray
		Fvector dir, pos;//float R=Random.randF(0.1f,2.f);
		dir.random_dir(); pos.random_point(Fvector().set(2, 2, 2));
		c.intersect(pos, dir, ir);
	}
	Msg("current intersect time %f ms", t.GetElapsed_sec()*1000.f);
}
