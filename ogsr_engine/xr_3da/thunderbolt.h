// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ThunderboltH
#define ThunderboltH
#pragma once

//refs
class ENGINE_API IRender_DetailModel;
class ENGINE_API CLAItem;

struct SThunderboltDesc
{
	// geom
	IRender_DetailModel*		l_model;
    // sound
    ref_sound					snd;
    // gradient
	struct SFlare
	{
    	float					fOpacity;
	    Fvector2				fRadius;
        shared_str				texture;
        shared_str				shader;
        ref_shader				hShader;
    	SFlare()				{ fOpacity = 0; fRadius.set(0.f,0.f);}
	};
    SFlare						m_GradientTop;
    SFlare						m_GradientCenter;
    shared_str					name;
	CLAItem*					color_anim;
public:
								SThunderboltDesc	(CInifile* pIni, LPCSTR sect);
							    ~SThunderboltDesc	();
};

struct SThunderboltCollection
{
	DEFINE_VECTOR(SThunderboltDesc*,DescVec,DescIt);
	DescVec			  			palette;
	shared_str					section;
public:
								SThunderboltCollection	(CInifile* pIni, LPCSTR sect);
								~SThunderboltCollection	();
	SThunderboltDesc*			GetRandomDesc			(){VERIFY(palette.size()>0); return palette[::Random.randI(palette.size())];}
};

#define THUNDERBOLT_CACHE_SIZE	8
//
class ENGINE_API CEffect_Thunderbolt
{
protected:
	DEFINE_VECTOR(SThunderboltCollection*,CollectionVec,CollectionVecIt);
	CollectionVec				collection;
	SThunderboltDesc*			current;
private:
    Fmatrix				  		current_xform;
	Fvector3					current_direction;

	ref_geom			  		hGeom_model;
    // states
	enum EState
	{
        stIdle,
		stWorking
	};
	EState						state;

	ref_geom			  		hGeom_gradient;

    Fvector						lightning_center;
    float						lightning_size;
    float						lightning_phase;

    float						life_time;
    float						current_time;
    float						next_lightning_time;
	BOOL						bEnabled;

    // params
    Fvector2					p_var_alt;
    float						p_var_long;
    float						p_min_dist;
    float						p_tilt;
    float						p_second_prop;
	float						p_sky_color;
	float						p_sun_color;
	float						p_fog_color;
private:
	BOOL						RayPick				(const Fvector& s, const Fvector& d, float& range);
    void						Bolt				(int id, float period, float life_time);
public:                     
								CEffect_Thunderbolt	(); 
								~CEffect_Thunderbolt();

	void						OnFrame				(int id,float period, float duration);
	void						Render				();

	int							AppendDef			(CInifile* pIni, LPCSTR sect);
};

#endif //ThunderboltH
