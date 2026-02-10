function normal(shader, t_base, t_second, t_detail)
    shader:begin("combine_peak", "combine_peak")
	:fog(false)
	:zb(false, false)
	:blend(true, blend.one,blend.one)

	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_smap", "$user$smap_lights")

	shader:dx10sampler("smp_smap")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end