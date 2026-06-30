#include "core/animation/animation_engine.hpp"

namespace stl_explorer {
namespace animation {

AnimationSequence::AnimationSequence() = default;

AnimationSequence& AnimationSequence::addFrame(AnimationFrame frame) {
    frames_.push_back(std::move(frame));
    return *this;
}

void AnimationSequence::play() {
    playing_ = true;
}

void AnimationSequence::pause() {
    playing_ = false;
}

void AnimationSequence::reset() {
    current_frame_ = 0;
    playing_ = false;
}

AnimationEngine& AnimationEngine::instance() {
    static AnimationEngine instance;
    return instance;
}

void AnimationEngine::setGlobalSpeed(double speed) {
    global_speed_ = speed;
}

} // namespace animation
} // namespace stl_explorer
