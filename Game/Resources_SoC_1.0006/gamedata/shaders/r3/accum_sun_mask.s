function element_0(shader, t_base, t_second, t_detail)
	shader:begin("accum_mask", "dumb")
		:fog(false)
		:zb(true, false)
	shader:dx10color_write_enable(false, false, false, false)
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("accum_mask", "dumb")
		:fog(false)
		:zb(true, false)
	shader:dx10color_write_enable(false, false, false, false)
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_t", "accum_sun_mask")
		:fog(false)
		:zb(false, false)
		:blend(true, blend.zero,blend.one)
		:aref(true, 1)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_diffuse", "$user$albedo")
	shader:dx10sampler("smp_nofilter")
	shader:dx10color_write_enable(false, false, false, false)
end