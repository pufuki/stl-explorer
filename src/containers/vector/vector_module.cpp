#include "containers/vector_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"
#include <sstream>
#include <algorithm>

namespace stl_explorer {
namespace containers {

using namespace ftxui;

VectorModule::VectorModule() {
    state_.capacity = 4;
    state_.size = 0;
    // Pre-populate with random values
    for (int i = 0; i < 3; ++i) {
        int v = utils::randomInt(1, 99);
        state_.data.push_back(v);
        state_.size++;
    }
    if (state_.size > state_.capacity) state_.capacity = 8;
    buildUI();
}

ModuleInfo VectorModule::info() const {
    return {
        .name = "std::vector",
        .header_file = "<vector>",
        .description = "Dynamic contiguous array with automatic size management"
    };
}

void VectorModule::reset() {
    state_ = VectorState{};
    state_.capacity = 4;
    for (int i = 0; i < 3; ++i) {
        int v = utils::randomInt(1, 99);
        state_.data.push_back(v);
        state_.size++;
    }
    animation_steps_.clear();
    current_animation_step_ = 0;
    input_str_.clear();
}

void VectorModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    state_ = VectorState{};
    state_.capacity = 4;
    for (int v : vals) {
        state_.data.push_back(v);
        state_.size++;
    }
    while (state_.size > state_.capacity) state_.capacity *= 2;
    animation_steps_.clear();
    current_animation_step_ = 0;
    input_str_.clear();
}

void VectorModule::buildUI() {
    InputOption input_opt;
    input_opt.placeholder = "Enter value...";
    input_opt.on_enter = [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push_back(val);
        input_str_.clear();
    };

    auto input = Input(&input_str_, input_opt);
    
    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();

    auto input_renderer = Renderer(input, [input]() {
        auto& theme = utils::Theme::instance();
        auto e = input->Render();
        if (input->Focused()) {
            return e | border | color(theme.colors().primary) | bold;
        }
        return e | border | color(theme.colors().muted);
    });

    auto push_btn = Button("push_back()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push_back(val);
        input_str_.clear();
    }, btn_opt);
    auto pop_btn = Button("pop_back()", [this]() { pop_back(); }, btn_opt);
    auto clear_btn = Button("clear()", [this]() { clear(); }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto prev_btn = Button("< Prev", [this]() { stepAnimationBackward(); }, btn_opt);
    auto next_btn = Button("Next >", [this]() { stepAnimationForward(); }, btn_opt);

    auto btns = Container::Vertical({
        input_renderer,
        push_btn,
        pop_btn,
        clear_btn,
        prev_btn,
        next_btn,
        back_btn
    });

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            pop_back();
            return true;
        }
        return false;
    });

    component_ = Renderer(events_btns, [this, events_btns]() {
        auto& theme = utils::Theme::instance();
        return hbox({
            renderFullUI() | flex,
            separator(),
            vbox({
                text("Operations") | bold | color(theme.colors().primary) | center,
                separator(),
                events_btns->Render()
            }) | size(WIDTH, EQUAL, 25)
        }) | bgcolor(theme.colors().background);
    });
}

