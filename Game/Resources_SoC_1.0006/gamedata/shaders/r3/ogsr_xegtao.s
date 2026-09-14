function element_0(shader, t_base, t_second, t_detail)
    shader:begin("stub_screen_space", "ogsr_xegtao_export")
        :fog(false)
        :zb(false, false)
    shader:dx10texture("s_position", "$user$position")
    shader:dx10texture("s_xegtao", "$user$xegtao")
end
