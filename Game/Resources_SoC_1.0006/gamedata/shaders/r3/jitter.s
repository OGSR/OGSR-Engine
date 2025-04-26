function jitter(shader)
	shader:dx10texture("jitter0", "$user$jitter_0")
	shader:dx10texture("jitter1", "$user$jitter_1")
	shader:dx10sampler("smp_jitter")
end