function normal		(shader, t_base, t_second, t_detail)
	shader	:begin		("portal","portal")
			:fog		(true)
			:zb 		(true,false)
			:sorting	(3, true)
			:blend		(true,blend.srcalpha,blend.invsrcalpha)
end
