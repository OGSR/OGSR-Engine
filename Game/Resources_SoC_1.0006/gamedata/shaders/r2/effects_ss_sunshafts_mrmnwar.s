function element_0(shader, t_base, t_second, t_detail)		-- [0]
	shader:begin("null", "ssss_mrmnwar_mask")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_position") 	:texture("$user$position")				:clamp() :f_none()
	shader:sampler	("s_image") 	:texture("$user$generic0")				:clamp() :f_none()
end

function element_1(shader, t_base, t_second, t_detail)		-- [1]
	shader:begin("null", "ssss_mrmnwar_mask_blur")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_mask")		:texture("$user$SunShaftsMask")			:clamp() :f_none()
end

function element_2(shader, t_base, t_second, t_detail)		-- [2]
	shader:begin("null", "ssss_mrmnwar_generation")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_mask_blur")	:texture("$user$SunShaftsMaskSmoothed")	:clamp() :f_none()
end

function element_3(shader, t_base, t_second, t_detail)		-- [3]
	shader:begin("null", "ssss_mrmnwar_generation")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_mask_blur")		:texture("$user$SunShaftsPass0")	:clamp() :f_none()
end

function element_4(shader, t_base, t_second, t_detail)		-- [4]
	shader:begin("null", "ssss_mrmnwar_display")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image") 	:texture("$user$generic0")				:clamp() :f_none()
	shader:sampler	("s_sunshafts") :texture("$user$SunShaftsMaskSmoothed")	:clamp() :f_none()
end