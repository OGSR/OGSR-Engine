function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp64")
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp64")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp32")
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp32_2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp16")
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp16_2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp8")
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp8_2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
end

function element_4(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_upsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_downsample", "$user$ssfx_bloom_tmp4")
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp4_2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
end
