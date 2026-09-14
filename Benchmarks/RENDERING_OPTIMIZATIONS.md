# Rendering optimizations

Living notes for work on `optimization/rendering-improvements`. Each entry is what changed, why, and what can break.

Measure with **`ReleaseTracyProfiler`**, Shift+F10, Main window GPU timestamps (or RenderDoc on that build). Plain Release `rs_stats` will not show these GPU copies.

Single-frame Tracy CSVs are noisy (~1–2 ms Frame swing). Compare named passes (HUD, sun, sorted, `combine_1` must match) before trusting Frame.

---

## Skip unused depth snapshots

**Status:** done  
**Files:** `ogsr_engine/Layers/xrRenderPC_R4/r4_R_render.cpp`

### What it did

Every frame used to `CopyResource` the scene depth buffer three times:

| Destination | When it is taken | Who reads it |
|---|---|---|
| `rt_tempzb` (`$user$temp_zb`) | After world geo, **before** HUD | 3D-scope z-write (`3dss_zwrite.ps`) |
| `rt_tempzb_dof` (`$user$zbuffer_dof`) | After HUD, **before** scope depth | DOF (`ogsr_dof.s`) |
| `rt_zbuffer` (`$user$zbuffer`) | After HUD + scope depth | TAA, DLSS, FSR3, puddles |

The first two copies are now skipped unless that feature will run this frame:

- `rt_tempzb` only if `mapScopeHUD` is not empty (same map `r_dsgraph_render_hud_scope_depth` draws).
- `rt_tempzb_dof` only if DOF params are non-zero (same test as `phase_dof`).

`rt_zbuffer` still copies every frame. Scene depth stays bound as a DSV, so later passes cannot sample it in place.

Copies now use `rt_Base_Depth->pSurface` instead of `GetResource` / `Release` on the DSV each time.

### What it can affect

- **3D scopes:** the tube should still show the world without HUD weapons in the depth. If `mapScopeHUD` is empty while a scope still samples `$user$temp_zb`, depth in the tube will be stale or wrong.
- **DOF (zoom / reload / script `set_dof_params`):** weapon and world should still defocus using HUD-inclusive depth. If DOF runs while all four `dof_params` look zero at copy time, it will sample a stale `$user$zbuffer_dof`.
- **TAA / DLSS / FSR / puddles:** unchanged; they still get a fresh `$user$zbuffer`.
- **Tracy markers:** `copy_zbuffer_scope` and `copy_zbuffer_scope_depth` only appear when the copy actually happens. `copy_zbuffer` should always appear.

### Sticky `copy_zbuffer_scope_depth` after first aim

**Status:** fixed  
**Files:** `ogsr_engine/xrGame/Weapon.cpp`

`CWeapon::UpdateDof` used to `set_dof_params` and *then* `clamp` the fade to `[0, 1]`. Fade-out only runs while `dof_zoom_effect > 0`, so the last frame wrote a leftover (tiny or negative) and never wrote zeros. `phase_dof` and the depth-copy gate both use `fis_zero` on those four values, so after the first ADS they kept running until restart. Clamp now happens first; the last fade frame exports `(0,0,0,0)`.

`copy_zbuffer_scope` is independent (3D-scope HUD map). That one should already vanish when you unaim.

### How to check

- Walk around, no scope, no DOF: two extra depth copies gone; image unchanged.
- 3D scope: reticle / world depth in the tube looks like before (no HUD mesh in the glass).
- Iron sights / reload DOF: blur still respects nearby HUD and world depth.
- DLSS/FSR/TAA: no extra ghosting vs before. `copy_zbuffer` still fires.

---

## Tracy editor: GPU pass filter and CSV export

**Status:** done (Tracy build only, not a rendering change)  
**Files:** `ogsr_engine/xrGame/embedded_editor/embedded_editor_main.cpp`

Shift+F10 Main window: case-insensitive name filter, scrollable pass list, Export CSV of the current frame (all depths) to `$app_data_root$/gpu_passes_YYYYMMDD_HHMMSS.csv` and the clipboard (`index,stack,name,time_ms`). Depth slider still applies to the on-screen list only. Needs `Build_ReleaseTracy.cmd` / `CONFIGURATION_GA=ReleaseTracyProfiler`. Does not affect Release visuals or GPU work.

---

## Post-process ping-pong: CAS after DLSS/FSR

**Status:** done (slice 1; slice 2 below removes the rest)  
**Files:** `RenderTargetPhaseAA.cpp`, `r4_rendertarget.h`, `r4_rendertarget_phase_combine.cpp`, `contrast_adaptive_sharpening.s`

### What it did

