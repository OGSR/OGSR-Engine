#pragma once
class	CBlend;
class	motion_marks;
class	IKinematicsAnimated;
class	ik_anim_state
{

			bool			is_step;
public:
						ik_anim_state	(  ): is_step( false )	{};
			void		update			( IKinematicsAnimated *K, const	CBlend *b, u16 interval );
IC			bool		step			( ){ return is_step; }
};

