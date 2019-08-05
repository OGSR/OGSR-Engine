#pragma once

constexpr u32 MAX_PARTS = 4;

constexpr f32 SAMPLE_FPS = 30.f;
constexpr f32 SAMPLE_SPF = (1.f / SAMPLE_FPS);
constexpr f32 END_EPS = SAMPLE_SPF + EPS;
constexpr f32 KEY_Quant = 32767.f;
constexpr f32 KEY_QuantI = 1.f / KEY_Quant;
