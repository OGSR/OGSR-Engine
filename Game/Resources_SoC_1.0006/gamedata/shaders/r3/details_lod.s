function l_special (shader, t_base, t_second, t_detail)
    shader :begin("lod","lod")
    shader  : blend (false, blend.one, blend.zero)
            : zb (true,  true)
            : fog (false)

            :dx10stencil (true, cmp_func.always,
                                255 , 127, 
                                stencil_op.keep, stencil_op.replace, stencil_op.keep)
            :dx10stencil_ref (1)

    shader:dx10texture("s_base", t_base)
    shader:dx10texture("s_hemi", t_base .. "_nm")

    shader:dx10sampler("smp_base");
    shader:dx10sampler("smp_linear");
end
