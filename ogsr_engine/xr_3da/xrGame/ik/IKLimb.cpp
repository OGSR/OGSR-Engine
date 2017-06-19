#include "stdafx.h"
#include "IKLimb.h"
#include "../../SkeletonCustom.h"
#include "../ode_include.h"
#include "../MathUtils.h"
#include "../GameObject.h"
#include "../Level.h"
#include "../game_object_space.h"
#include "../ik_anim_state.h"

#ifdef DEBUG
#include "../PHDebug.h"
#endif
const Matrix	IKLocalJoint	={0,0,1,0,  -1,0,0,0, 0,-1,0,0, 0,0,0,1};//. in XGlobal
const Fmatrix	XLocalJoint		={0,-1,0,0, -1,0,0,0, 0,0,1,0,  0,0,0,1};

const Fmatrix	xm2im			={0,0,1,0,	0,1,0,0, 1,0,0,0, 0,0,0,1};

const Fvector	xgproj_axis		={0,1,0};
const Fvector	xgpos_axis		={0,0,1};

const Fvector	xlproj_axis		={1,0,0};
const Fvector	xlpos_axis		={0,0,1};
typedef float IVektor [3];

const IVektor	lproj_vector	={0,0,1};
const IVektor	lpos_vector		={-1,0,0};

const IVektor	gproj_vector	={0,0,1};//. in XGlobal
const IVektor	gpos_vector		={1,0,0};



CIKLimb::CIKLimb()
{
	Invalidate();
}

void CIKLimb::Invalidate()
{
//	m_prev_frame			=u32(-1)											;
//	m_prev_state_anim		=false												;
	m_id					=u16(-1)											;
	m_toe_position			.set(0,0,0)											;
	m_bones[0]				=BI_NONE											;	
	m_bones[1]				=BI_NONE											;	
	m_bones[2]				=BI_NONE											;	
	m_bones[3]				=BI_NONE											;	
	
}
void XM_IM(const Fmatrix	&XM,Fmatrix	&IM)
{
	IM.mul_43(xm2im,XM);
}
void XM_IM(const Fmatrix	&XM,Matrix	&IM)
{
	//((Fmatrix*)(&IM))->mul_43(xm2im,XM);
	XM_IM(XM,*((Fmatrix*)(&IM)));
}
void IM_XM(const Matrix	&IM,Fmatrix	&XM)
{
	XM.mul_43(xm2im,*((Fmatrix*)(&IM)));
}

void XM2IM(const Fmatrix	&XM,Fmatrix	&IM)
{
	//IM=xm2im*XM*xm2im^-1
	Fmatrix tmp;
	tmp.mul_43(xm2im,XM);
	IM.mul_43(tmp,xm2im);
}
void XM2IM(const Fmatrix &XM,Matrix &IM)
{
	XM2IM(XM,*((Fmatrix*)(&IM)));
}
void IM2XM(const Matrix &IM,Fmatrix &XM)
{
	XM2IM(*((Fmatrix*)(&IM)),XM);
}
void XV2IV(const Fvector	&XV,IVektor &IV)
{
	xm2im.transform_dir(cast_fv(IV),XV);
}

void IV2XV(const IVektor &IV,Fvector	&XV)
{
	xm2im.transform_dir(XV),cast_fv(IV);
}

void CIKLimb::Calculate(SCalculateData &cd)
{
	//m_prev_state_anim=true;
	ApplyContext(cd);
	if(cd.apply)
			Solve(cd);
}



float		CIKLimb::SwivelAngle( const Fmatrix &ihip, const SCalculateData& cd )
{
	Fvector foot; foot.set( cd.m_K->LL_GetTransform(m_bones[2]).c );// use "0" channal only?
	ihip.transform_tiny( foot );
	xm2im.transform_tiny( foot );
	
	Fvector knee; knee.set( cd.m_K->LL_GetTransform( m_bones[1] ).c );
	
	Fmatrix ih;
	CBoneData& BD=cd.m_K->LL_GetData(m_bones[0]);
	ih.mul_43(cd.m_K->LL_GetTransform(BD.GetParentID()),BD.bind_transform);
	ih.invert();

	ih.transform_tiny( knee );
	xm2im.transform_tiny( knee );

	return m_limb.KneeAngle( cast_fp( foot ), cast_fp( knee ) );

}

IC Fmatrix &get_base( Fmatrix &base, const Fvector &p0, const Fvector &p1 )
{
	base.c.set( 0, 0, 0 );
	base.i.sub( p1, p0 );
	base.i.normalize( );//safe?
	Fvector::generate_orthonormal_basis( base.i, base.j, base.k );
	return base;
}

