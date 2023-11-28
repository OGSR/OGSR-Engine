#pragma once
#include <random>

class CRandom final
{
    static constexpr s32 maxI = 32767;

    std::mt19937 generator{std::random_device()()}; // mt19937 is a standard mersenne_twister_engine

    inline s32 nextValue(const s32 max) { return std::uniform_int_distribution<s32>{0, max - 1}(generator); }

public:
    CRandom() = default;

    [[nodiscard]] inline s32 randI() { return nextValue(maxI); }
    [[nodiscard]] inline s32 randI(const s32 max) { return !max ? max : nextValue(max); }
    [[nodiscard]] inline s32 randI(const s32 min, const s32 max) { return min + randI(max - min); }
    [[nodiscard]] inline s32 randIs(const s32 range) { return randI(-range, range); }
    [[nodiscard]] inline s32 randIs(const s32 range, const s32 offs) { return offs + randIs(range); }

    [[nodiscard]] inline float randF() { return static_cast<float>(randI()) / static_cast<float>(maxI - 1); }
    [[nodiscard]] inline float randF(const float max) { return randF() * max; }
    [[nodiscard]] inline float randF(const float min, const float max) { return min + randF(max - min); }
    [[nodiscard]] inline float randFs(const float range) { return randF(-range, range); }
    [[nodiscard]] inline float randFs(const float range, const float offs) { return offs + randFs(range); }
};

inline thread_local CRandom Random{};
