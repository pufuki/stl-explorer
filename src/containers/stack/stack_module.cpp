#include "containers/stack_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"
#include <sstream>

namespace stl_explorer {
namespace containers {

using namespace ftxui;

StackModule::StackModule() {
    // Pre-populate with random values
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
    buildUI();
}

containers::ModuleInfo StackModule::info() const {
    return {
        .name = "std::stack",
        .header_file = "<stack>",
        .description = "LIFO container adapter"
    };
}

void StackModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
}

void StackModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int v : vals) {
        data_.push_back(v);
    }
}

void StackModule::buildUI() {
    InputOption input_opt;
    input_opt.placeholder = "Enter value...";
    input_opt.on_enter = [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push(val);
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

    auto push_btn = Button("push()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push(val);
        input_str_.clear();
    }, btn_opt);
    auto pop_btn = Button("pop()", [this]() { pop(); }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({input_renderer, push_btn, pop_btn, back_btn});

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            pop();
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

Element StackModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    // Stack visualization (vertical, top at top)
    std::vector<Element> stack_elements;
    for (int i = static_cast<int>(data_.size()) - 1; i >= 0; --i) {
        bool is_top = (i == static_cast<int>(data_.size()) - 1);
        Color c = is_top ? Color{0x4E, 0xCF, 0x8E} : theme.colors().muted;

        std::string label = is_top ? " TOP -> " : "        ";
        auto block = hbox({
            text(label) | dim,
            text(" " + std::to_string(data_[i]) + " ") | borderLight | color(c) | (is_top ? bold : nothing),
        });

        stack_elements.push_back(block);
    }

    if (stack_elements.empty()) {
        stack_elements.push_back(text("(empty stack)") | dim | center);
    }

    std::string top_str = data_.empty() ? "N/A" : std::to_string(data_.back());

    return vbox({
        text("std::stack") | bold | color(theme.colors().primary) | center,
        text("LIFO — Last In, First Out") | dim | center,
        separator(),
        vbox(stack_elements) | center,
        separator(),
        text("Top: " + top_str) | bold | color(Color{0x4E, 0xCF, 0x8E}) | center,
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("push() adds to top, pop() removes from top — both O(1)") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: push(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): pop()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void StackModule::push(int value) {
    data_.push_back(value);
    current_operation_ = "push(" + std::to_string(value) + ")";
}

void StackModule::pop() {
    if (!data_.empty()) {
        int val = data_.back();
        data_.pop_back();
        current_operation_ = "pop() -> " + std::to_string(val);
    } else {
        current_operation_ = "pop() -> Stack is empty!";
    }
}

ftxui::Component StackModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
