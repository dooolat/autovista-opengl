#include "FpsCounter.h"

#include <cmath>

bool FpsCounter::update(double currentTimeSeconds) {
    if (!std::isfinite(currentTimeSeconds)) return false;

    if (!started_) {
        intervalStart_ = currentTimeSeconds;
        started_ = true;
    }

    ++frameCount_;
    const double elapsed = currentTimeSeconds - intervalStart_;
    if (elapsed < 0.5) return false;

    framesPerSecond_ = static_cast<int>(std::lround(frameCount_ / elapsed));
    frameCount_ = 0;
    intervalStart_ = currentTimeSeconds;
    return true;
}