function element_0(shader, t_base, t_second, t_detail)		-- [0] generation of sunshafts mask
	shader:begin("stub_screen_space", "ssss_ogse_mask")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_position", "$user$position")
	shader:dx10texture	("s_image", "$user$generic0")
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] first pass
	shader:begin("stub_screen_space", "ssss_ogse_blur")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_sun_shafts", "$user$sun_shafts0")
end

function element_2(shader, t_base, t_second, t_detail)		-- [2] second pass
	shader:begin("stub_screen_space", "ssss_ogse_blur")
		:fog		(false)
		:zb			(false, false)
	shader:dx10texture	("s_sun_shafts", "$user$sun_shafts1")
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] third pass
	shader:begin("stub_screen_space", "ssss_ogse_blur")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_sun_shafts", "$user$sun_shafts0")
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] combine pass
	shader:begin("stub_screen_space", "ssss_ogse_final")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_position", "$user$position")
	shader:dx10texture	("s_image", "$user$generic0")
	shader:dx10texture	("s_sun_shafts", "$user$sun_shafts1")
	-- Jitter
	shader:dx10texture	("jitter0", "$user$jitter_0")
	shader:dx10texture	("jitter1", "$user$jitter_1")
	shader:dx10texture	("jitter_hq", "$user$jitter_hq")
	
	shader:dx10sampler	("smp_jitter")
end