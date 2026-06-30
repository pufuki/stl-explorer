#pragma once

#include <functional>
#include <chrono>
#include <memory>
#include <string>

namespace stl_explorer {
namespace animation {

enum class AnimationState {
    Pending,
    Running,
    Paused,
    Completed,
    Cancelled
};

enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic
};

class Animation {
public:
    using UpdateCallback = std::function<void(double progress)>;
    using CompleteCallback = std::function<void()>;

    Animation();
    ~Animation() = default;

    Animation& setDuration(std::chrono::milliseconds duration);
    Animation& setDelay(std::chrono::milliseconds delay);
    Animation& setEasing(EasingType type);
    Animation& onUpdate(UpdateCallback callback);
    Animation& onComplete(CompleteCallback callback);

    void start();
    void pause();
    void resume();
    void cancel();
    void reset();

    void update();

    AnimationState state() const { return state_; }
    double progress() const;
    bool isRunning() const { return state_ == AnimationState::Running; }
    bool isCompleted() const { return state_ == AnimationState::Completed; }

private:
    std::chrono::milliseconds duration_{300};
    std::chrono::milliseconds delay_{0};
    std::chrono::steady_clock::time_point start_time_;
    EasingType easing_{EasingType::EaseOut};
    AnimationState state_{AnimationState::Pending};
    UpdateCallback update_callback_;
    CompleteCallback complete_callback_;
    double current_progress_{0.0};
};

using AnimationPtr = std::unique_ptr<Animation>;

} // namespace animation
} // namespace stl_explorer
