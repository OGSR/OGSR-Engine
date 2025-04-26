local tex_base = "water\\water_water"
local tex_nmap = "water\\water_normal"

function normal(shader, t_base, t_second, t_detail)
    shader:begin("water_puddles", "water_puddles")
        :sorting(2, false)
        :blend(true, blend.srcalpha, blend.invsrcalpha)
        :zb(true, false)
        :fog(true)

    shader:dx10texture("s_base", tex_base)
    shader:dx10texture("s_nmap", tex_nmap)

    shader:dx10texture("s_env0", "$user$sky0")
    shader:dx10texture("s_env1", "$user$sky1")

    shader:dx10texture("env_s0", "$user$env_s0")
    shader:dx10texture("env_s1", "$user$env_s1")

    shader:dx10texture("s_position", "$user$position")
    shader:dx10texture("s_image", "$user$generic_temp")
    shader:dx10texture("t_zbuffer", "$user$zbuffer")

    shader:dx10sampler("smp_base")
    shader:dx10sampler("smp_linear")
    shader:dx10sampler("smp_nofilter")
    shader:dx10sampler("smp_rtlinear")
end