void	CIKLimb::GetKnee				( Fvector &knee, const SCalculateData& cd ) const
{
	const Fvector hip		= cd.m_K->LL_GetTransform(m_bones[0]).c;
				  knee		= cd.m_K->LL_GetTransform(m_bones[1]).c;
	const Fvector foot		= cd.m_K->LL_GetTransform(m_bones[2]).c;
	Fvector p0; p0.sub( foot, hip ); Fvector p1; p1.sub( cd.goal.c, hip );
	float mp0 = p0.magnitude();
	if(fis_zero(mp0))
		return;
	p0.mul( 1.f/mp0 );
	knee.sub( hip );
	float dot = p0.dotproduct( knee );
	Fvector b1; b1.mul( p0, dot );
	Fvector b2; b2.sub(knee,b1);
	Fvector bb1;bb1.mul( p1, 1.f/mp0*dot );//mp1
	knee.add( bb1, b2 );
	knee.add( hip );
}

#ifdef DEBUG
bool	repeat = false;
#endif
void	CIKLimb::Solve(SCalculateData& cd)
{
		Matrix gl;
#ifdef DEBUG 
		if( m_limb.SetGoal( Goal( gl, cd.goal, cd ), ph_dbg_draw_mask.test( phDbgIKLimits ) ) )
#else
		if( m_limb.SetGoal( Goal( gl, cd.goal, cd ), FALSE ) )
#endif
		{
			float x[7];
			
			Fvector pos;
			GetKnee( pos, cd );
			
#ifdef	DEBUG
			if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
			{
				Fvector dbg_pos;
				cd.m_obj.transform_tiny(dbg_pos, pos);
				DBG_DrawPoint( dbg_pos, 0.02f, D3DCOLOR_XRGB( 255, 255 , 255 ) );
			}
#endif
			Fmatrix ihip;
			GetHipInvert( ihip, cd );
			ihip.transform_tiny( pos );
			xm2im.transform_tiny( pos );

			if( m_limb.SolveByPos( cast_fp( pos ), x ) )
			{
						cd.m_angles=x;
						CalculateBones( cd );
						//m_prev_state_anim=false;
			}
#ifdef DEBUG
			else
			{
				Msg( "ik not solved" );
				if( repeat )
				{
					sv_state = sv_state_DBR;
				}
			}
#endif
		}
#ifdef DEBUG
		else
		{
			Msg( "ik not solved" );
			if( repeat )
			{
				sv_state = sv_state_DBR;
			}
		}

		if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
		{
			Fvector dbg_pos;
			cd.m_K->LL_GetBoneInstance(m_bones[2]).mTransform.transform_tiny( dbg_pos, m_toe_position );
			cd.m_obj.transform_tiny( dbg_pos );
			DBG_DrawPoint( dbg_pos, 0.02f, D3DCOLOR_XRGB( 255, 255 , 0 ) );
		}
#endif
}

IC void set_limits( float &min, float &max, SJointLimit& l)
{
	min=-l.limit.y  ;max=-l.limit.x  ;
	min += M_PI; max += M_PI;
	clamp( min, 0.f, 2 * M_PI ); clamp( max, 0.f, 2 * M_PI );

	//min = 0/*-2*M_PI*/; max = 2*M_PI;
}

IC void free_limits( float &min, float &max)
{
	min = 0  ;max = 2 * M_PI  ;
}

