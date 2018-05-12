local tex_env0                = "$user$sky0"
local tex_env1                = "$user$sky1"

function normal(shader, t_base, t_second, t_detail)
	shader:begin 	("effects_ogse_reflection","effects_ogse_reflection")
        : sorting	(2, false)
        : blend		(true,blend.srcalpha,blend.invsrcalpha)
        : zb		(true,false)
        : distort	(false)
        : fog		(true)
	shader:sampler        ("s_env0")       :texture  (tex_env0)   : clamp()
	shader:sampler        ("s_env1")       :texture  (tex_env1)   : clamp()
	shader:sampler        ("s_position")   :texture  ("$user$position")
	shader:sampler        ("s_image")	   :texture  ("$user$generic0")	: clamp() : f_linear ()
end