function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_build", "bloom_build")
		:fog(false)
		:zb(false, false)
        :blend(false, blend.srcalpha,blend.invsrcalpha)
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_tonemap", "$user$tonemap")
	shader:dx10sampler("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_filter", "bloom_filter")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$bloom1")
	shader:dx10sampler("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_filter", "bloom_filter")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$bloom2")
	shader:dx10sampler("smp_rtlinear")
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_build", "bloom_filter_f")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$bloom1")
	shader:dx10sampler("smp_rtlinear")
end

function element_4(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_build", "bloom_filter_f")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_bloom", "$user$bloom2")
	shader:dx10sampler("smp_rtlinear")
end
