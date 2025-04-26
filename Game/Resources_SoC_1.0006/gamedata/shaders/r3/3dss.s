function element_0(shader, t_base, t_second, t_detail)
    shader:begin("3dss_base","3dss_base")
    : fog (false)       
    : blend(true, blend.zero, blend.one)
    : scopelense(true)
    shader:dx10texture("s_base", t_base)
    shader:dx10sampler("smp_base")
    shader:dx10stencil(true, cmp_func.always, 2, 2, stencil_op.replace, stencil_op.replace, stencil_op.replace)
    shader:dx10stencil_ref(2)
end

function element_1(shader, t_base, t_second, t_detail)
    shader:begin("3dss_base","3dss_zwrite")
	: zb(false, true)
    shader:dx10texture("rt_tempzb", "$user$temp_zb")
    shader:dx10sampler("smp_rtlinear")
    shader:dx10stencil(true, cmp_func.equal, 2, 2, stencil_op.zero, stencil_op.zero, stencil_op.zero)
    shader:dx10stencil_ref(2)
end

local function models_scope_reticle(rendertarget_name, shader, t_base, t_second, t_detail)
	shader:begin("3dss_reticle", "3dss_reticle")
	: fog(true)
	: zb(true, false)
	: blend(true, blend.srcalpha, blend.invsrcalpha)
	: aref(true, 0)
	: sorting(2, true)
	: distort(true)
	shader:dx10texture("s_base", t_base)
	shader:dx10texture("s_prev_frame", rendertarget_name)
	shader:dx10texture("s_tonemap", "$user$tonemap")
	shader:dx10texture("s_heat", "$user$heat")
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_blur_2", "$user$blur_2")
	shader:dx10texture("s_inside", "shaders\\scope_utility\\inside")
	shader:dx10texture("s_dirt", "shaders\\scope_utility\\dirt")
	shader:dx10texture("s_reflection", "shaders\\scope_utility\\reflection")
	shader:dx10texture("s_heat_map", "shaders\\scope_utility\\heat_map")
	shader:dx10sampler("smp_base")
end

function element_2(...)
    models_scope_reticle("$user$generic_combine", ...)
end

function element_3(shader, t_base, t_second, t_detail)
    shader:begin("model_def_lplanes", "base_lplanes")
    : fog(false)
    : zb(true,false)
    : blend(true,blend.srcalpha,blend.one)
    : aref(true,0)
    : sorting(2, true)
    shader:dx10texture("s_base", t_base)
    shader:dx10sampler("smp_base")
end

function element_4(...)
    models_scope_reticle("$user$generic_combine_scope", ...)
end