void CIKLimb::Create( u16 id, CKinematics* K, const u16 bones[4], const Fvector& toe_pos, bool collide_ )
{
	m_id	 = id;
	m_collide= collide_;
	m_bones[0]=bones[0]; m_bones[1]=bones[1]; m_bones[2]=bones[2]; m_bones[3]=bones[3];
	m_toe_position.set(toe_pos);
//////////////////////////////////////////////////////////////////////
	xr_vector<Fmatrix> binds;
	K->LL_GetBindTransform( binds );
	Fmatrix XT,XS;
	XT.set( binds[bones[0]] ); XT.invert( ); XT.mulB_43( binds[bones[1]] );
	XS.set( binds[bones[1]] ); XS.invert( ); XS.mulB_43( binds[bones[2]] );
	Matrix T,S;XM2IM(XT,T);XM2IM(XS,S);
/////////////////////////////////////////////////////////////////////
	float lmin[7],lmax[7];
	SJointLimit* limits = K->LL_GetData( bones[0] ).IK_data.limits;
	set_limits( lmin[0], lmax[0], limits[0] );
	set_limits( lmin[1], lmax[1], limits[1] );
	set_limits( lmin[2], lmax[2], limits[1] );
	//free_limits( lmin[0], lmax[0] );
	//lmin[0] = M_PI * 3.f/4.f; 
	lmin[1]+=1.0f; lmax[1]-=0.f;
	lmin[2]+=1.0f; lmax[2]-=0.f;
	lmax[0] = 2* M_PI - M_PI * 2.f/3.f;

//  lmin[2]=-1.f;lmax[2]=1.f;

	limits=K->LL_GetData( bones[1] ).IK_data.limits;
	set_limits( lmin[3], lmax[3], limits[1] );
	free_limits( lmin[3], lmax[3] );

	limits=K->LL_GetData( bones[2] ).IK_data.limits;
	set_limits( lmin[4], lmax[4], limits[0] );
	set_limits( lmin[5], lmax[5], limits[1] );
	set_limits( lmin[6], lmax[6], limits[2] );
	//free_limits( lmin[4], lmax[4] );
	//free_limits( lmin[5], lmax[5] );
	//free_limits( lmin[6], lmax[6] );
	//lmin[6]=-1.f;lmax[6]=1.f;
	lmin[4]-=1.0f; lmax[4]+=1.f;
	lmin[5]-=1.0f; lmax[5]+=1.f;
	lmin[6]-=1.0f; lmax[6]+=1.f;
	//swap(lmin[4],lmax[4]);
	//swap(lmin[5],lmax[5]);
	//swap(lmin[6],lmax[6]);
	m_limb.init( T, S, ZXY, ZXY, gproj_vector, gpos_vector, lmin, lmax );
}

void CIKLimb::Destroy()
{

}
IC bool state_valide(const calculate_state &prev_state)
{
	return  (Device.dwFrame - prev_state.frame) == 1; //prev_state.state !=calculate_state::not_definite &&
}


IC void	CIKLimb::GetPickDir(Fvector &v, const Fmatrix &gl_bone )
{
	v.set(0, -1, 0 );
	if(!state_valide(sv_state))
	{
		sv_state.pick =v;
		VERIFY( _valid( v ) );
#ifdef	DEBUG
		if( ph_dbg_draw_mask.test( phDbgIK ) )
			Msg("prev state not valide");
#endif
		return;
	}
	const Fmatrix &anim_global = gl_bone; //anim_global.mul_43( cd.m_obj, cd.goal );
	Fmatrix sv_anim_global; sv_anim_global.mul_43( sv_state.obj_pos, sv_state.anim_pos );
	Fvector p0; Fvector p1;
	sv_anim_global.transform_tiny( p0, m_toe_position );
	anim_global.transform_tiny( p1, m_toe_position );
	Fvector dir; dir.sub( p1, p0 );
	if(dir.y > 0)
			dir.y = -dir.y;
	dir.mul( dir,0.01f/Device.fTimeDelta );
	dir.add(Fvector().set(0,-0.05f,0));
	dir.add(sv_state.pick);
	
	float m = dir.magnitude();
	if(m < EPS)
			return;
	dir.mul(dir,1/m);
	v.set( dir );
	VERIFY( _valid( v ) );
	sv_state.pick =v;
}

float CIKLimb::CollideFoot( float angle, const Fmatrix &gl_anim, Fplane &p, Fvector &ax )
{
	Fvector nc_toe; gl_anim.transform_tiny( nc_toe, m_toe_position );				//non collided toe
	float dfoot_plain	 =	m_toe_position.x;//xm.i.dotproduct( nc_toe ) - xm.i.dotproduct( xm.c );	//distanse from foot bone to foot plain
	float dfoot_tri		 =	-p.d - p.n.dotproduct( gl_anim.c );// dist from foot bone pos to tri plain
	VERIFY( dfoot_plain > 0.f );
	if( dfoot_tri > dfoot_plain * gl_anim.i.dotproduct( p.n ) ) // foot under tri 
	{
		Fvector axp;
		axp.sub( nc_toe, gl_anim.c );//normal from nc_toe to ax
		axp.sub( Fvector( ).mul( ax, axp.dotproduct( ax ) ) );
		//Fvector( ).mul( ax, ax.dotproduct( nc_toe ) - ax.dotproduct( xm.c ) );
		float dtoe_ax = axp.magnitude();
		axp.sub( Fvector( ).mul( gl_anim.i, axp.dotproduct( gl_anim.i ) ) );
		float dfoot = axp.magnitude( );
		if( dtoe_ax > EPS_L &&  dfoot_tri < dtoe_ax && dfoot > EPS_L && dfoot < dtoe_ax )
		{
			angle += asinf( dfoot_tri/dtoe_ax ) ;
			VERIFY( _valid( angle ) );
			angle -= acosf( dfoot/dtoe_ax );
			VERIFY( _valid( angle ) );
		}
	}
	clamp( angle, - M_PI/3, 0.f );
	return angle;
}