Element VectorModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    // Header
    auto header = hbox({
        text("std::vector") | bold | color(theme.colors().primary),
        filler(),
        text("<vector>") | dim
    });

    // Visualization
    std::vector<Element> blocks;
    for (size_t i = 0; i < state_.data.size(); ++i) {
        bool is_highlighted = (!animation_steps_.empty() &&
                              current_animation_step_ < animation_steps_.size() &&
                              animation_steps_[current_animation_step_].highlight_index == static_cast<int>(i));

        std::string val = std::to_string(state_.data[i]);
        if (val.length() < 4) val = std::string((4-val.length())/2, ' ') + val + std::string((4-val.length()+1)/2, ' ');

        auto block = text(val) | borderLight | (is_highlighted ? bold : nothing);
        blocks.push_back(block);
    }

    // Add capacity slots
    for (size_t i = state_.data.size(); i < state_.capacity && i < 16; ++i) {
        blocks.push_back(text("    ") | borderLight);
    }

    auto viz = blocks.empty() ? text("(empty)") | dim | center : hbox(blocks);

    // Index labels
    std::vector<Element> idx_labels;
    for (size_t i = 0; i < state_.data.size(); ++i) {
        std::string idx = "[" + std::to_string(i) + "]";
        if (idx.length() < 4) idx = std::string((4-idx.length())/2, ' ') + idx + std::string((4-idx.length()+1)/2, ' ');
        idx_labels.push_back(text(idx) | dim);
    }

    // Stats
    auto stats = hbox({
        text("Size: ") | dim,
        text(std::to_string(state_.size)) | color(theme.colors().success),
        text(" | Capacity: ") | dim,
        text(std::to_string(state_.capacity)) | color(theme.colors().warning)
    });

    // Step info
    std::string step_info = "Ready";
    if (!animation_steps_.empty() && current_animation_step_ < animation_steps_.size()) {
        step_info = "Step " + std::to_string(current_animation_step_ + 1) + "/" +
                   std::to_string(animation_steps_.size()) + ": " +
                   animation_steps_[current_animation_step_].description;
    }

    return vbox({
        header,
        separator(),
        text("Dynamic array — contiguous memory, auto-resizes") | dim | center,
        separator(),
        viz | center,
        idx_labels.empty() ? text("") : hbox(idx_labels) | center,
        separator(),
        stats | center,
        separator(),
        text(state_.explanation.empty() ? "Type a number and click push_back()" : state_.explanation) | dim | center,
        text(step_info) | color(theme.colors().accent) | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: push_back(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): pop_back()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void VectorModule::push_back(int value) {
    animation_steps_.clear();
    current_animation_step_ = 0;

    if (state_.size >= state_.capacity) {
        size_t old_cap = state_.capacity;
        state_.capacity = state_.capacity == 0 ? 1 : state_.capacity * 2;

        recordAnimationStep({
            .visual_data = state_.data,
            .size = state_.size,
            .capacity = old_cap,
            .description = "Capacity exceeded! Reallocating..."
        });

        recordAnimationStep({
            .visual_data = state_.data,
            .size = state_.size,
            .capacity = state_.capacity,
            .description = "New capacity: " + std::to_string(state_.capacity)
        });
    }

    state_.data.push_back(value);
    state_.size++;

    recordAnimationStep({
        .visual_data = state_.data,
        .size = state_.size,
        .capacity = state_.capacity,
        .description = "push_back(" + std::to_string(value) + ") complete",
        .highlight_index = static_cast<int>(state_.size - 1)
    });

    state_.current_operation = "push_back(" + std::to_string(value) + ")";
    state_.explanation = "Element appended at end in O(1) amortized time.";
}

void VectorModule::pop_back() {
    if (state_.data.empty()) {
        state_.explanation = "Cannot pop_back() from empty vector!";
        return;
    }

    animation_steps_.clear();
    current_animation_step_ = 0;

    int val = state_.data.back();
    state_.data.pop_back();
    state_.size--;

    recordAnimationStep({
        .visual_data = state_.data,
        .size = state_.size,
        .capacity = state_.capacity,
        .description = "pop_back() -> " + std::to_string(val)
    });

    state_.current_operation = "pop_back()";
    state_.explanation = "Removed last element (" + std::to_string(val) + ") in O(1). Capacity unchanged.";
}

void VectorModule::clear() {
    animation_steps_.clear();
    state_.data.clear();
    state_.size = 0;

    recordAnimationStep({
        .visual_data = state_.data,
        .size = 0,
        .capacity = state_.capacity,
        .description = "clear() complete"
    });

    state_.current_operation = "clear()";
    state_.explanation = "Removed all elements. Capacity unchanged at " + std::to_string(state_.capacity) + ".";
}

void VectorModule::recordAnimationStep(const AnimationStep& step) {
    animation_steps_.push_back(step);
}

void VectorModule::applyStep(size_t step_index) {
    if (step_index < animation_steps_.size()) {
        const auto& step = animation_steps_[step_index];
        state_.data = step.visual_data;
        state_.size = step.size;
        state_.capacity = step.capacity;
    }
}

void VectorModule::stepAnimationBackward() {
    if (current_animation_step_ > 0) {
        current_animation_step_--;
        applyStep(current_animation_step_);
    }
}

void VectorModule::stepAnimationForward() {
    if (current_animation_step_ + 1 < animation_steps_.size()) {
        current_animation_step_++;
        applyStep(current_animation_step_);
    }
}

void VectorModule::replayAnimation() {
    if (!animation_steps_.empty()) {
        current_animation_step_ = 0;
        applyStep(0);
    }
}

ftxui::Component VectorModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
