#pragma once

#include <wrl.h>

// must conform to following order of allocation/free
// a(A), a(B), a(C), a(D), ....
// f(A), f(B), f(C), f(D), ....
// a(A), a(B), a(C), a(D), ....
//	this mean:
//		use as litle of queries as possible
//		first try to use queries allocated first
//	assumption:
//		used queries number is much smaller than total count

class R_occlusion
{
private:
	struct	_Q	{
	  u32        order;
	  Microsoft::WRL::ComPtr<ID3DQuery> Q;
	  u32        ttl;
	};

	static constexpr u32 iInvalidHandle = 0xFFFFFFFF;

	bool enabled;
	std::vector<_Q>			pool;		// sorted (max ... min), insertions are usually at the end
	std::vector<_Q>			used;		// id's are generated from this and it is cleared from back only
	std::vector<u32>		fids;		// free id's
	u32 last_frame;

	void cleanup_lost();

public:
#if defined(USE_DX10) || defined(USE_DX11)
	typedef	u64		occq_result;
#else	//	USE_DX10
	typedef	u32		occq_result;
#endif	//	USE_DX10
public:
	R_occlusion		();
	~R_occlusion	();

	void			occq_destroy	(				);
	u32				occq_begin		(u32&	ID		);	// returns 'order'
	void			occq_end		(u32&	ID		);
	occq_result		occq_get		(u32&	ID		);
	void occq_free( u32 ID );
};
