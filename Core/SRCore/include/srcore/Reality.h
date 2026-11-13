// SPLIT REALITY — SRCore
// Engine-independent gameplay rules. No Unreal headers, no exceptions, no RTTI,
// so the same sources compile inside an Unreal module and in the CMake test build.
#pragma once

#include <cstdint>

namespace sr {

// The four realities of Asterion. Values are stable: they are used in
// save data, network messages and collision-channel mapping.
enum class Reality : uint8_t {
    Past = 0,
    Present = 1,
    Future = 2,
    Echo = 3,
};

constexpr int kRealityCount = 4;

constexpr Reality kAllRealities[kRealityCount] = {
    Reality::Past, Reality::Present, Reality::Future, Reality::Echo};

const char* ToString(Reality r);

// Fallback pairing used when fewer than four players are present.
// "Memory" realities (Past, Echo) pair together; "physical" realities
// (Present, Future) pair together. Chosen so that, with two players, the
// player who holds the clues is never the player who operates the machinery,
// which keeps communication mandatory. See Documentation/GAMEPLAY_DESIGN.md.
constexpr Reality Partner(Reality r) {
    switch (r) {
        case Reality::Past: return Reality::Echo;
        case Reality::Echo: return Reality::Past;
        case Reality::Present: return Reality::Future;
        case Reality::Future: return Reality::Present;
    }
    return r;
}

// Small value-type set of realities.
class RealityMask {
public:
    constexpr RealityMask() = default;
    constexpr explicit RealityMask(uint8_t bits) : bits_(bits & kAllBits) {}

    static constexpr RealityMask None() { return RealityMask(0); }
    static constexpr RealityMask All() { return RealityMask(kAllBits); }
    static constexpr RealityMask Of(Reality r) { return RealityMask(Bit(r)); }

    constexpr bool Contains(Reality r) const { return (bits_ & Bit(r)) != 0; }
    constexpr bool IsEmpty() const { return bits_ == 0; }
    int Count() const;

    constexpr RealityMask With(Reality r) const { return RealityMask(bits_ | Bit(r)); }
    constexpr RealityMask Without(Reality r) const {
        return RealityMask(static_cast<uint8_t>(bits_ & ~Bit(r)));
    }
    constexpr RealityMask operator|(RealityMask o) const { return RealityMask(bits_ | o.bits_); }
    constexpr RealityMask operator&(RealityMask o) const { return RealityMask(bits_ & o.bits_); }
    constexpr bool operator==(RealityMask o) const { return bits_ == o.bits_; }
    constexpr bool operator!=(RealityMask o) const { return bits_ != o.bits_; }

    constexpr uint8_t Bits() const { return bits_; }

private:
    static constexpr uint8_t kAllBits = 0x0F;
    static constexpr uint8_t Bit(Reality r) {
        return static_cast<uint8_t>(1u << static_cast<uint8_t>(r));
    }
    uint8_t bits_ = 0;
};

// Returns true if an object existing in `objectRealities` should be visible
// (and network-relevant) to a viewer currently perceiving `viewerReality`.
// This one predicate drives rendering visibility, collision and replication
// relevancy, so the three can never disagree.
constexpr bool IsPerceivable(RealityMask objectRealities, Reality viewerReality) {
    return objectRealities.Contains(viewerReality);
}

}  // namespace sr