IC void tri_plane(const CDB::TRI &tri, Fplane &p )
{
	Fvector*	pVerts	= Level( ).ObjectSpace.GetStaticVerts( );
	p.n.mknormal	( pVerts[tri.verts[0]], pVerts[tri.verts[1]], pVerts[tri.verts[2]] );
	VERIFY( !fis_zero( p.n.magnitude( ) ) );
	p.n.invert( );
	p.d = -p.n.dotproduct( pVerts[tri.verts[0]] );
}

const float min_dot = 0.9f;// M_SQRT1_2;//M_SQRT1_2;
void CIKLimb::make_shift(Fmatrix &xm, const Fplane &p,const Fvector &pick_dir )
{
	Fvector shift = pick_dir;
	Fvector toe; xm.transform_tiny( toe, m_toe_position );

	float dot = p.n.dotproduct( shift );
	
	if( _abs( dot ) < min_dot )
	{
		shift.add( Fvector( ).mul( p.n, min_dot - _abs( dot ) ) );
		dot = p.n.dotproduct( shift );
		//shift.set( p.n );
		//dot = 1.f;
	}
	VERIFY( !fis_zero( dot ) );
	float shift_m = ( -p.d - p.n.dotproduct( toe ) )/dot;
	shift.mul( shift_m );
	xm.c.add( shift );
	
}

void CIKLimb::GetFootStepMatrix( Fmatrix	&m, const Fmatrix &gl_anim, const  SIKCollideData &cld, bool collide )
{
	if( !cld.collided ||  ( collide && cld.clamp_down ) )
	{
		m.set( gl_anim );
		return;
	}
	Fplane p = cld.m_plane;//; tri_plane( *cld.m_tri, p );
	Fmatrix xm; xm.set( gl_anim );
	Fvector ax; ax.crossproduct( p.n, xm.i );
	float s=ax.magnitude( );
	clamp( s, 0.f, 1.f );
	float angle = asinf( -s );
	VERIFY( _valid( angle ) );
	if( !fis_zero( s ) )
	{
		ax.mul( 1.f/s );
		if( collide )
			angle = CollideFoot( angle, gl_anim, p, ax );
		Fvector c = xm.c;
		xm.mulA_43( Fmatrix( ).rotation( ax,  angle ) );
		xm.c = c;
	}
	make_shift( xm, p, cld.m_pick_dir );
	VERIFY( _valid( xm ) );
	m.set( xm );
}


void CollideGoal( Fmatrix &g, const  SIKCollideData &cld )
{
	
	if( cld.collided && !cld.clamp_down )
	{
#ifdef DEBUG
	if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
	{
		DBG_DrawLine(cld.m_collide,cld.m_anime,D3DCOLOR_XRGB(0,0,255));
	}
#endif
		g.c.add( Fvector( ).sub( cld.m_collide,cld.m_anime ) );
	}
}

IC float clamp_rotation( Fquaternion &q, float v )
{
	float angl;Fvector ax;
	q.get_axis_angle( ax, angl );
	float abs_angl = _abs( angl );
	if( abs_angl > v )
	{
		if( angl <  0.f ) v = -v;
		q.rotation( ax, v );
		q.normalize( );
	}
	return abs_angl;
}

IC float  clamp_rotation( Fmatrix &m, float v )
{
	Fquaternion q;
	q.set(m);
	float r = clamp_rotation( q, v );
	Fvector c = m.c;
	m.rotation( q );
	m.c = c;
	return r;
}

IC void get_axix_angle( const Fmatrix &m, Fvector &ax, float &angl )
{
	Fquaternion q;
	q.set( m );
	q.get_axis_angle( ax, angl );
}

IC bool clamp_change( Fmatrix& m, const Fmatrix &start, float ml, float ma, float tl, float ta )
{
	Fmatrix diff; diff.mul_43( Fmatrix( ).invert( start ), m );
	float linear_ch	 = diff.c.magnitude( );
	bool ret = linear_ch < tl;
	if( linear_ch > ml )
		diff.c.mul( ml/linear_ch );
	if( clamp_rotation( diff, ma ) > ta )
		ret = false;
	if(!ret)
		m.mul_43( start, diff );
	return ret;
}

