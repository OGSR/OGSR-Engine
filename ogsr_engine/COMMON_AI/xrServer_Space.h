////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects space
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

#define SERVER_ENTITY_DECLARE_BEGIN0(__A)				class __A	{ public: DECLARE_SCRIPT_REGISTER_FUNCTION
#define SERVER_ENTITY_DECLARE_BEGIN(__A,__B)			class __A : public __B	{ typedef __B inherited; public: DECLARE_SCRIPT_REGISTER_FUNCTION
#define SERVER_ENTITY_DECLARE_BEGIN2(__A,__B,__C)		class __A : public __B, public __C	{ typedef __B inherited1; typedef __C inherited2; public: DECLARE_SCRIPT_REGISTER_FUNCTION
#define SERVER_ENTITY_DECLARE_BEGIN3(__A,__B,__C,__D)	class __A : public __B, public __C, public __D	{ typedef __B inherited1; typedef __C inherited2; typedef __D inherited3; public: DECLARE_SCRIPT_REGISTER_FUNCTION

#define	SERVER_ENTITY_DECLARE_END \
public:\
	virtual void 			UPDATE_Read		(NET_Packet& P); \
	virtual void 			UPDATE_Write	(NET_Packet& P); \
	virtual void 			STATE_Read		(NET_Packet& P, u16 size); \
	virtual void 			STATE_Write		(NET_Packet& P); \
};

struct	SRotation
{
	float  yaw, pitch, roll;
	SRotation() { yaw=pitch=roll=0; }
	SRotation(float y, float p, float r) { yaw=y;pitch=p;roll=r; }
};

enum EPOType {
	epotBox,
	epotFixedChain,
    epotFreeChain,
    epotSkeleton
};

DEFINE_VECTOR	(u32,						DWORD_VECTOR,					DWORD_IT);
DEFINE_VECTOR	(bool,						BOOL_VECTOR,					BOOL_IT);
DEFINE_VECTOR	(float,						FLOAT_VECTOR,					FLOAT_IT);
DEFINE_VECTOR	(LPSTR,						LPSTR_VECTOR,					LPSTR_IT);
DEFINE_VECTOR	(Fvector,					FVECTOR_VECTOR,					FVECTOR_IT);

#ifdef XRGAME_EXPORTS
#	define DECLARE_ENTITY_DESTROY
#endif

#ifdef DECLARE_ENTITY_DESTROY
template <class T> void	F_entity_Destroy	(T *&P)
{
	xr_delete	(P);
};
#endif
