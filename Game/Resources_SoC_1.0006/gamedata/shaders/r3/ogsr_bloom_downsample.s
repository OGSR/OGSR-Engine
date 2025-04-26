function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom1")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp2")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp4")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp8")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_4(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp16")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end

function element_5(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_bloom_downsample")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$ssfx_bloom_tmp32")

	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end
