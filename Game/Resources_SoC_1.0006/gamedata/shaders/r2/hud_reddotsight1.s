function normal		(shader, t_base, t_second, t_detail)
  shader:begin  	("hud_reddotsight1","hud_reddotsight1")
      : fog    		(false)
      : zb     		(true,false)
      : blend   	(true,blend.srcalpha,blend.one)
      : aref    	(true,0)
      : sorting		(2, true)
  shader:sampler	("s_base")      :texture  (t_base): clamp()
end