void get_diff_avalue( const Fmatrix & m0, const Fmatrix &m1, float &l, float &a )
{
	Fmatrix diff; diff.mul_43( Fmatrix( ).invert( m1 ), m0 );
	l = diff.c.magnitude( );
	Fvector ax; 
	get_axix_angle( diff, ax, a );
	a = _abs( a );
}
IC void get_blend_speed_limits(float& l,float& a, const SCalculateData& cd, const calculate_state	&sv_state )
{
	Fmatrix anim_global; anim_global.mul_43( cd.m_obj, cd.goal );
	Fmatrix sv_anim_global; sv_anim_global.mul_43( sv_state.obj_pos, sv_state.anim_pos );
	get_diff_avalue( sv_anim_global, anim_global, l, a );
	l*=1.5f;//a*=1.5;
}
void	CIKLimb::SetNewGoal			( const SIKCollideData &cld, SCalculateData& cd )
{
	if(!cd.do_collide)
						return;
	const Fmatrix &obj			= cd.m_obj;
	const Fmatrix iobj			= Fmatrix().invert( obj );
	const Fmatrix anim_local	= cd.goal;
	float l,a;
	get_blend_speed_limits( l, a, cd, sv_state );
	//CollideGoal( cd.goal, cld );
	GetFootStepMatrix( cd.goal, Fmatrix( ).mul_43( obj, cd.goal ), cld, true );
	cd.goal.mulA_43( iobj );

	const Fmatrix gl_goal	= Fmatrix( ).mul_43( obj, cd.goal );
	Fmatrix blend_to		=gl_goal;
	Fmatrix blend_from		=sv_state.goal;
	bool	blending		=state_valide( sv_state ) && (   sv_state.blending || sv_state.foot_step != cd.foot_step );
	if( !state_valide( sv_state ) )
	{
		if(cd.foot_step)
			GetFootStepMatrix( sv_state.collide_pos, gl_goal, cld, false );
	} 
	else if( cd.foot_step ) //the foot in animation on ground
	{
		if( !sv_state.foot_step )
		{	
			Fmatrix cl = gl_goal;
			if( sv_state.blending )
					cl = sv_state.goal;
			GetFootStepMatrix( sv_state.collide_pos, cl, cld, false ); // find where we can place the foot
			sv_state.speed_blend_l =	l/Device.fTimeDelta;
			sv_state.speed_blend_a =	a/Device.fTimeDelta;
		} 
		if( blending )
		{
			blend_to = sv_state.collide_pos;
			sv_state.speed_blend_l	+= 1.f * Device.fTimeDelta;
			sv_state.speed_blend_a	+= 5.f * Device.fTimeDelta;
			l = sv_state.speed_blend_l * Device.fTimeDelta;
			a = sv_state.speed_blend_a * Device.fTimeDelta;
		}  else
			cd.goal.mul_43( iobj, sv_state.goal );
	} 
	
#ifdef DEBUG
	if(ph_dbg_draw_mask.test( phDbgDrawIKGoal ))
	{
		if( cd.foot_step && state_valide( sv_state ) )
		{
			DBG_DrawMatrix( sv_state.collide_pos, 1.0f, 100 );
			DBG_DrawPoint ( sv_state.collide_pos.c, 0.05, D3DCOLOR_XRGB( 0, 255 , 255 ) );
		}
		if( cd.do_collide )
		{
			DBG_DrawPoint( cld.m_anime, 0.03f, D3DCOLOR_XRGB( 255, 255 , 255 ) );
			if( cld.collided )
				DBG_DrawPoint( cld.m_collide , 0.05f, D3DCOLOR_XRGB( 255, 0 , 0 ) );
		}
		if( blending )
		{
			if(cd.foot_step  != sv_state.foot_step)
					sv_state.count	= 50;
			int c = 55 + 200/50 * sv_state.count; 
			if( sv_state.count>0 )
				DBG_OpenCashedDraw( );
			Fvector a0;
			sv_state.goal.transform_tiny( a0, m_toe_position );
			Fvector a1;
			blend_from.transform_tiny( a1, m_toe_position );
			DBG_DrawLine( a0, a1, D3DCOLOR_XRGB( c, 0, c ) );
			Fvector a2;
			gl_goal.transform_tiny( a2, m_toe_position );
			DBG_DrawLine( a1, a2, D3DCOLOR_XRGB( 0, c , 0 ) );
			Fvector a3; sv_state_DBR.goal.transform_tiny( a3, m_toe_position );
			DBG_DrawLine( a3, a0, D3DCOLOR_XRGB( c, c , 0 ) );
			if( Fvector( ).sub( a3, a0 ).magnitude() > 0.1f )
					DBG_DrawLine( a3, a0, D3DCOLOR_XRGB( c, 0 , 0 ) );
			if( sv_state.count > -1 )
			{
				DBG_ClosedCashedDraw( 3000 );
				--sv_state.count;
			} 
		}
	}
	sv_state_DBR = sv_state;
#endif
	
	if( blending )
	{
		VERIFY( state_valide( sv_state ) );
		blending = !clamp_change( blend_to, blend_from, l, a, 0.0000001f, 0.00005f );//0.01f //0.005f
		cd.goal.mul_43( iobj, blend_to );
	}
	sv_state.foot_step = cd.foot_step;
	sv_state.anim_pos.set( anim_local );
	sv_state.goal.mul_43( obj, cd.goal );
	sv_state.obj_pos.set( obj );
	sv_state.frame = Device.dwFrame;
	sv_state.blending = blending;
}


