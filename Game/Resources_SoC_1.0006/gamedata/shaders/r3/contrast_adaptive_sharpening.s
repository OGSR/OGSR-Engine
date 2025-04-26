function element_0(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "postprocess_cas")
    :fog(false)
    :zb(false, false)
    shader:dx10texture("t_current", "$user$generic0")
    shader:dx10sampler("SamplerLinearClamp")
end

function element_1(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "postprocess_cas")
    :fog(false)
    :zb(false, false)
    shader:dx10texture("t_current", "$user$generic_combine_scope")
    shader:dx10sampler("SamplerLinearClamp")
end