Display-sized post used to draw into `rt_Generic_combine` then `CopyResource` into `rt_Postprocess_0` because shaders sample `$user$postprocess0`. With DLSS/FSR that was two full RGBA16F copies before anything else: upscale output → postprocess0, then CAS → copy back.

When CAS runs after a temporal upscaler:

- Skip the upscale → postprocess0 copy
- CAS samples `$user$generic_combine` (element 1) and writes `rt_Postprocess_0`
- Skip the CAS copy

TAA/SMAA/no-AA keep the old seed (`copy_pp_from_generic0`, CAS element 0). Do **not** ping-pong with `rt_Generic_0` — that buffer is render-sized when DLSS/FSR is on.

### What it can affect

- **CAS after DLSS/FSR:** sharpening should match. If shaders did not recook, element 1 is missing and the engine falls back to the old copies.
- **3D scopes:** see slice 2. `copy_pp_3dss` is no longer “whenever a scope is up”; it only runs when the latest image is still in postprocess0.

### How to check

- Tracy, DLSS+CAS, no ADS: `copy_pp_after_upscale` and `copy_pp_cas` gone; `CAS` is draw-only.
- TAA or `r_aa_cas 0`: `copy_pp_after_upscale` / `copy_pp_from_generic0` still seed the pair.

### Measured (cordon_pingpong.csv)

Versus `cordon_depth_optimization.csv`: CAS 0.078 → 0.064 ms. Leftover `copy_pp_ssss` / `copy_pp_combine2` were 0.026 ms each. HUD/sorted did not match that capture — do not use its Frame.

---

## Post-process ping-pong: remaining display-sized copies

**Status:** done  
**Files:** `r4_rendertarget.h`, `RenderTargetRenderScreenQuad.cpp`, `RenderTargetPhaseAA.cpp`, `RenderTargetPhaseSSSS.cpp`, `r4_rendertarget_phase_combine.cpp`, `r4_rendertarget_phase_PP.cpp`, `r4_rendertarget_phase_lut.cpp`, `rendertarget_phase_dof.cpp`, `rendertarget_phase_gasmask_dudv.cpp`, `rendertarget_phase_nightvision.cpp`, `rendertarget_phase_thermalvision.cpp`, `RenderTargetPhaseRainDrops.cpp`, `rendertarget_phase_fakescope.cpp`

### What it did

Each post writer used to draw into combine then `CopyResource` into postprocess0 (~0.026 ms per blit). The two display-sized buffers are now a ping-pong pair:

- `m_pp_current_is_combine` tracks which one holds the latest image (`false` = `rt_Postprocess_0`).
- After `set_Element`, `pp_remap_scene_srv` rebinds any `$user$postprocess0` / `$user$generic_combine` slot to the current buffer via the existing `CTexture` SRV (no `surface_set`).
- Each writer samples current, draws into the other RT, then flips. Remap is on only between `BeginPostprocess` and `phase_pp` so TAA/SMAA/SSR are untouched.

DLSS/FSR still write combine. Skipping the upscale→postprocess0 copy marks combine as current so CAS element 1 does not sample the RT it is about to write.

3DSS still samples `$user$generic_combine` while drawing into `rt_Postprocess_0` (cannot remap: that would bind the color RT as an SRV). If SSSS or heat overlay already left the latest image in combine, that copy is skipped. If CAS (or another ping-pong write) left it in postprocess0, `copy_pp_3dss` still syncs combine first.

`copy_pp_after_upscale` / `copy_pp_from_generic0` remain when CAS is off or there is no temporal upscaler — seed into the pair, not a per-pass tax.

### What it can affect / regressions to watch

Two failure modes: **stale buffer** (effect missing, or last frame’s effect) and **3D-scope glass sampling the wrong RT**.

**3D scopes** (reticle is not remapped):

| Setup | If broken, the tube shows |
|---|---|
| DLSS/FSR + CAS, shafts **off** | Unsharpened world (`copy_pp_3dss` should still fire) |
| Shafts **on** | World without shafts (copy should *not* fire; combine already has them) |
| Thermal NVG (`pnv` 2/3) | Glass missing the heat overlay, or reticle heat-tinted |
| Aim / unaim / swap weapons | One frame of garbage, or it sticks |
| `r_dlss_3dss_scale_factor` > 1 | Separate `$user$generic_combine_scope` path; should be unchanged |

**Missed remap** (odd vs even flip count). `phase_pp` samples combine; after an odd number of flips the latest image is in postprocess0 and only remap saves it. Drunk / radiation / gray / dual-vision / colormap is the cheapest way to see that. Flip the count on purpose: CAS only; CAS + shafts; add iron-sight DOF, LUT, gasmask (two flips), NVG 1, rain.

