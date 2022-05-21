
function normal(shader, t_base, t_second, t_detail)
	shader:begin("model_def_lplanes", "models_lense_scope")
      : fog    		(false)
      : zb     		(true,false)
      : blend   	(true,blend.srcalpha,blend.one)
      : aref    	(true,0)
      : sorting		(2, true)
	shader:dx10texture("s_base", t_base)
	shader:dx10texture("s_vp2",	"$user$viewport2")	
	shader:dx10sampler("smp_base")	
end
