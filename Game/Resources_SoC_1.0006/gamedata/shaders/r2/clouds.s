function normal         (shader, t_base, t_second, t_detail)
        shader:begin    ("clouds","clouds")
                        : fog               (false)
--                        : zb                (true,false)
--	Igor: avoid siluets.
                        : zb                (false,false)
                        : sorting        	(3, true)
                        : blend             (true, blend.srcalpha,blend.invsrcalpha)
        shader:sampler        ("s_clouds0")   :texture        ("null")        : wrap() : f_anisotropic()
        shader:sampler        ("s_clouds1")   :texture        ("null")        : wrap() : f_anisotropic()
        shader:sampler        ("s_tonemap")   :texture        ("$user$tonemap")
end
