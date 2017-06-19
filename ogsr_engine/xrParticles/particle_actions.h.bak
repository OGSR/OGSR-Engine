//---------------------------------------------------------------------------
#ifndef particle_actionsH
#define particle_actionsH


namespace PAPI{
// refs
	struct ParticleEffect;
	struct PARTICLES_API			ParticleAction
	{
		enum{
			ALLOW_ROTATE	= (1<<1)
		};
		Flags32			m_Flags;
		PActionEnum		type;	// Type field
		ParticleAction	(){m_Flags.zero();}
        
		virtual void 	Execute		(ParticleEffect *pe, float dt)	= 0;
		virtual void 	Transform	(const Fmatrix& m)				= 0;

		virtual void 	Load		(IReader& F)=0;
		virtual void 	Save		(IWriter& F)=0;
	};
    DEFINE_VECTOR(ParticleAction*,PAVec,PAVecIt);
	class ParticleActions{
		PAVec			actions;
	public:
						ParticleActions()						{actions.reserve(4);	}
						~ParticleActions()						{clear();				}
		IC void			clear			()
        {
			for (PAVecIt it=actions.begin(); it!=actions.end(); it++) 
				xr_delete(*it);
			actions.clear();
		}
		IC void			append			(ParticleAction* pa)	{actions.push_back(pa);	}
		IC bool			empty			()						{return	actions.empty();}
		IC PAVecIt		begin			()						{return	actions.begin();}
		IC PAVecIt		end				()						{return actions.end();	}
        IC int			size			()						{return actions.size();	}
        IC void			resize			(int cnt)        		{actions.resize(cnt);	}
        void			copy			(ParticleActions* src);
	};
};
//---------------------------------------------------------------------------
#endif
