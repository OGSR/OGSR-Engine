function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_sslr")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_normal", "$user$normal")
	shader:dx10texture("s_env0", "$user$env_s0")
	shader:dx10texture("s_env1", "$user$env_s1")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_sslr_gauss")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$SSLR0")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_sslr_gauss")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$SSLR1")
end
