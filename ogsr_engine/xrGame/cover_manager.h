////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager.h
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "profiler.h"
#include "quadtree.h"

class CCoverPoint;

namespace LevelGraph {
	class	CVertex;
};

class CCoverManager {
public:
	typedef CQuadTree<CCoverPoint> CPointQuadTree;

protected:
	CPointQuadTree					*m_covers;
	xr_vector<bool>					m_temp;
	mutable xr_vector<CCoverPoint*>	m_nearest;


protected:
	IC		bool					edge_vertex			(u32 index);
	IC		bool					cover				(LevelGraph::CVertex *v, u32 index0, u32 index1);
	IC		bool					critical_point		(LevelGraph::CVertex *v, u32 index, u32 index0, u32 index1);
	IC		bool					critical_cover		(u32 index);

private:
	template <typename _evaluator_type, typename _restrictor_type>
	IC		bool					inertia				(float radius, _evaluator_type &evaluator, const _restrictor_type &restrictor) const;

public:
									CCoverManager		();
	virtual							~CCoverManager		();
			void					compute_static_cover();
	IC		CPointQuadTree			&covers				() const;
	IC		CPointQuadTree			*get_covers			();
			void					clear				();
	template <typename _evaluator_type, typename _restrictor_type>
	IC		const CCoverPoint		*best_cover			(const Fvector &position, float radius, _evaluator_type &evaluator, const _restrictor_type &restrictor) const;
	template <typename _evaluator_type>
	IC		const CCoverPoint		*best_cover			(const Fvector &position, float radius, _evaluator_type &evaluator) const;
	IC		bool					operator()			(const CCoverPoint *) const;
	IC		float					weight				(const CCoverPoint *) const;
	IC		void					finalize			(const CCoverPoint *) const;
};

#include "cover_manager_inline.h"