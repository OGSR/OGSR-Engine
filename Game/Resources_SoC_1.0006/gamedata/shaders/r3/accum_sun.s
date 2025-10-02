local function case_0_1(shader, t_base, t_second, t_detail)
	shader:begin("accum_sun", "accum_sun_near")
		:fog(false)
		:zb(true, false, true)
		:blend(false, blend.one, blend.zero)
	shader:dx10cullmode(1)
	shader:dx10zfunc(cmp_func.greater)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_diffuse", "$user$albedo")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	
	jitter.jitter(shader)
	
	shader:dx10sampler("smp_smap")
end

function element_0(shader, t_base, t_second, t_detail)
	case_0_1(shader, t_base, t_second, t_detail)
end

function element_1(shader, t_base, t_second, t_detail)
	case_0_1(shader, t_base, t_second, t_detail)
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("accum_sun", "accum_sun_far")
		:fog(false)
		:zb(true, false)
		:blend(false, blend.one, blend.zero)
	shader:dx10cullmode(1)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_diffuse", "$user$albedo")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	
	jitter.jitter(shader)
	
	local s_smap = shader:dx10sampler("smp_smap")
	shader:dx10adress(s_smap, 4)
	shader:dx10bordercolor(s_smap, 255, 255, 255, 255)
end