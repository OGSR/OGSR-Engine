function element_0(shader, t_base, t_second, t_detail)
	shader:begin("accum_sun", "accum_sun_cascade2")
		:fog(false)
		:zb(true, false)
		:blend(false, blend.one, blend.zero)
	shader:dx10cullmode(1)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_sun_cascade_2")
	shader:dx10texture("s_diffuse", "$user$albedo")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	
	jitter.jitter(shader)
	
	local s_smap = shader:dx10sampler("smp_smap")
	shader:dx10adress(s_smap, 4)
	shader:dx10bordercolor(s_smap, 255, 255, 255, 255)
end