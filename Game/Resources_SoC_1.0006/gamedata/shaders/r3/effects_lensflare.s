local t_glow = "shaders\\glow_white"
local t_flare = "shaders\\lensflare"
local t_dirt = "shaders\\lensdirt"

function normal(shader, t_base, t_second, t_detail)
	shader:begin("effects_lensflare", "effects_lensflare")
        : sorting	(3, false)
        : blend		(true,blend.srcalpha,blend.one)
		: aref		(true, 2)	
        : zb		(false,false)
        : distort	(false)
        : fog		(false)
	shader:dx10texture("s_glow", t_glow)
	shader:dx10texture("s_flare", t_flare)
	shader:dx10texture("s_dirt_mask", t_dirt)
	shader:dx10texture("s_position", "$user$position")

	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_base")	
end