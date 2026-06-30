#pragma once

#include "containers/module_base.hpp"
#include <vector>
#include <ftxui/component/component.hpp>
#include <functional>
#include <string>

namespace stl_explorer {
namespace containers {

struct VectorState {
    std::vector<int> data;
    size_t size{0};
    size_t capacity{0};
    std::string current_operation;
    std::string explanation;
};

struct AnimationStep {
    std::vector<int> visual_data;
    size_t size;
    size_t capacity;
    std::string description;
    int highlight_index{-1};
};

class VectorModule : public IContainerModule {
public:
    VectorModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Vector"; }
    ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void push_back(int value);
    void pop_back();
    void clear();
    void stepAnimationBackward();
    void stepAnimationForward();
    void replayAnimation();
    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    void recordAnimationStep(const AnimationStep& step);
    void applyStep(size_t step_index);
    ftxui::Element renderFullUI() const;

    VectorState state_;
    std::vector<AnimationStep> animation_steps_;
    size_t current_animation_step_{0};
    ftxui::Component component_;
    std::string input_str_;
    std::function<void()> back_callback_;
};

} // namespace containers
} // namespace stl_explorer