void CIKLimb::ApplyContext( SCalculateData &cd )
{
	cd.do_collide = m_collide && cd.do_collide ;
	SetAnimGoal( cd );
	cd.foot_step = anim_state.step();//is_ground( cd );
	SIKCollideData cld;
	if( cd.do_collide )
			cld = collide_data;
	//Collide( cld, ( CGameObject* )cd.m_K->Update_Callback_Param, cd.goal, cd.foot_step );
	SetNewGoal(cld,cd);
}

void	CIKLimb::	AnimGoal			( Fmatrix &gl, CKinematicsAnimated	&K )
{
	K.Bone_GetAnimPos( gl, m_bones[2], 1<<0, false );
}

void	CIKLimb::SetAnimGoal			(SCalculateData& cd)
{
	AnimGoal( cd.goal, *cd.m_K );
	cd.apply = true;
}

void	CIKLimb::Update( CGameObject *O, const	CBlend *b, u16 interval )
{
	if(!m_collide)
				return;
	Fmatrix foot;
	CKinematicsAnimated *K = O->Visual( )->dcast_PKinematicsAnimated( );
	AnimGoal( foot,  *K );
	anim_state.update( K, b, interval );
	Collide( collide_data, O, foot, anim_state.step() );
}

const float		pick_dist		=1.f	;
void CIKLimb::Collide( SIKCollideData &cld, CGameObject *O, const Fmatrix &foot, bool foot_step )
{
	cld.collided = false;
	const Fmatrix &obj = O->XFORM( );
	Fmatrix gl_bone; gl_bone.mul_43( obj, foot );
	Fvector pos;
	gl_bone.transform_tiny( pos, m_toe_position );
	cld.m_anime.set(pos);
	GetPickDir( cld.m_pick_dir, gl_bone );
	const Fvector& pick_v = cld.m_pick_dir; 

	pos.sub( Fvector( ).mul( pick_v, pick_dist ) );
	float l_pick_dist = pick_dist;
	if( foot_step )
		l_pick_dist += 1.f;

	collide::rq_result	R;

	if( g_pGameLevel->ObjectSpace.RayPick( pos, pick_v, l_pick_dist, collide::rqtBoth, R, O ) )
	{
		if( !R.O )
		{
			cld.collided = true;
			CDB::TRI	* tri	= Level( ).ObjectSpace.GetStaticTris( ) + R.element;
			tri_plane( *tri, cld.m_plane );
			cld.m_collide.add( pos, Fvector( ).mul( pick_v, R.range ) );
			cld.clamp_down = R.range > pick_dist + EPS_L;
		} else {
			
			IRender_Visual* V =R.O->Visual();
			if( V )
			{
				CKinematics *K = V->dcast_PKinematics( );
				if( K )
				{
					float dist = l_pick_dist;
					if( K->PickBone(R.O->XFORM(), cld.m_plane.n, dist,  pos, pick_v,(u16) R.element))
					{
						cld.collided = true;
						Fvector point; point.add( pos, Fvector( ).mul( pick_v, dist ) );
						cld.m_plane.n.invert();
						cld.m_plane.d = -cld.m_plane.n.dotproduct( point );
					}
				}
			}	
			
		}
	}

#ifdef DEBUG
	if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) && cld.collided && !R.O )
	{
		CDB::TRI	*tri	= Level( ).ObjectSpace.GetStaticTris( ) + R.element;
		Fvector p = pos;p.add( Fvector( ).mul( pick_v, l_pick_dist ) );
		DBG_DrawLine(pos,p,D3DCOLOR_XRGB( 255, 0, 0 ) );
		if( tri )
		{
			Fvector p = pos;p.add( Fvector( ).mul( pick_v, l_pick_dist ) );
			DBG_DrawTri( tri,Level( ).ObjectSpace.GetStaticVerts( ), D3DCOLOR_XRGB( 255, 0, 0 ) );
		}
	}
