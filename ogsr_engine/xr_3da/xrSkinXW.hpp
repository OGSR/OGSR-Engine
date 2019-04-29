#pragma once

struct vertRender;
struct vertBoned1W;
struct vertBoned2W;
struct vertBoned3W;
struct vertBoned4W;
class CBoneInstance;

ENGINE_API void Skin1W(vertRender* D, vertBoned1W* S, u32 vCount, CBoneInstance* Bones);
ENGINE_API void Skin2W(vertRender* D, vertBoned2W* S, u32 vCount, CBoneInstance* Bones);
ENGINE_API void Skin3W(vertRender* D, vertBoned3W* S, u32 vCount, CBoneInstance* Bones);
ENGINE_API void Skin4W(vertRender* D, vertBoned4W* S, u32 vCount, CBoneInstance* Bones);
