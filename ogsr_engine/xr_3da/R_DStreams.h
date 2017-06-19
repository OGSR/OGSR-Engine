#ifndef r_DStreamsH
#define r_DStreamsH
#pragma once

enum
{
	LOCKFLAGS_FLUSH		= D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND	= D3DLOCK_NOOVERWRITE
};

class ENGINE_API _VertexStream
{
private :
	IDirect3DVertexBuffer9*		pVB;
	u32							mSize;			// size in bytes
	u32							mPosition;		// position in bytes
	u32							mDiscardID;		// ID of discard - usually for caching
public:
	IDirect3DVertexBuffer9*		old_pVB;
#ifdef DEBUG
	u32							dbg_lock;
#endif
private:
	void						_clear			()
	{
		pVB			= NULL;
		mSize		= 0;
		mPosition	= 0;
		mDiscardID	= 0;
#ifdef DEBUG
		dbg_lock	= 0;
#endif
	}
public:
	void						Create			();
	void						Destroy			();
	void						reset_begin		();
	void						reset_end		();

	IC IDirect3DVertexBuffer9*	Buffer()		{ return pVB;			}
	IC u32						DiscardID()		{ return mDiscardID;	}
	IC void						Flush()			{ mPosition=mSize;		}

	void*						Lock			( u32 vl_Count, u32 Stride, u32& vOffset );
	void						Unlock			( u32 Count, u32 Stride);

	_VertexStream()				{ _clear();		};
	~_VertexStream()			{ Destroy();	};
};

class ENGINE_API _IndexStream
{
private :
	IDirect3DIndexBuffer9*		pIB;
	u32							mSize;		// real size (usually mCount, aligned on 512b boundary)
	u32							mPosition;
	u32							mDiscardID;
public:
	IDirect3DIndexBuffer9*		old_pIB;
private:
	void						_clear	()
	{
		pIB			= NULL;
		mSize		= 0;
		mPosition	= 0;
		mDiscardID	= 0;
	}
public:
	void						Create			();
	void						Destroy			();
	void						reset_begin		();
	void						reset_end		();

	IC IDirect3DIndexBuffer9*	Buffer()		{ return pIB;			}
	IC u32						DiscardID()		{ return mDiscardID;	}
	void						Flush()			{ mPosition=mSize;		}

	u16*						Lock			( u32 Count, u32& vOffset );
	void						Unlock			(u32 RealCount);

	_IndexStream()				{ _clear();		};
	~_IndexStream()				{ Destroy();	};
};
#endif
