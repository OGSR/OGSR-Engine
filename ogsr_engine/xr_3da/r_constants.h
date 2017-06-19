#ifndef r_constantsH
#define r_constantsH
#pragma once

#include "../xrCore/xr_resource.h"

class ENGINE_API	R_constant_setup;

enum
{
	RC_float	= 0,
	RC_int		= 1,
	RC_bool		= 2,
	RC_sampler	= 99
};
enum
{
	RC_1x1		= 0,					// vector1, or scalar
	RC_1x4,								// vector4
	RC_2x4,								// 4x2 matrix, transpose
	RC_3x4,								// 4x3 matrix, transpose
	RC_4x4,								// 4x4 matrix, transpose
	RC_1x4a,							// array: vector4
	RC_3x4a,							// array: 4x3 matrix, transpose
	RC_4x4a								// array: 4x4 matrix, transpose
};
enum
{
	RC_dest_pixel	= (1<<0),
	RC_dest_vertex	= (1<<1),
	RC_dest_sampler	= (1<<2)
};

struct	R_constant_load
{
	u16						index;		// linear index (pixel)
	u16						cls;		// element class

	R_constant_load() : index(u16(-1)), cls(u16(-1)) {};

	IC BOOL					equal		(R_constant_load& C)
	{
		return (index==C.index) && (cls == C.cls);
	}
};

struct	R_constant			:public xr_resource
{
	shared_str				name;		// HLSL-name
	u16						type;		// float=0/integer=1/boolean=2
	u16						destination;// pixel/vertex/(or both)/sampler

	R_constant_load			ps;
	R_constant_load			vs;
	R_constant_load			samp;
	R_constant_setup*		handler;

	R_constant() : type(u16(-1)), destination(0), handler(NULL) { };

	IC BOOL					equal		(R_constant& C)
	{
		return (0==xr_strcmp(name,C.name)) && (type==C.type) && (destination==C.destination) && ps.equal(C.ps) && vs.equal(C.vs) && samp.equal(C.samp) && handler==C.handler;
	}
	IC BOOL					equal		(R_constant* C)
	{
		return equal(*C);
	}
};
typedef	resptr_core<R_constant,resptr_base<R_constant> > ref_constant;

// Automatic constant setup
class	ENGINE_API			R_constant_setup
{
public:
	virtual void			setup		(R_constant* C)	= 0;
};

class	ENGINE_API			R_constant_table	: public xr_resource_flagged	{
public:
	typedef xr_vector<ref_constant>		c_table;
	c_table					table;
private:
	void					fatal		(LPCSTR s);
public:
	~R_constant_table					();

	void					clear		();
	BOOL					parse		(void* desc, u16 destination);
	void					merge		(R_constant_table* C);
	ref_constant			get			(LPCSTR		name);		// slow search
	ref_constant			get			(shared_str&	name);		// fast search

	BOOL					equal		(R_constant_table& C);
	BOOL					equal		(R_constant_table* C)	{	return equal(*C);		}
	BOOL					empty		()						{	return 0==table.size();	}
};
typedef	resptr_core<R_constant_table,resptr_base<R_constant_table> >				ref_ctable;

#endif
