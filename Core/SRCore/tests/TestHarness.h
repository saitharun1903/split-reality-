// Minimal dependency-free test harness (network access to fetch a framework is
// not guaranteed in build environments; this keeps the core self-contained).
#pragma once

#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace srtest {

struct TestCase {
    const char* name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& Registry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& FailureCount() {
    static int failures = 0;
    return failures;
}

struct Registrar {
    Registrar(const char* name, std::function<void()> fn) { Registry().push_back({name, fn}); }
};

inline void Fail(const char* file, int line, const std::string& what) {
    ++FailureCount();
    std::printf("    FAIL %s:%d: %s\n", file, line, what.c_str());
}

}  // namespace srtest

#define SR_CONCAT2(a, b) a##b
#define SR_CONCAT(a, b) SR_CONCAT2(a, b)

#define TEST(name)                                                              \
    static void name();                                                         \
    static srtest::Registrar SR_CONCAT(reg_, name)(#name, name);                \
    static void name()

#define CHECK(cond)                                                             \
    do {                                                                        \
        if (!(cond)) srtest::Fail(__FILE__, __LINE__, "CHECK(" #cond ")");      \
    } while (0)

#define CHECK_EQ(a, b)                                                          \
    do {                                                                        \
        if (!((a) == (b))) srtest::Fail(__FILE__, __LINE__, "CHECK_EQ(" #a ", " #b ")"); \
    } while (0)

#define CHECK_NEAR(a, b, eps)                                                   \
    do {                                                                        \
        if (std::fabs(static_cast<double>(a) - static_cast<double>(b)) > (eps)) \
            srtest::Fail(__FILE__, __LINE__,                                    \
                         "CHECK_NEAR(" #a ", " #b ") got " + std::to_string(a)); \
    } while (0)
