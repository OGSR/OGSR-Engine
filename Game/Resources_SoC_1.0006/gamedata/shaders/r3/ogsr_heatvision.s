function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_heatvision")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_bloom_new", "$user$pp_bloom")

	shader:dx10texture("s_blur_2", "$user$blur_2")
	shader:dx10texture("s_blur_4", "$user$blur_4")
	shader:dx10texture("s_blur_8", "$user$blur_8")

	shader:dx10texture("s_heat", "$user$heat")

	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end