function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_build", "bloom_luminance_1")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$bloom1")
	shader:dx10sampler("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_filter", "bloom_luminance_2")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$lum_t64")
	shader:dx10sampler("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_filter", "bloom_luminance_3")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_image", "$user$lum_t8")
	shader:dx10texture("s_tonemap", "$user$tonemap_src")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_nofilter")
end
