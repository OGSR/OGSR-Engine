function element_0(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "postprocess_cas")
    :fog(false)
    :zb(false, false)
    shader:dx10texture("t_current", "$user$postprocess0")
    shader:dx10sampler("SamplerLinearClamp")
end

-- Samples $user$generic_combine (DLSS/FSR output) instead of postprocess0.
function element_1(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "postprocess_cas")
    :fog(false)
    :zb(false, false)
    shader:dx10texture("t_current", "$user$generic_combine")
    shader:dx10sampler("SamplerLinearClamp")
end
