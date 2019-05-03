local t_numbers = "ogse\\effects\\detector_upgrade"
local t_wave = "ogse\\detector_wave"

-- common rendering --
function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("deffer_model_flat","deffer_base_flat")
			: fog		(false)
			: blend		(false,blend.srcalpha,blend.one)
			: aref 		(false,2)
			: sorting	(1, false)
			: zb 		(true,true)
			: emissive 	(true)
	shader:dx10texture	("s_base", t_base)
	shader:dx10sampler	("smp_base")
	shader:dx10stencil	( 	true, cmp_func.always, 
							255 , 127, 
							stencil_op.keep, stencil_op.replace, stencil_op.keep)
	shader:dx10stencil_ref	(1)
end

-- shadow map rendering --
function l_point		(shader, t_base, t_second, t_detail)
	shader:begin	("shadow_direct_model","shadow_direct_base")
			: fog		(false)
			: blend		(false,blend.srcalpha,blend.one)
			: aref 		(false,2)
			: sorting	(1, false)
			: zb 		(true,true)
	shader:dx10texture	("s_base", t_base)
	shader:dx10sampler	("smp_base")
	shader:dx10stencil	( 	true, cmp_func.always, 
							255 , 127, 
							stencil_op.keep, stencil_op.replace, stencil_op.keep)
	shader:dx10stencil_ref	(1)
end

-- emissive rendering --
function l_special	(shader, t_base, t_second, t_detail)
	shader:begin	("deffer_model_flat",	"models_detector_3")
			: zb 		(true,false)
			: fog		(false)
			: emissive 	(true)
	shader:dx10texture ("s_wave", t_wave)
	shader:dx10texture ("s_numbers", t_numbers)
	shader:dx10sampler ("smp_linear")
	shader:dx10color_write_enable( true, true, true, false)
end
