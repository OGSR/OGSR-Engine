local t_dirt = "ogse\\effects\\lensdirt"

function normal                (shader, t_base, t_second, t_detail)
  shader:begin                	("effects_lens_dirt","effects_lens_dirt")
        : sorting       		(3, false)
        : blend                	(true,blend.one,blend.one)
		: aref					(true, 2)	
        : zb                	(false,false)
        : distort        		(false)
        : fog                	(false)
  shader:sampler        ("s_dirt")       :texture  (t_dirt): clamp()
end