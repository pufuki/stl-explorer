#pragma once

#include "animation.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <queue>

namespace stl_explorer {
namespace animation {

// Represents a single frame in an animation sequence
struct AnimationFrame {
    std::string description;
    std::function<void()> apply;
    std::chrono::milliseconds duration{300};
};

// Manages a sequence of animation frames
class AnimationSequence {
public:
    AnimationSequence();

    AnimationSequence& addFrame(AnimationFrame frame);
    AnimationSequence& addPause(std::chrono::milliseconds duration);
    AnimationSequence& setDescription(const std::string& description);

    void play();
    void pause();
    void stepForward();
    void stepBackward();
    void reset();
    void skipToEnd();

    bool hasNextFrame() const;
    bool hasPreviousFrame() const;
    size_t currentFrameIndex() const { return current_frame_; }
    size_t totalFrames() const { return frames_.size(); }
    bool isPlaying() const { return playing_; }
    bool isComplete() const { return current_frame_ >= frames_.size(); }
    const std::string& currentDescription() const;

private:
    std::vector<AnimationFrame> frames_;
    size_t current_frame_{0};
    bool playing_{false};
    std::string description_;
};

// Central animation controller
class AnimationEngine {
public:
    static AnimationEngine& instance();

    void setGlobalSpeed(double speed);
    double globalSpeed() const { return global_speed_; }

    void enqueue(std::unique_ptr<AnimationSequence> sequence);
    void playCurrent();
    void pauseCurrent();
    void stepNext();
    void stepPrevious();
    void stopAll();

    bool hasActiveSequence() const;
    AnimationSequence* currentSequence();

    void setOnFrameChanged(std::function<void(size_t, size_t)> callback);
    void setOnSequenceComplete(std::function<void()> callback);

private:
    AnimationEngine() = default;
    AnimationEngine(const AnimationEngine&) = delete;
    AnimationEngine& operator=(const AnimationEngine&) = delete;

    double global_speed_{1.0};
    std::queue<std::unique_ptr<AnimationSequence>> sequences_;
    AnimationSequence* current_sequence_{nullptr};
    std::function<void(size_t, size_t)> on_frame_changed_;
    std::function<void()> on_sequence_complete_;
};

} // namespace animation
} // namespace stl_explorer
