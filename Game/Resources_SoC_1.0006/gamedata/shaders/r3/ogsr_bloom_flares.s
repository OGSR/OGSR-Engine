function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_flares")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_emissive", "$user$ssfx_bloom_emissive")
	shader:dx10texture("s_lenscolors", "fx\\lens_colors")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp4")
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp4")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp4_2")
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp4_2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end