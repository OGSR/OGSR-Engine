local function elemets(shader, t_base, t_second, t_detail, s_image)
	shader:begin("stub_screen_space", "ogsr_blur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", s_image)
	shader:dx10texture("s_position", "$user$position")
	//shader:dx10texture("s_lut_atlas", "shaders\\lut_atlas")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end

function element_0(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$generic0")
end

function element_1(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$blur_h_2")
end

function element_2(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$generic0")
end

function element_3(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$blur_h_4")
end

function element_4(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$generic0")
end

function element_5(shader, t_base, t_second, t_detail)
	elemets(shader, t_base, t_second, t_detail, "$user$blur_h_8")
end