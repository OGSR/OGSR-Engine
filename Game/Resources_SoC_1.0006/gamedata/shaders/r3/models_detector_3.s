local t_numbers = "ogse\\effects\\detector_upgrade"

-- common rendering --
function normal(shader, t_base, t_second, t_detail)
	shader:begin("deffer_model_flat", "models_detector_3_base")
		:fog(false)
		:emissive(true)
	shader:dx10texture("s_base", t_base)
	shader:dx10sampler("smp_base")
	shader:dx10stencil(true, cmp_func.always, 255 , 127, stencil_op.keep, stencil_op.replace, stencil_op.keep)
	shader:dx10stencil_ref(1)
	--shader:dx10color_write_enable( true, true, true, false) --KRodin: не раскомментировать ни в коем случае!!! Экран начнет просвечиваться. Интересный вопрос, зачем это используется в models_selflight_det.s
end

-- emissive rendering --
function l_special(shader, t_base, t_second, t_detail)
	shader:begin("deffer_model_flat", "models_detector_3")
		:zb(true, false)
		:fog(false)
		:emissive(true)
	shader:dx10texture("s_base", t_base)
	shader:dx10sampler("smp_base")
	shader:dx10texture("s_numbers", t_numbers)
	shader:dx10sampler("smp_linear")
	--shader:dx10color_write_enable(true, true, true, false) --в ES отключено
end
