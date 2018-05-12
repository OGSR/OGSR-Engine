function normal   (shader, t_base, t_second, t_detail)
  shader:begin    ("deffer_model_flat","deffer_base_aref_flat")
      --: fog       (true)
      : zb        (true,false)
      --: blend     (true,blend.srcalpha,blend.invsrcalpha)
      --: aref      (true,0)
      --: sorting   (1,true)
	  : emissive (true)

  shader:sampler  ("s_base")       :texture    (t_base)
end

function l_special	(shader, t_base, t_second, t_detail)
	shader:begin	("shadow_direct_model_my",	"collimator_ps")
			: zb 		(true,false)
			: fog		(false)
			: aref      (true,0)
			: emissive 	(true)
	log("models_lightplanes_00.l_special: "..tostring(t_base))
	shader:sampler	("s_base")      :texture	(t_base)
end