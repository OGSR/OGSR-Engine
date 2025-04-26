function normal         (shader, t_base, t_second, t_detail)
        shader:begin    ("clouds","clouds")
                        : fog               (false)
--	TODO: DX10: Check if this is ok.
--                        : zb                (true,false)
						: zb                (false,false)
                        : sorting        	(3, true)
                        : blend             (true, blend.srcalpha,blend.invsrcalpha)
	shader:dx10texture	("s_clouds0", "null")
	shader:dx10texture	("s_clouds1", "null")

	shader:dx10sampler	("smp_base")
end
