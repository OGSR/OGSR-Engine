local t_numbers = "ogse\\effects\\detector_upgrade"

-- common rendering --
function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("deffer_model_flat","deffer_base_flat")
			: fog		(false)
			: blend		(false,blend.srcalpha,blend.one)
			: aref 		(false,2)
			: sorting	(1, false)
			: zb 		(true,true)
			: emissive 	(true)
	shader:sampler	("s_base")      :texture	(t_base)
end

-- shadow map rendering --
function l_point		(shader, t_base, t_second, t_detail)
	shader:begin	("shadow_direct_model","shadow_direct_base")
			: fog		(false)
			: blend		(false,blend.srcalpha,blend.one)
			: aref 		(false,2)
			: sorting	(1, false)
			: zb 		(true,true)
	shader:sampler	("s_base")      :texture	(t_base)
end

-- emissive rendering --
function l_special	(shader, t_base, t_second, t_detail)
	shader:begin	("deffer_model_flat",	"accum_detector_3")
			: zb 		(true,false)
			: fog		(false)
			: emissive 	(true)
	shader:sampler	("s_wave")      :texture	("ogse\\detector_wave") :wrap()
	shader:sampler	("s_numbers")   :texture	(t_numbers)
end