#endif
}

Fmatrix&	CIKLimb::GetHipInvert( Fmatrix &ihip, const SCalculateData& cd  )
{
	CKinematics *K=cd.m_K;
	Fmatrix H;
	CBoneData& bd=K->LL_GetData(m_bones[0]);
	H.set(bd.bind_transform);
	H.mulA_43(K->LL_GetTransform(bd.GetParentID()));
	H.c.set(K->LL_GetTransform(m_bones[0]).c);
	ihip.invert( H );
	return ihip;
}

Matrix &CIKLimb::Goal			( Matrix &gl, const Fmatrix &xm, SCalculateData& cd )
{
#ifdef DEBUG
	const Fmatrix &obj=cd.m_obj;
	if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
	{
		Fmatrix DBGG;
		DBGG.mul_43( obj, xm );
		DBG_DrawMatrix( DBGG, 0.2f );
		if( cd.do_collide )
		{
			DBG_DrawLine( sv_state.goal.c, DBGG.c, D3DCOLOR_XRGB( 255, 0, 255 ) );
			DBG_DrawPoint( sv_state.goal.c, 0.05, D3DCOLOR_XRGB( 255, 255, 255 ) );
			DBG_DrawPoint( DBGG.c, 0.04,D3DCOLOR_XRGB(0, 255 ,0 ) );
			Fvector ch; ch.sub( DBGG.c, sv_state.goal.c );
			if( ch.magnitude( ) > 0.5f )
			{
				DBG_DrawMatrix( sv_state.goal, 3.5f );
			}
		}
		Fmatrix DBH; GetHipInvert( DBH, cd );
		DBH.invert( );
		DBGG.mul_43( obj, DBH );
		DBG_DrawMatrix( DBGG, 0.2f );
		
	}
#endif

	Fmatrix H;
	GetHipInvert( H, cd );
	Fmatrix G; 
	G.mul_43( H, xm );
	XM2IM( G, gl );
	return gl;
}

void CIKLimb::CalculateBones(SCalculateData &cd)
{
	VERIFY(cd.m_angles);
	CKinematics *K=cd.m_K;
	K->LL_GetBoneInstance(m_bones[0]).set_callback(bctCustom,BonesCallback0,&cd);
	K->LL_GetBoneInstance(m_bones[1]).set_callback(bctCustom,BonesCallback1,&cd);
	K->LL_GetBoneInstance(m_bones[2]).set_callback(bctCustom,BonesCallback2,&cd);
	K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite=TRUE;
	K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite=TRUE;
	K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite=TRUE;
	CBoneData &BD=K->LL_GetData(m_bones[0]);
	K->Bone_Calculate(&BD,&K->LL_GetTransform(BD.GetParentID()));

	K->LL_GetBoneInstance(m_bones[0]).set_callback(bctCustom,NULL,NULL);
	K->LL_GetBoneInstance(m_bones[1]).set_callback(bctCustom,NULL,NULL);
	K->LL_GetBoneInstance(m_bones[2]).set_callback(bctCustom,NULL,NULL);
	K->LL_GetBoneInstance(m_bones[0]).Callback_overwrite=FALSE;
	K->LL_GetBoneInstance(m_bones[1]).Callback_overwrite=FALSE;
	K->LL_GetBoneInstance(m_bones[2]).Callback_overwrite=FALSE;
}

void	DBG_DrawRotationLimitsY(const Fmatrix &start,float ang, float l, float h )
{
#ifdef DEBUG
	DBG_DrawRotationY( start, ang - EPS, ang + EPS, 0.15f, D3DCOLOR_XRGB( 0, 255, 0 ), false, 1 );
	DBG_DrawRotationY( start, l, h, 0.15f, D3DCOLOR_ARGB( 50, 0, 250, 0 ), true );
#endif // DEBUG
}

void	DBG_DrawRotationLimitsZ(const Fmatrix &start,float ang, float l, float h )
{
#ifdef DEBUG
	DBG_DrawRotationZ( start, ang - EPS, ang + EPS, 0.15f, D3DCOLOR_XRGB( 0, 0, 255 ), false, 1 );
	DBG_DrawRotationZ( start, l, h, 0.15f, D3DCOLOR_ARGB( 50, 0, 0, 250 ), true );
#endif // DEBUG
}

