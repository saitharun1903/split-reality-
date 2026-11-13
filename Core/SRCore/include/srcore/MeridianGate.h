// SPLIT REALITY — SRCore
// Content generator for the Chapter 1 cooperative puzzle, "The Meridian Gate".
// The solution is derived from a session seed so every session differs and
// solutions cannot be memorised from a video. Uses its own PRNG (SplitMix64)
// because std:: distributions are not identical across compilers/platforms.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "srcore/CoopPuzzle.h"

namespace sr {

class SplitMix64 {
public:
    explicit SplitMix64(uint64_t seed) : state_(seed) {}
    uint64_t Next();
    // Uniform in [0, bound). bound must be > 0.
    uint32_t NextBelow(uint32_t bound);

private:
    uint64_t state_;
};

// Everything each reality's content needs to present its share of the clue.
struct MeridianGateLayout {
    static constexpr size_t kGlyphCount = 6;     // glyphs carved in the Past archive
    static constexpr size_t kSequenceLength = 4; // glyphs the gate needs, in order

    // PAST sees: the dedication lists these glyphs, in this order.
    std::array<int, kSequenceLength> glyphSequence{};

    // ECHO sees: energy filaments linking glyph g to socket socketForGlyph[g].
    // PRESENT sees: six unlabelled sockets (labels burned away).
    std::array<int, kGlyphCount> socketForGlyph{};

    // FUTURE sees: the surge cycle. The "engage" lever is safe only during the
    // calm window. Runtime systems raise the "SurgeCalm" condition.
    float surgePeriodSeconds = 0.0f;
    float calmWindowSeconds = 0.0f;
};

constexpr const char* kMeridianSocketInput = "Socket";
constexpr const char* kMeridianEngageInput = "Engage";
constexpr const char* kMeridianCalmCondition = "SurgeCalm";

MeridianGateLayout GenerateMeridianGateLayout(uint64_t seed);

// Puzzle definition: four socket presses (Present) in glyph order, then the
// Engage lever (Present) while the SurgeCalm condition is raised.
PuzzleDefinition BuildMeridianGatePuzzle(const MeridianGateLayout& layout);

}  // namespace sr
