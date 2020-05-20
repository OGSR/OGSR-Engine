function element_0(shader, t_base, t_second, t_detail)		-- [0] generation of sunshafts mask
	shader:begin("null", "ssss_ogse_mask")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_position") 	:texture("$user$position")		:clamp() :f_none()
	shader:sampler	("s_image") 	:texture("$user$generic0")		:clamp() :f_none()
end

function element_1(shader, t_base, t_second, t_detail)		-- [1] first pass
	shader:begin("null", "ssss_ogse_blur")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_sun_shafts"):texture("$user$sun_shafts0")	:clamp() :f_none()
end

function element_2(shader, t_base, t_second, t_detail)		-- [2] second pass
	shader:begin("null", "ssss_ogse_blur")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_sun_shafts"):texture("$user$sun_shafts1")	:clamp() :f_none()
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] third pass
	shader:begin("null", "ssss_ogse_blur")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_sun_shafts"):texture("$user$sun_shafts0")	:clamp() :f_none()
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] combine pass
	shader:begin("null", "ssss_ogse_final")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_position") 	:texture("$user$position")		:clamp() :f_none()
	shader:sampler	("s_image") 	:texture("$user$generic0")		:clamp() :f_none()
	shader:sampler	("s_sun_shafts"):texture("$user$sun_shafts1")	:clamp() :f_none()
	-- Jitter
	shader:sampler	("jitter0")		:texture("$user$jitter_0")		:wrap()  :f_none()
	shader:sampler	("jitter1")		:texture("$user$jitter_1")		:wrap()  :f_none()
	shader:sampler	("jitter_hq")	:texture("$user$jitter_hq")		:wrap()	 :f_none()
end