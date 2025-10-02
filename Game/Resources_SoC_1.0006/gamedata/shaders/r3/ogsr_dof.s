function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_dof")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_blur_2", "$user$blur_2")
	shader:dx10texture("t_zbuffer", "$user$zbuffer_dof");

	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_dof_pp")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("samplero_pepero", "$user$dof")

	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end
