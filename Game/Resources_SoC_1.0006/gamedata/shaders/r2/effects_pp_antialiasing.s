--// SMAA -------------------------------------------------------------------------------------------
function element_2(shader, t_base, t_second, t_detail)		-- [2] SMAA: pass 0 - edge detection
	shader:begin("null", "smaa_edge_detect")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")		:texture("$user$generic0") 				:clamp() :f_linear()
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] SMAA: pass 1 - blending weight calculation
	shader:begin("null", "smaa_bweight_calc")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_edgetex")	:texture("$user$smaa_edgetex") 			:clamp() :f_linear()
	shader:sampler	("s_areatex")	:texture("shaders\\smaa_area_tex_dx9") 	:clamp() :f_linear()
	shader:sampler	("s_searchtex")	:texture("shaders\\smaa_search_tex") 	:clamp() :f_point()
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] SMAA: pass 2 - neighbourhood blending
	shader:begin("null", "smaa_neighbour_blend")
		:fog		(false)
		:zb			(false, false)
	shader:sampler	("s_image")		:texture("$user$generic0") 				:clamp() :f_linear()
	shader:sampler	("s_blendtex")	:texture("$user$smaa_blendtex") 		:clamp() :f_linear()
end
