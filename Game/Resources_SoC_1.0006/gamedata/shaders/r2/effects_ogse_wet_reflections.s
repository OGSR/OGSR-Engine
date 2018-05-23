local tex_env0                = "$user$sky0"         -- "sky\\sky_8_cube"
local tex_env1                = "$user$sky1"         -- "sky\\sky_8_cube"

function normal                (shader, t_base, t_second, t_detail)
  shader:begin                	("null","ogse_wet_reflections")
        : sorting       		(3, false)
        : blend                	(true,blend.srcalpha,blend.invsrcalpha)
		: aref					(false, 2)	
        : zb                	(false,false)
        : distort        		(false)
        : fog                	(false)
  shader:sampler        ("s_env0")       :texture  (tex_env0)   : clamp()
  shader:sampler        ("s_env1")       :texture  (tex_env1)   : clamp()
  shader:sampler        ("s_position")   :texture  ("$user$position")	: clamp()
  shader:sampler        ("s_normal")   :texture  ("$user$normal")	: clamp()
  shader:sampler        ("s_image")	   :texture  ("$user$generic0")	: clamp() : f_linear ()
end