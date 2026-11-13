#include "srcore/Reality.h"

namespace sr {

const char* ToString(Reality r) {
    switch (r) {
        case Reality::Past: return "Past";
        case Reality::Present: return "Present";
        case Reality::Future: return "Future";
        case Reality::Echo: return "Echo";
    }
    return "Unknown";
}

int RealityMask::Count() const {
    int n = 0;
    for (Reality r : kAllRealities) {
        if (Contains(r)) ++n;
    }
    return n;
}

}  // namespace sr
