#pragma once

class FpsCounter {
public:
    bool update(double currentTimeSeconds);
    int framesPerSecond() const noexcept { return framesPerSecond_; }

private:
    double intervalStart_ = 0.0;
    int frameCount_ = 0;
    int framesPerSecond_ = 0;
    bool started_ = false;
};