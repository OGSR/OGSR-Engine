function normal		(shader, t_base, t_second, t_detail)
	shader:begin("effects_bullet_tracer","effects_bullet_tracer")
			:zb(false, false)
			:blend(true, blend.one, blend.one)

	shader:dx10texture	("s_base", t_base)
	shader:dx10texture	("s_position", "$user$position")
	shader:dx10sampler	("smp_base")
	shader:dx10sampler	("smp_nofilter")
end
