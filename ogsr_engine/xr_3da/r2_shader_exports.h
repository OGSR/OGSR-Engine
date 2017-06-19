#pragma once

class ShExports
{
	Fvector2 artefacts_position[8];
	Fvector2 anomalys_position[8];
	Ivector2 detector_params;

public:
	ShExports()
	{
		for (int i = 0; i < 8; ++i)
		{
			artefacts_position[i].set(0.f, 0.f);
			anomalys_position[i].set(0.f, 0.f);
		}
		detector_params.set(0, 0);
	}

	void set_artefact_position			(u32 _i, Fvector2 _pos) { artefacts_position[_i].set(_pos); };
	void set_anomaly_position			(u32 _i, Fvector2 _pos) { anomalys_position[_i].set(_pos); };
	void set_detector_params			(Ivector2 _pos)			{ detector_params.set(_pos); };

	Fvector2 get_artefact_position		(u32 _i)				 {return artefacts_position[_i];}
	Fvector2 get_anomaly_position		(u32 _i)				{ return anomalys_position[_i]; }
	Ivector2 get_detector_params			()						 { return detector_params; }
};

ENGINE_API extern ShExports *shader_exports;