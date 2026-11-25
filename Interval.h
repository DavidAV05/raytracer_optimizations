
#pragma once

#include <limits>

class Interval {
public:
    constexpr Interval() noexcept
        : min_(std::numeric_limits<double>::infinity()),
          max_(-std::numeric_limits<double>::infinity()) {}

    constexpr Interval(const double min, const double max) noexcept
        : min_(min), max_(max) {}

    [[nodiscard]] constexpr double min() const noexcept {
        return min_;
    }

    [[nodiscard]] constexpr double max() const noexcept {
        return max_;
    }

    [[nodiscard]] constexpr double size() const noexcept {
        return max_ - min_;
    }

    [[nodiscard]] constexpr bool contains(const double value) const noexcept {
        return min_ <= value && value <= max_;
    }

    [[nodiscard]] constexpr bool surrounds(const double value) const noexcept {
        return min_ < value && value < max_;
    }

    [[nodiscard]] double clamp(const double x) const {
        if (x < min_) return min_;
        if (x > max_) return max_;
        return x;
    }

    static const Interval empty;
    static const Interval universe;

private:
    double min_;
    double max_;
};


inline constexpr Interval Interval::empty{
    std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity()
};

inline constexpr Interval Interval::universe{
    -std::numeric_limits<double>::infinity(),
    std::numeric_limits<double>::infinity()
};
