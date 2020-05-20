function element_0(shader, t_base, t_second, t_detail)		-- [0]
	shader:begin("stub_screen_space", "ssss_mrmnwar_mask")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_position", "$user$position")
	shader:dx10texture	("s_image", "$user$generic0")
end

function element_1(shader, t_base, t_second, t_detail)		-- [1]
	shader:begin("stub_screen_space", "ssss_mrmnwar_mask_blur")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_mask", "$user$SunShaftsMask")
	
	shader:dx10sampler	("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)		-- [2]
	shader:begin("stub_screen_space", "ssss_mrmnwar_generation")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_mask_blur", "$user$SunShaftsMaskSmoothed")
end

function element_3(shader, t_base, t_second, t_detail)		-- [3]
	shader:begin("stub_screen_space", "ssss_mrmnwar_generation")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_mask_blur", "$user$SunShaftsPass0")
end

function element_4(shader, t_base, t_second, t_detail)		-- [4]
	shader:begin("stub_screen_space", "ssss_mrmnwar_display")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", "$user$generic0")
	shader:dx10texture	("s_sunshafts", "$user$SunShaftsMaskSmoothed")
end