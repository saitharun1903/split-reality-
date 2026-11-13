#include "TestHarness.h"

int main() {
    int run = 0;
    for (const srtest::TestCase& t : srtest::Registry()) {
        const int before = srtest::FailureCount();
        t.fn();
        ++run;
        std::printf("%s %s\n", srtest::FailureCount() == before ? "[PASS]" : "[FAIL]", t.name);
    }
    std::printf("\n%d tests, %d failed checks\n", run, srtest::FailureCount());
    return srtest::FailureCount() == 0 ? 0 : 1;
}
