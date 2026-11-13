#include "srcore/MeridianGate.h"

namespace sr {

uint64_t SplitMix64::Next() {
    uint64_t z = (state_ += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

uint32_t SplitMix64::NextBelow(uint32_t bound) {
    // Rejection sampling to avoid modulo bias.
    const uint64_t limit = UINT64_MAX - (UINT64_MAX % bound);
    uint64_t v;
    do {
        v = Next();
    } while (v >= limit);
    return static_cast<uint32_t>(v % bound);
}

namespace {

// Fisher–Yates shuffle of an index array using the deterministic PRNG.
template <size_t N>
void Shuffle(std::array<int, N>& values, SplitMix64& rng) {
    for (size_t i = N - 1; i > 0; --i) {
        const size_t j = rng.NextBelow(static_cast<uint32_t>(i + 1));
        const int t = values[i];
        values[i] = values[j];
        values[j] = t;
    }
}

}  // namespace

MeridianGateLayout GenerateMeridianGateLayout(uint64_t seed) {
    SplitMix64 rng(seed);
    MeridianGateLayout layout;
    constexpr size_t kN = MeridianGateLayout::kGlyphCount;

    // Shuffle glyph ids; the first kSequenceLength form the sequence.
    std::array<int, kN> glyphs{};
    for (size_t i = 0; i < kN; ++i) glyphs[i] = static_cast<int>(i);
    Shuffle(glyphs, rng);
    for (size_t i = 0; i < MeridianGateLayout::kSequenceLength; ++i) {
        layout.glyphSequence[i] = glyphs[i];
    }

    // Independent permutation: which socket each glyph's filament reaches.
    for (size_t i = 0; i < kN; ++i) layout.socketForGlyph[i] = static_cast<int>(i);
    Shuffle(layout.socketForGlyph, rng);

    // Surge cycle: 9.0–14.0 s period, 2.5–4.0 s calm window (0.5 s steps).
    layout.surgePeriodSeconds = 9.0f + 0.5f * static_cast<float>(rng.NextBelow(11));
    layout.calmWindowSeconds = 2.5f + 0.5f * static_cast<float>(rng.NextBelow(4));
    return layout;
}

PuzzleDefinition BuildMeridianGatePuzzle(const MeridianGateLayout& layout) {
    PuzzleDefinition def;
    def.id = "Ch1.MeridianGate";
    def.inputs.push_back({kMeridianSocketInput, Reality::Present});
    def.inputs.push_back({kMeridianEngageInput, Reality::Present});
    for (int glyph : layout.glyphSequence) {
        def.solution.push_back({kMeridianSocketInput, layout.socketForGlyph[static_cast<size_t>(glyph)], ""});
    }
    def.solution.push_back({kMeridianEngageInput, 1, kMeridianCalmCondition});
    def.maxMistakesBeforeLockout = 3;
    def.lockoutSeconds = 20.0f;
    return def;
}

}  // namespace sr
