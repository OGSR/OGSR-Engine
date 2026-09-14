local function setup_ao(shader, pixel_shader)
    shader:begin("stub_screen_space", pixel_shader)
        :fog(false)
        :zb(false, false)
    shader:dx10texture("s_position", "$user$position")
    shader:dx10sampler("smp_nofilter")
end

function element_0(shader, t_base, t_second, t_detail)
    setup_ao(shader, "ogsr_ao")
end

function element_1(shader, t_base, t_second, t_detail)
    setup_ao(shader, "ogsr_ao_half")
end

function element_2(shader, t_base, t_second, t_detail)
    setup_ao(shader, "ogsr_ao_resolve")
    shader:dx10texture("s_ao_half", "$user$ao_half")
end
