local tex_base                = "water\\water_studen"
local tex_nmap                = "water\\water_normal"
local tex_dist                = "water\\water_dudv"
local tex_env0                = "$user$sky0"
local tex_env1                = "$user$sky1"

function normal (shader, t_base, t_second, t_detail)
	shader:begin     ("water_studen","water_studen")
        : sorting  (2, false)
        : blend    (true,blend.srcalpha,blend.invsrcalpha)
        : zb       (true,false)
        : distort  (true)
        : fog      (true)
	shader:sampler        ("s_base")       :texture  (tex_base)
	shader:sampler        ("s_nmap")       :texture  (tex_nmap)
	shader:sampler        ("s_env0")       :texture  (tex_env0)   : clamp()
	shader:sampler        ("s_env1")       :texture  (tex_env1)   : clamp()
	shader:sampler        ("s_position")   :texture  ("$user$position")
end