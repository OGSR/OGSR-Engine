# XeGTAO

MIT-licensed Intel XeGTAO from https://github.com/GameTechDev/XeGTAO,
pinned to commit `a5b1686c7ea37788eeb3576b5be47f7c03db532c`.

`XeGTAO.h` and `XeGTAO.hlsli` originate in `Source/Rendering/Shaders/`.
Local modifications to `XeGTAO.hlsli`: the root-qualified include
`xegtao\XeGTAO.h`, required by OGSR's shader include resolver, and an explicit
saturate before converting denoised visibility to R8_UINT (the final rescale
can overshoot 1). Scalar swizzles are expanded to explicit vectors because
OGSR's FXC rejects forms such as `2.xx`. The same expansion is applied in
the adapted prefilter body. The wrapper uses XeGTAO's `lpfloat` texture type
verbatim because FXC treats it as distinct from `float` in texture templates.
`XeGTAO.h` is unchanged. See `LICENSE`.

Use backslashes in shader include paths: OGSR's virtual filesystem does not
normalize forward slashes during lookup, even when Windows/FXC accepts them.

OGSR adapters live in the parent directory as `ogsr_xegtao*`. The prefilter
adapts the upstream 16x16 routine to read linear view-space depth from OGSR's
G-buffer instead of converting hardware depth. Compute textures are padded
to multiples of 16 and border-filled, preserving five complete mip levels
even at odd or very small resolutions. Reconstruction accounts for padding
and the engine's projection jitter. Normals come from the existing G-buffer.
