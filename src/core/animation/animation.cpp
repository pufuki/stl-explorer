#include "core/animation/animation.hpp"

namespace stl_explorer {
namespace animation {

Animation::Animation() = default;

Animation& Animation::setDuration(std::chrono::milliseconds duration) {
    duration_ = duration;
    return *this;
}

Animation& Animation::setDelay(std::chrono::milliseconds delay) {
    delay_ = delay;
    return *this;
}

Animation& Animation::setEasing(EasingType type) {
    easing_ = type;
    return *this;
}

Animation& Animation::onUpdate(UpdateCallback callback) {
    update_callback_ = std::move(callback);
    return *this;
}

Animation& Animation::onComplete(CompleteCallback callback) {
    complete_callback_ = std::move(callback);
    return *this;
}

void Animation::start() {
    state_ = AnimationState::Running;
    start_time_ = std::chrono::steady_clock::now();
}

void Animation::pause() {
    if (state_ == AnimationState::Running) {
        state_ = AnimationState::Paused;
    }
}

void Animation::resume() {
    if (state_ == AnimationState::Paused) {
        state_ = AnimationState::Running;
    }
}

void Animation::cancel() {
    state_ = AnimationState::Cancelled;
}

void Animation::reset() {
    state_ = AnimationState::Pending;
    current_progress_ = 0.0;
}

void Animation::update() {
    if (state_ != AnimationState::Running) return;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);

    if (elapsed < delay_) return;

    auto animation_elapsed = elapsed - delay_;

    if (animation_elapsed >= duration_) {
        current_progress_ = 1.0;
        state_ = AnimationState::Completed;
        if (update_callback_) update_callback_(1.0);
        if (complete_callback_) complete_callback_();
        return;
    }

    current_progress_ = static_cast<double>(animation_elapsed.count()) /
                         static_cast<double>(duration_.count());

    if (update_callback_) {
        update_callback_(current_progress_);
    }
}

double Animation::progress() const {
    return current_progress_;
}

} // namespace animation
} // namespace stl_explorer
