function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "pp_lut")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_lut_atlas", "shaders\\lut_atlas")

	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_linear")
end
