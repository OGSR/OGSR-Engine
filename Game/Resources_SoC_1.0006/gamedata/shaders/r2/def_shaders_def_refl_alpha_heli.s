local heli_refl = "sky\\sky_heli"

function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("heli_refl", "heli_refl")
			: fog		(true)
			: zb 		(true,true)
			: blend		(true,blend.srcalpha,blend.invsrcalpha)
			: sorting	(2, true)
			: distort	(false)
	shader:sampler	("s_base")      :texture (t_base)
	shader:sampler	("s_lmap")		:texture (t_second)
	shader:sampler	("s_hemi")		:texture (string.sub(t_second, 1, string.len(t_second)-1) .. "2")
	shader:sampler	("s_env")      :texture (heli_refl): clamp()
end