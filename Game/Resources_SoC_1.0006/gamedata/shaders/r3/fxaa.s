function element_0(shader, t_base, t_second, t_detail)
	shader:begin("fxaa_main", "fxaa_main")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_base0", "$user$generic0")
	shader:dx10sampler("smp_rtlinear")
end