If a pass sampled the wrong buffer:

- **Shafts:** rays missing, or stuck from the previous camera angle
- **DOF:** HUD in focus while the world isn’t, or the reverse after unaim (also still check the sticky-DOF fix above)
- **LUT:** grade pops in a frame late
- **Gasmask:** visor reflection / breath on an unmasked scene
- **NVG 1:** green on a pre-DOF or pre-mask frame
- **Rain drops:** drops on a scene without shafts/DOF
- **CAS:** no sharpen, or a D3D11 “simultaneous SRV and RT” error if it samples the RT it writes
- **`combine_2` motion blur:** blur from the previous pose when turning quickly

**Fallback seeds** (one look each): TAA, SMAA (skips CAS), `r_aa_cas 0`, FSR, AA off. You should still see `copy_pp_after_upscale` or `copy_pp_from_generic0`, and no per-pass `copy_pp_*` after that.

Recook shaders if CAS looks off. Alt-tab / change DLSS quality / change resolution once (RTs recreate; flags reset next frame).

### How to check

- Tracy, DLSS+CAS, shafts on, no ADS: `copy_pp_ssss`, `copy_pp_combine2`, `copy_pp_dof`, `copy_pp_lut`, `copy_pp_nvg`, `copy_pp_gasmask_*`, `copy_pp_rain` gone. Image unchanged.
- 3D scope, shafts off: `copy_pp_3dss` present; tube is sharpened.
- 3D scope, shafts on: `copy_pp_3dss` absent; tube has shafts + CAS.
- TAA or `r_aa_cas 0`: seed copy still fires; later per-pass copies still gone.

### Measured (cordon_pingpong2.csv)

Versus slice 1 (`cordon_pingpong.csv`): `copy_pp_ssss` / `copy_pp_combine2` gone. `phase_ss_ss` 0.239 → 0.213 ms (−0.026, matches the 0.0261 ms copy). `combine_2` 0.248 → 0.220 ms (−0.028, matches 0.0264). CAS stays draw-only at 0.063 ms. Sun / lights / DLSS / blur / bloom match. ~0.05 ms this slice, ~0.07 ms across both ping-pong slices. Frame 60.8 → 59.2 is mostly a cheaper `combine_1` plus unmatched sorted transparents — do not quote that as ping-pong. HUD finally matched baseline (0.10 vs 0.09 ms).

---

## Gate whole PHASE_BLUR

**Status:** skipped  
SSFX bloom is forced on and samples `$user$blur_2`. The six-pass pyramid (~0.16 ms) always has a consumer. A 1/4+1/8 skip would be tiny; not worth it.

---

## Skip SSSS RT clears when shafts off

**Status:** skipped  
Two display-sized clears. The flare comment is stale (`combine_2_naa.ps` does not sample `s_mask_flare_*`). Tiny; left alone.

---

## SSFX bloom draws emissive twice

**Status:** skipped  
**Files:** `ogsr_engine/Layers/xrRenderPC_R4/r4_R_render.cpp` (472–479)

With `R2FLAG_SSFX_BLOOM` (always on) the same `mapEmissive` / `mapHUDEmissive` meshes are drawn into `rt_Accumulator` (lighting) and again into render-sized `rt_ssfx_bloom_emissive` (RGBA8, sampled by `ogsr_bloom.ps` without the scene threshold). Plus a full-res clear of that RT every frame.

### Why it is not worth it

- **Empty view (Cordon captures):** `DEFER_SELF_ILLUM` is already **0.000 ms**. Empty maps skip both draws. The leftover is one RGBA8 clear + RT bind — same class as the SSSS clears we skipped. Not in the same league as the 0.026 ms display-sized RGBA16F copies.
- **Lamps / PDA / detectors on screen:** you pay a second geometry pass of those meshes, not a fullscreen blit. A handful of quads, not perceptible bandwidth.
- **MRT to draw once into both RTs** needs every emissive pixel shader (`accum_emissive*`, `accum_lamp`, addon `l_special`) to write `SV_Target0` + `SV_Target1` and recook. Wrong output = missing lamp lighting or missing lamp bloom. Not worth that risk for a sub-copy saving.

Skipping the clear when the maps are empty still ghosts last frame’s lamps unless bloom is told to ignore `s_emissive` — another shader recook for the same tiny clear.

Old `phase_bloom` (~0.020 ms, includes luminance for exposure) still runs and `combine_2_naa.ps` still samples `$user$bloom1`. That is a separate dual-bloom question; do not rip it out without a visual check. `phase_ssfx_bloom` itself (~0.088 ms) is the real bloom work.

