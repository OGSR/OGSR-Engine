local t_dirt = "ogse\\effects\\lensdirt"

function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("effects_sun", "effects_sun")
			: fog		(false)
			: zb 		(true,false)
			: blend		(true,blend.srcalpha,blend.one)
			: aref 		(true,2)
			: sorting	(3, false)
    shader:sampler 	("s_base")		:texture        (t_base)		:clamp()
	shader:sampler        ("s_dirt")       :texture  (t_dirt): clamp()
end