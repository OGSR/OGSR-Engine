//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef SpawnPointH
#define SpawnPointH

#include "LevelGameDef.h"
#include "../../xr_3da/xrGame/xrServer_Objects_abstract.h"
#include "CustomObject.h"

// refs
class CSE_Visual;
class CSE_Motion;
class CObjectAnimator;
class ISE_Abstract;

class CSpawnPoint : public CCustomObject {
	typedef CCustomObject inherited;

    friend class    SceneBuilder;
public:                           
    class CLE_Visual{
    public:
        bool                    b_tmp_lock;
    	CSE_Visual*		source;
        IRender_Visual*	visual;
        void 			OnChangeVisual	();  
        void 			PlayAnimation	();
    public:
						CLE_Visual		(CSE_Visual* src);
        virtual			~CLE_Visual		();
    };
    class CLE_Motion{
    public:
    	CSE_Motion*		source;
        CObjectAnimator*animator;
        void 			OnChangeMotion	();
        void 			PlayMotion		();
    public:
						CLE_Motion		(CSE_Motion* src);
        virtual			~CLE_Motion		();
    };
	struct SSpawnData: public ISE_AbstractLEOwner{
		CLASS_ID		m_ClassID;
        shared_str 		m_Profile;
		ISE_Abstract*	m_Data;
        CLE_Visual*		m_Visual;
        CLE_Motion*		m_Motion;
        SSpawnData	()
        {
			m_ClassID	= 0;
			m_Data		= 0;
            m_Visual	= 0;
            m_Motion	= 0;
        }
        ~SSpawnData	()
        {
        	Destroy	();
        }
        void		Create	(LPCSTR entity_ref);
        void		Destroy	();
        bool		Valid	(){return m_Data;}

        void		Save	(IWriter&);
        bool		Load	(IReader&);
		bool 		ExportGame(SExportStreams* F, CSpawnPoint* owner);

		void		FillProp(LPCSTR pref, PropItemVec& values);

		void    	Render	(bool bSelected, const Fmatrix& parent,int priority, bool strictB2F);
		void    	OnFrame	();
		virtual void get_bone_xform	(LPCSTR name, Fmatrix& xform);
	};

	SSpawnData    	m_SpawnData;
	CCustomObject*	m_AttachedObject;

    EPointType		m_Type;
    union{
    	struct{
		    u8		m_RP_TeamID;
		    u8		m_RP_Type;
		    u8		m_RP_GameType;
            u8		reserved;
        };
        struct{
        	float	m_EM_Radius;
            float 	m_EM_Power;
            float	m_EM_ViewDist;
            u32		m_EM_FogColor;
            float	m_EM_FogDensity;
            u32		m_EM_AmbientColor;
            u32		m_EM_SkyColor;
            u32		m_EM_HemiColor;
        };
    };

    shared_str		SectionToEditor			(shared_str);
    shared_str		EditorToSection			(shared_str);
	void __stdcall	OnProfileChange			(PropValue* prop);
	void __stdcall	OnFillChooseItems		(ChooseValue*);
    bool 			OnAppendObject	(CCustomObject* object);
protected:
    virtual void 	SetPosition		(const Fvector& pos);
    virtual void 	SetRotation		(const Fvector& rot);
    virtual void 	SetScale		(const Fvector& scale);
public:
	                CSpawnPoint    	(LPVOID data, LPCSTR name);
    void            Construct   	(LPVOID data);
    virtual         ~CSpawnPoint   	();
    virtual bool	CanAttach		() {return true;}
    
	bool 			RefCompare		(LPCSTR ref);
    virtual LPCSTR	RefName			();

    bool			CreateSpawnData	(LPCSTR entity_ref);
	virtual void    Render      	( int priority, bool strictB2F );
	virtual bool    RayPick     	( float& distance,	const Fvector& start,	const Fvector& direction, SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick		( const CFrustum& frustum );
	virtual bool    GetBox      	(Fbox& box);

	virtual void 	OnFrame			();

	virtual void 	Select			(int  flag);

  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
    virtual bool	ExportGame		(SExportStreams* data);

	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);

    bool			AttachObject	(CCustomObject* obj);
    void			DetachObject	();
    
    virtual bool	OnChooseQuery	(LPCSTR specific);
};

#endif /*_INCDEF_Glow_H_*/

