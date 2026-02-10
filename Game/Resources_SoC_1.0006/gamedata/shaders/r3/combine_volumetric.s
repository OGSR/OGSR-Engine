function normal         (shader, t_base, t_second, t_detail)
    shader:begin        ("stub_screen_space", "combine_volumetric")
            : fog       (false)
            : zb        (false,false)
            : blend     (true,blend.one,blend.one)
            : sorting   (2, false)

	shader:dx10texture	("s_vollight1", "$user$generic2")
	shader:dx10texture	("s_vollight2", "$user$generic3")
	--shader:dx10texture 	("noise_tex", 	"fx\\blue_noise")
	--shader:dx10texture 	("s_position", 	"$user$position")

	--shader:dx10sampler	("smp_linear")
	--shader:dx10sampler	("smp_nofilter")
end