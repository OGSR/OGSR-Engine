function normal        (shader, t_base, t_second, t_detail)
    shader:begin    ("deffer_model_flat","deffer_base_flat")
    
    : fog (false) 
	: zb  (true,true)

    shader:dx10stencil    (     true, cmp_func.always, 
                            255 , 127, 
                            stencil_op.keep, stencil_op.replace, stencil_op.keep)
    shader:dx10stencil_ref    (1)        
    
    shader:dx10texture("s_base",    t_base)
    shader:dx10texture("s_bump",    t_base.."_bump")
    shader:dx10texture("s_bumpX",   t_base.."_bump#")

    shader:dx10sampler("smp_base") 
	shader:dx10sampler ("smp_rtlinear")
end

function l_special	(shader, t_base, t_second, t_detail)
	shader:begin	("shadow_direct_model",	"accum_emissivel")
			: zb 		(true,true)
			: fog		(true)
			: emissive 	(true)
end



