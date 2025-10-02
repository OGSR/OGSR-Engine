function element_0(shader, t_base, t_second, t_detail)
	shader:begin("dumb", "dumb")
		:fog(false)
		:zb(true, false)
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_t", "dumb")
		:fog(false)
		:zb(false, false)
	shader:dx10color_write_enable(false, false, false, false)
	shader:dx10cullmode(1)
	shader:dx10stencil(true, cmp_func.always, 0, 254, stencil_op.zero, stencil_op.zero, stencil_op.zero)
end
