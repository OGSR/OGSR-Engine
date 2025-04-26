function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("stub_notransform_postpr","postprocess")
			: fog	(false)
			: zb 	(false,false)
	shader:dx10texture	("s_base0", "$user$generic_combine")
	shader:dx10texture	("s_base1", "$user$generic_combine")
	shader:dx10texture	("s_noise", "fx\\fx_noise2")

	shader:dx10sampler	("smp_rtlinear")
	shader:dx10sampler	("smp_linear")
end

function l_special        (shader, t_base, t_second, t_detail)
	shader:begin	("stub_notransform_postpr","postprocess_CM")
			: fog	(false)
			: zb 	(false,false)
	shader:dx10texture	("s_base0", "$user$generic_combine")
	shader:dx10texture	("s_base1", "$user$generic_combine")
	shader:dx10texture	("s_noise", "fx\\fx_noise2")

	shader:dx10sampler	("smp_rtlinear")
	shader:dx10sampler	("smp_linear")

	shader:dx10texture	("s_grad0", "$user$cmap0")
	shader:dx10texture	("s_grad1", "$user$cmap1")
end