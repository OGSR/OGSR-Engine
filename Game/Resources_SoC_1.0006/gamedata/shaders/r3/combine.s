function element_0(shader, t_base, t_second, t_detail)
	shader:begin("combine_1", "combine_1")
		:fog(false)
		:zb(false, false)
		:blend(true, blend.invsrcalpha,blend.srcalpha)
		:dx10stencil(true, cmp_func.lessequal, 255, 0, stencil_op.keep, stencil_op.keep, stencil_op.keep)
		:dx10stencil_ref(1)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_diffuse", "$user$albedo")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("env_s0", "$user$env_s0")
	shader:dx10texture("env_s1", "$user$env_s1")
	shader:dx10texture("sky_s0", "$user$sky0")
	shader:dx10texture("sky_s1", "$user$sky1")

	jitter.jitter(shader)

	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_rtlinear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "combine_distort")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_distort", "$user$generic1")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "combine_tonemap")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_diffuse", "$user$albedo")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_tonemap", "$user$tonemap")

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_rtlinear")
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "combine_2_naa")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_bloom", "$user$bloom1")
	shader:dx10texture("s_ssfx_bloom", "$user$ssfx_bloom1")

	shader:dx10texture("s_mask_flare_1", "$user$SunShaftsMaskSmoothed")
	shader:dx10texture("s_mask_flare_2", "$user$sun_shafts1")
	shader:dx10texture("s_flares", "$user$flares")
	shader:dx10texture("s_noise", "fx\\blue_noise") --Simp: юзается в effects_flare.h!

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
	shader:dx10sampler("smp_linear")
end