---

## Remaining

- Full-res SSR when wet
- `WaitOnSwapChain` busy-wait

---

## Dedicated AO pass and optional half-resolution evaluation

**Status:** implemented; in-game visual and performance comparison pending

**Files:** `r4_rendertarget_phase_ao.cpp`, `r4_rendertarget_phase_combine.cpp`, `r4_rendertarget.{h,cpp}`, `xrRender_console.{h,cpp}`, `combine.s`, `combine_1.ps`, `combine_1_ao.ps`, `ogsr_ao*.{ps,s}`

### What changed

SSDO and GTAO can now produce a separate visibility texture, consumed by `combine_1` before the existing colored-AO and ambient-light treatment. This creates the common output used by the subsequent XeGTAO integration described below.

`r_ao_resolution` switches at runtime:

| Value | Behavior | GPU markers |
|---|---|---|
| `legacy` | Original AO inside `combine_1`; no dedicated AO draws | `combine_1` |
| `full` (default) | Existing AO at full **internal render** resolution into R16F | `phase_ao` / `ao_evaluate_full` |
| `half` | One AO evaluation per 2x2 render-pixel block, then depth/normal-aware reconstruction into R16F | `phase_ao` / `ao_evaluate_half`, `ao_resolve` |

AO method and sample quality still use `r_ao_mode` and `r2_ssao`. These are shader permutations, so apply changes with `vid_restart` as before. With AO disabled at shader creation, the new targets and AO shaders are not allocated and no AO pass runs.

Full mode is a comparison baseline and integration step, **not a promised speedup**: it adds a texture write/read without reducing the sample count. Half mode provides the work reduction, at the cost of spatial detail. With DLSS/FSR, both sizes are relative to the already reduced internal render resolution, not display resolution.

Half mode chooses the nearest covered G-buffer pixel in each 2x2 block and stores visibility, view depth and the packed normal in RGBA16F. A four-tap resolve rejects mismatched depth/normals. If no compatible sample exists, it returns unoccluded visibility rather than borrowing foreground AO. Odd dimensions use rounded-up half targets and clamped integer coordinates. Full mode preserves the original interpolated UVs and jitter reconstruction, important for SSDO's noise hash.

The final AO target is fully overwritten each frame. There is no new temporal history, no copy-back, and no additional depth snapshot. The combine viewport is restored after AO. Target recreation follows the existing renderer reset path.

### How to check

1. Build the engine and deploy the changed and new shader files together. Ensure cached shaders are recompiled. Enable either AO method and a nonzero `r2_ssao`, then `vid_restart`.
2. Fixed camera/weather/settings: switch between `r_ao_resolution legacy` and `r_ao_resolution full`. Lighting should match apart from R16F visibility rounding. Full-mode time must include **both** `phase_ao` and `combine_1`.
3. Switch to `r_ao_resolution half`. Compare the sum of those passes and total frame time over multiple frames, not a single capture. Do not count `phase_ao` and its children twice.
4. Inspect grass, fence wires, building corners, foreground silhouettes and HUD weapons. Half mode can lose thin-surface AO or show bright gaps where no compatible background sample exists; use full mode when that tradeoff is unacceptable.
5. Check scope aim/unaim, TAA, SMAA, AA off, DLSS and FSR, including odd internal dimensions and resolution/quality changes. Look for viewport clipping, unstable contact shadows and shader resource binding warnings.
6. Disable AO and restart the renderer: no `phase_ao` marker; lighting should match the old AO-off path.

### Validation

`Benchmarks/validate_ao_shaders.ps1` compiles 33 shader variants with FXC: both methods, all quality levels including off, legacy/buffer composition, full/half evaluation, and reconstruction. This is compilation coverage, not an in-game performance measurement.

The full ReleaseTracyProfiler engine build and executable link passed (`bin_x64/xrEngine.exe`), with existing compiler warnings. A standalone D3D11 WARP smoke test passed 14 cases: full-resolution visibility matched the legacy calculation for SSDO and GTAO on even, odd and 1x1 targets, with and without jitter; half-resolution results were finite and sky pixels remained unoccluded; reconstruction rejected both depth and normal discontinuities. In-game screenshots and timings are still required before claiming a speedup or visual equivalence in actual scenes.

---

## Selectable XeGTAO

**Status:** implemented and working in-game; performance and visual comparison remain user-measured

**Files:** `r4_rendertarget_phase_xegtao.cpp`, AO routing/target lifecycle and console files, `dx10ShaderResourceStateCache.cpp`, `ogsr_xegtao*`, `shaders/r3/xegtao/`.

