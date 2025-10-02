function normal(shader, t_base, t_second, t_detail)
	  shader:begin("skybox", "skybox")
		: fog(false)
		: zb(false,false)
	shader:dx10texture("s_sky0", "$null")
	shader:dx10texture("s_sky1", "$null")	
	shader:dx10sampler("smp_rtlinear")	
end