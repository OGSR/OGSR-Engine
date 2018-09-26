local t_flare = "ogse\\effects\\LensFlare"
local t_glow = "glow\\glow_white"
local t_dirt = "ogse\\effects\\lensdirt"

function normal                (shader, t_base, t_second, t_detail)
  shader:begin                	("effects_lens_flare","effects_lens_flare")
        : sorting       		(3, false)
        : blend                	(true,blend.srcalpha,blend.one)
		: aref					(true, 2)	
        : zb                	(false,false)
        : distort        		(false)
        : fog                	(false)
  shader:sampler        ("s_flare")      :texture  (t_flare): clamp()
  shader:sampler        ("s_glow")       :texture  (t_glow): clamp()
  shader:sampler        ("s_dirt")       :texture  (t_dirt): clamp()
  shader:sampler        ("s_position")   :texture  ("$user$position"): clamp()
end