### What changed

`r_ao_mode st_xegtao` selects the MIT-licensed Intel implementation, pinned to upstream commit `a5b1686c7ea37788eeb3576b5be47f7c03db532c`. The default method remains SSDO. Existing SSDO/GTAO full/half/legacy modes are retained.

The XeGTAO path always uses **full internal render resolution**. `r_ao_resolution` applies only to SSDO/GTAO; `half` and `legacy` do not change XeGTAO's resolution or send it through legacy inline AO. The initialization log makes this explicit. With DLSS/FSR, internal resolution is already smaller than display resolution.

1. An adapted compute prefilter reads existing linear view depth and builds five R16F depth mips in one dispatch. It does not copy hardware depth or regenerate normals.
2. XeGTAO evaluates visibility using the G-buffer's octahedral normals. `r2_ssao st_opt_low`, `st_opt_medium`, and `st_opt_high` select 1x2, 2x2, and 3x3 slice/step presets respectively (samples are taken on both sides of each slice).
3. One upstream edge-aware denoise pass filters R8 visibility and packed R8 edges.
4. A full-screen export converts integer visibility into the existing R16F AO target. `combine_1_ao` retains the existing colored-AO and ambient-light treatment. Sky exports exactly 1.

The compute textures are padded to multiples of 16, with border replication during prefiltering. Projection constants compensate for padding and match OGSR's existing jittered view-space reconstruction. Math is FP32 on SM5; compact depth/AO storage does not require native FP16 arithmetic or a newer shader compiler. There is no separate AO temporal history: noise advances with TAA/DLSS/FSR, otherwise it stays fixed.

Resources and shaders are allocated only when XeGTAO and nonzero AO quality are active at target creation; they are released on renderer reset. The legacy half-size target is not allocated for XeGTAO. Direct compute uses explicit UAV/SRV transitions and clears/invalidate state before returning to graphics. The backend's reset now clears compute SRV cache entries and dirty ranges too, as it already did for other stages.

`r_xegtao_radius` adjusts the view-space radius live (default 0.5, range 0.05-4). Radius/art direction is not calibrated to match legacy GTAO, so compare quality and cost at acceptable visual settings, not just matching preset names.

### How to check

Build the engine and deploy all new shader files, **including the `xegtao` subdirectory**. Then:

```text
r_ao_mode st_xegtao
r2_ssao st_opt_medium
r_xegtao_radius 0.5
vid_restart
```

- Optional shader compilation coverage: run `Benchmarks/validate_xegtao_shaders.ps1` (15 SM5 variants).
- Compare a fixed scene against `st_ssdo` and `st_gtao`, restarting after each method/quality change. Include AO off as a reference. Test the existing optimized half-resolution mode separately; XeGTAO is not guaranteed to beat half-resolution AO.
- Capture `phase_ao` and its `ao_xegtao` children: `ao_xegtao_prefilter`, `ao_xegtao_evaluate`, `ao_xegtao_denoise`, `ao_xegtao_export`. Compare total frame time and `phase_ao + combine_1`; do not sum parents and their children. The outer scope also captures state-transition overhead.
- Check flat walls, corners, thin foliage/fences, depth discontinuities, screen borders, sky silhouettes, and HUD weapons/aimed scopes. Inspect noise and dark halos both stationary and in motion. Weapon projection/depth conventions are inherited from the G-buffer and need particular visual scrutiny.
- Check AA off/SMAA and TAA/DLSS/FSR, multiple internal sizes (including odd sizes), repeated `vid_restart`, and AO off. AO off must allocate no XeGTAO resources and emit no AO passes.
- Watch for shader errors, device/debug-layer binding warnings, invalid pixels, or changed unrelated rendering. Performance and image-quality claims should be based on these measurements.

### TODO

- Add half-resolution XeGTAO evaluation and an AO-aware resolve path.
- Add optional bent-normal output for directional ambient lighting.
- Add dedicated temporal accumulation/history if testing shows a benefit over the existing TAA/DLSS/FSR noise progression.
- Add dedicated weapon/HUD handling if G-buffer depth conventions produce visible mismatches.
- Benchmark XeGTAO against full- and half-resolution SSDO/GTAO and tune the default radius/quality presets.

Static checks passed for project/filter XML and validation-script PowerShell syntax. A 54-case arithmetic sanity check matched the padded XeGTAO reconstruction to the existing G-buffer formula at even, odd and tiny sizes, with and without jitter. These are not compilation or GPU execution tests.

---

## Template for the next entry

```
## Short name

**Status:** done | skipped
**Files:** …

### What it did

### What it can affect

### How to check
```
