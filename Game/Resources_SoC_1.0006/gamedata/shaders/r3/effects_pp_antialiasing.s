--// SMAA -------------------------------------------------------------------------------------------
function element_2(shader, t_base, t_second, t_detail)		-- [2] SMAA: pass 0 - edge detection
	shader:begin("stub_screen_space", "smaa_edge_detect")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", 	"$user$generic0")
end

function element_3(shader, t_base, t_second, t_detail)		-- [3] SMAA: pass 1 - blending weight calculation
	shader:begin("stub_screen_space", "smaa_bweight_calc")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_edgetex", 	"$user$smaa_edgetex")
	shader:dx10texture	("s_areatex", 	"shaders\\smaa_area_tex_dx10")
	shader:dx10texture	("s_searchtex", "shaders\\smaa_search_tex")
end

function element_4(shader, t_base, t_second, t_detail)		-- [4] SMAA: pass 2 - neighbourhood blending
	shader:begin("stub_screen_space", "smaa_neighbour_blend")
		:fog			(false)
		:zb				(false, false)
	shader:dx10texture	("s_image", 	"$user$generic0")
	shader:dx10texture	("s_blendtex", 	"$user$smaa_blendtex")
end
