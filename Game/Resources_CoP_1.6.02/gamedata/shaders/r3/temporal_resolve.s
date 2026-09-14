function element_0(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "temporal_resolve")
        :fog(false)
        :zb(false, false)
    shader:dx10texture("t_current", "$user$generic0")
    shader:dx10sampler("SamplerLinearClamp")
end
