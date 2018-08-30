#pragma once

#include	"fixedmap.h"

#define render_alloc xalloc
typedef xr_allocator render_allocator;

// #define	USE_RESOURCE_DEBUGGER

namespace	R_dsgraph
{
	// Elementary types
	struct _NormalItem	{
		float				ssa;
		IRender_Visual*		pVisual;
	};

	struct _MatrixItem	{
		float				ssa;
		IRenderable*		pObject;
		IRender_Visual*		pVisual;
		Fmatrix				Matrix;				// matrix (copy)
	};

	struct _MatrixItemS	: public _MatrixItem
	{
		ShaderElement*		se;
	};

	struct _LodItem		{
		float				ssa;
		IRender_Visual*		pVisual;
	};

#ifdef USE_RESOURCE_DEBUGGER
	typedef	ref_vs						vs_type;
	typedef	ref_ps						ps_type;
#else
	typedef	IDirect3DVertexShader9*		vs_type;
	typedef	IDirect3DPixelShader9*		ps_type;
#endif

	// NORMAL
	typedef xr_vector<_NormalItem,render_allocator::helper<_NormalItem>::result>			mapNormalDirect;
	struct	mapNormalItems		: public	mapNormalDirect										{	float	ssa;	};
	struct	mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalItems,render_allocator>				{	float	ssa;	};
	struct	mapNormalStates		: public	FixedMAP<IDirect3DStateBlock9*,mapNormalTextures,render_allocator>	{	float	ssa;	};
	struct	mapNormalCS			: public	FixedMAP<R_constant_table*,mapNormalStates,render_allocator>			{	float	ssa;	};
	struct	mapNormalPS			: public	FixedMAP<ps_type, mapNormalCS,render_allocator>						{	float	ssa;	};
	struct	mapNormalVS			: public	FixedMAP<vs_type, mapNormalPS,render_allocator>						{	};
	typedef mapNormalVS			mapNormal_T;

	// MATRIX
	typedef xr_vector<_MatrixItem,render_allocator::helper<_MatrixItem>::result>	mapMatrixDirect;
	struct	mapMatrixItems		: public	mapMatrixDirect										{	float	ssa;	};
	struct	mapMatrixTextures	: public	FixedMAP<STextureList*,mapMatrixItems,render_allocator>				{	float	ssa;	};
	struct	mapMatrixStates		: public	FixedMAP<IDirect3DStateBlock9*,mapMatrixTextures,render_allocator>	{	float	ssa;	};
	struct	mapMatrixCS			: public	FixedMAP<R_constant_table*,mapMatrixStates,render_allocator>			{	float	ssa;	};
	struct	mapMatrixPS			: public	FixedMAP<ps_type, mapMatrixCS,render_allocator>						{	float	ssa;	};
	struct	mapMatrixVS			: public	FixedMAP<vs_type, mapMatrixPS,render_allocator>						{	};
	typedef mapMatrixVS			mapMatrix_T;

	// Top level
	typedef FixedMAP<float,_MatrixItemS,render_allocator>			mapSorted_T;
	typedef mapSorted_T::TNode						mapSorted_Node;

	typedef FixedMAP<float,_MatrixItemS,render_allocator>			mapHUD_T;
	typedef mapHUD_T::TNode							mapHUD_Node;

	typedef FixedMAP<float,_LodItem,render_allocator>				mapLOD_T;
	typedef mapLOD_T::TNode							mapLOD_Node;
};