void	DBG_DrawRotationLimitsX(const Fmatrix &start,float ang, float l, float h )
{
#ifdef DEBUG
	DBG_DrawRotationX( start, ang + EPS, ang - EPS, 0.15f, D3DCOLOR_XRGB( 255, 0, 0 ), false, 1 );
	DBG_DrawRotationX( start, l, h, 0.15f, D3DCOLOR_ARGB( 50, 255, 0, 0 ), true );
#endif // DEBUG
}

void	DBG_DrawRotation3(const Fmatrix &start, const float angs[7], const AngleInt limits[7], u16 y, u16 z, u16 x)
{
	Fmatrix DBGG = start;
	DBG_DrawRotationLimitsY( DBGG, -angs[y], -limits[y].Low(), -limits[y].High() );
	DBGG.mulB_43(Fmatrix().rotateY(-angs[y]));
	DBG_DrawRotationLimitsZ( DBGG, -angs[z], -limits[z].Low(), -limits[z].High() );
	DBGG.mulB_43(Fmatrix().rotateZ(-angs[z]));
	DBG_DrawRotationLimitsX( DBGG, -angs[x], -limits[x].Low(), -limits[x].High() );
}
IC void ang_evaluate(Fmatrix& M, const float ang[3] )
{
	Fmatrix ry;ry.rotateY( -ang[0] );
	Fmatrix rz;rz.rotateZ( -ang[1] );
	Fmatrix rx;rx.rotateX( -ang[2] );
	M.mul_43(Fmatrix().mul_43( ry, rz ), rx);
}

IC void CIKLimb:: get_start( Fmatrix &start, SCalculateData &D, u16 bone )
{
	CKinematics		*K	=D.m_K;
	VERIFY( K );
	CIKLimb&		L	=D.m_limb;
	CBoneData		&BD	=K->LL_GetData( L.m_bones[bone] );
	start.mul_43( K->LL_GetTransform( BD.GetParentID( ) ), BD.bind_transform );
}

void 	CIKLimb::BonesCallback0				(CBoneInstance* B)
{
	SCalculateData* D	=(SCalculateData*)B->Callback_Param;
	VERIFY( D );

	float	const	*x	=D->m_angles;
	Fmatrix			bm;
	ang_evaluate	( bm, x );
	Fmatrix start	; 
	get_start		( start, *D, 0 );

	B->mTransform.mul_43( start, bm );

#ifdef DEBUG
	CIKLimb&	L	=D->m_limb;
	if( ph_dbg_draw_mask1.test( phDbgDrawIKLimits ) )
		DBG_DrawRotation3( Fmatrix( ).mul_43( D->m_obj, start ), x, L.m_limb.jt_limits, 0, 1, 2 );
	if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
	{
		DBG_DrawMatrix( Fmatrix( ).mul_43( D->m_obj, start ), 1.f );
		DBG_DrawMatrix( Fmatrix( ).mul_43( D->m_obj, Fmatrix( ).mul_43( start, bm ) ), 0.75f );
	}
#endif
	
}
void 	CIKLimb::BonesCallback1				(CBoneInstance* B)
{
	SCalculateData	*D	=(SCalculateData*)B->Callback_Param;

	float	const	*x	=D->m_angles;
	Fmatrix 		bm;
	bm.rotateY		( x[3] );

	Fmatrix start	; 
	get_start		( start, *D, 1 );
	B->mTransform.mul_43( start, bm );
}
void 	CIKLimb::BonesCallback2				(CBoneInstance* B)
{
	SCalculateData	*D		=(SCalculateData*)B->Callback_Param;
	
	float	const	*x		=D->m_angles;
	Fmatrix 		bm;
	ang_evaluate	( bm, x+4 );

	Fmatrix start	; 
	get_start		( start, *D, 2 );
	B->mTransform.mul_43( start, bm );

#ifdef DEBUG
	CIKLimb&		L	=D->m_limb;
	if(ph_dbg_draw_mask1.test(phDbgDrawIKLimits))
	{
		DBG_DrawRotation3( Fmatrix().mul_43( D->m_obj, start ), x, L.m_limb.jt_limits, 4, 5, 6 );
	}
	if( ph_dbg_draw_mask.test( phDbgDrawIKGoal ) )
	{
		DBG_DrawMatrix( Fmatrix( ).mul_43( D->m_obj,Fmatrix().mul_43( start, bm ) ), 0.3f );
		DBG_DrawMatrix( Fmatrix( ).mul_43( D->m_obj, start ), 0.3f );
	}
#endif
}