//----------------------------------------------------
#ifndef MotionH
#define MotionH

#ifdef _LW_EXPORT
#include <lwrender.h>
#include <lwhost.h>
#endif

#include "bone.h"

// refs
class CEnvelope;
class IWriter;
class IReader;
class motion_marks;

enum EChannelType{
	ctUnsupported = -1,
	ctPositionX=0,
	ctPositionY,
	ctPositionZ,
	ctRotationH,
	ctRotationP,
	ctRotationB,
	ctMaxChannel
};

struct st_BoneMotion
{
	enum {
		flWorldOrient = 1<<0,
	};
	shared_str		name;
	CEnvelope*	envs			[ctMaxChannel];
	Flags8		m_Flags;
    			st_BoneMotion()	{name=0; m_Flags.zero(); ZeroMemory(envs,sizeof(CEnvelope*)*ctMaxChannel);}
    void        SetName(LPCSTR nm)	{	name=nm;	}
};
// vector по костям
DEFINE_VECTOR(st_BoneMotion,BoneMotionVec,BoneMotionIt);

//--------------------------------------------------------------------------
class ENGINE_API CCustomMotion
{
protected:
	enum EMotionType
	{
		mtObject	= 0,
		mtSkeleton,
		ForceDWORD	= u32(-1)
	};
	EMotionType		mtype{};
	int				iFrameStart, iFrameEnd;
	float			fFPS;
public:
	shared_str		name;
public:
					CCustomMotion	();
					CCustomMotion	(CCustomMotion* src);
	virtual			~CCustomMotion	();

	void			SetName			(const char* n)	{string256 tmp; tmp[0]=0; if(n){strcpy_s(tmp,n); strlwr(tmp);} name=tmp;}
	LPCSTR			Name			()				{return name.c_str();}
    int				FrameStart		()				{return iFrameStart;}
    int				FrameEnd		()				{return iFrameEnd;}
    float			FPS				()				{return fFPS;}
    int				Length			()				{return iFrameEnd-iFrameStart;}

	void			SetParam		(int s, int e, float fps){iFrameStart=s; iFrameEnd=e; fFPS=fps;}

	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf)=0;
	virtual bool	LoadMotion		(const char* buf)=0;

#ifdef _LW_EXPORT
	CEnvelope*		CreateEnvelope	(LWChannelID chan, LWChannelID* chan_parent=0);
#endif
};

//--------------------------------------------------------------------------
class ENGINE_API COMotion: public CCustomMotion
{
	CEnvelope*		envs			[ctMaxChannel];
public:
					COMotion		();
					COMotion		(COMotion* src);
	virtual			~COMotion		();

	void			Clear			();

	void			_Evaluate		(float t, Fvector& T, Fvector& R);
	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);

#ifdef _LW_EXPORT
	void			ParseObjectMotion(LWItemID object);
#endif
};

//--------------------------------------------------------------------------

enum ESMFlags
{
    esmFX = 1 << 0,
    esmStopAtEnd = 1 << 1,
    esmNoMix = 1 << 2,
    esmSyncPart = 1 << 3,
    esmUseFootSteps = 1 << 4,
    esmRootMover = 1 << 5,
    esmIdle = 1 << 6,
    esmUseWeaponBone = 1 << 7,
};

struct ECORE_API SAnimParams		{
    float			t;
    float			min_t;
    float			max_t;
    BOOL			bPlay;
	BOOL			bWrapped;
public:
					SAnimParams(){bWrapped=false;bPlay=false;t=0.f;min_t=0.f;max_t=0.f;}
    void			Set		(CCustomMotion* M);
	void 			Set		(float start_frame, float end_frame, float fps);
    float			Frame	()			{ return t;}
    void			Update	(float dt, float speed, bool loop);
    void			Play	(){bPlay=true; t=min_t;}
    void			Stop	(){bPlay=false; t=min_t;}
    void			Pause	(bool val){bPlay=!val;}
};

class ENGINE_API CClip{
public:
	struct AnimItem{
    	shared_str	name;
        u16			slot;
        			AnimItem	():slot(u16(-1)){}
        void		set			(shared_str nm, u16 s){name=nm;slot=s;}
        void		clear		(){set("",u16(-1));}
        bool		valid		(){return !!(name.size()&&(slot!=u16(-1)));}
        bool		equal		(const AnimItem& d) const {return name.equal(d.name)&&(slot==d.slot);}
    };
	shared_str		name;
    AnimItem		cycles[4];
    AnimItem		fx;
    
    float			fx_power;
    float			length;
public:
	virtual void	Save	(IWriter& F);
	virtual bool	Load	(IReader& F);
    bool			Equal	(CClip* c);
};
#endif
