local t_numbers = "ogse\\effects\\detector_upgrade"

-- common rendering --
function normal(shader, t_base, t_second, t_detail)
	shader:begin("deffer_model_flat", "models_detector_3_base")
		:fog(false)
		:emissive(true)
	shader:sampler("s_base"):texture(t_base)
end

-- emissive rendering --
function l_special(shader, t_base, t_second, t_detail)
	shader:begin("deffer_model_flat", "models_detector_3")
		:zb(true,false)
		:fog(false)
		:emissive(true)
	shader:sampler("s_numbers"):texture(t_numbers)
end
