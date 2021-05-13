#pragma once

struct firedeps
{
	Fmatrix m_FireParticlesXForm; //направление для партиклов огня и дыма
	Fvector vLastFP{}; // fire point
	Fvector vLastFP2{}; // fire point2
	Fvector vLastFD{}; // fire direction
	Fvector vLastSP{}; // shell point

	firedeps() {
		m_FireParticlesXForm.identity();
	}
};
