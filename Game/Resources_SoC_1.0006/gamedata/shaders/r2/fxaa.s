function element_0(shader, t_base, t_second, t_detail)
	shader:begin("fxaa_main", "fxaa_main")
		:fog(false)
		:zb(false, false)
	shader:sampler("s_base0"):texture("$user$generic0")
end
