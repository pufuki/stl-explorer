#include "containers/deque_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"

namespace stl_explorer {
namespace containers {

using namespace ftxui;

DequeModule::DequeModule() {
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
    buildUI();
}

containers::ModuleInfo DequeModule::info() const {
    return {
        .name = "std::deque",
        .header_file = "<deque>",
        .description = "Double-ended queue"
    };
}

void DequeModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
}

void DequeModule::resetWithInput(const std::string& input) {
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

void DequeModule::buildUI() {
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

    auto pb_btn = Button("push_back()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push_back(val);
        input_str_.clear();
    }, btn_opt);
    auto pf_btn = Button("push_front()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        push_front(val);
        input_str_.clear();
    }, btn_opt);
    auto popb_btn = Button("pop_back()", [this]() { pop_back(); }, btn_opt);
    auto popf_btn = Button("pop_front()", [this]() { pop_front(); }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({input_renderer, pf_btn, pb_btn, popf_btn, popb_btn, back_btn});

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

Element DequeModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> elements;
    for (size_t i = 0; i < data_.size(); ++i) {
        bool is_front = (i == 0);
        bool is_back = (i == data_.size() - 1);
        Color c = is_front ? Color{0x4E, 0xCF, 0x8E} : (is_back ? Color{0xFF, 0xAA, 0x00} : theme.colors().muted);
        auto block = text(" " + std::to_string(data_[i]) + " ") | borderLight | color(c);
        if (is_front || is_back) block = block | bold;
        elements.push_back(block);
    }

    if (elements.empty()) elements.push_back(text("(empty)") | dim);

    std::string front_str = data_.empty() ? "N/A" : std::to_string(data_.front());
    std::string back_str = data_.empty() ? "N/A" : std::to_string(data_.back());

    return vbox({
        text("std::deque") | bold | color(theme.colors().primary) | center,
        text("Double-Ended Queue — insert/remove from both ends") | dim | center,
        separator(),
        hbox({
            text("FRONT-> ") | bold | color(Color{0x4E, 0xCF, 0x8E}),
            hbox(elements),
            text(" <-BACK") | bold | color(Color{0xFF, 0xAA, 0x00})
        }) | center,
        separator(),
        hbox({
            text("Front: " + front_str) | color(Color{0x4E, 0xCF, 0x8E}),
            text("  |  ") | dim,
            text("Back: " + back_str) | color(Color{0xFF, 0xAA, 0x00}),
        }) | center,
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("All push/pop operations are O(1). Random access O(1).") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: push_back(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): pop_back()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void DequeModule::push_back(int value) {
    data_.push_back(value);
    current_operation_ = "push_back(" + std::to_string(value) + ")";
}

void DequeModule::push_front(int value) {
    data_.push_front(value);
    current_operation_ = "push_front(" + std::to_string(value) + ")";
}

void DequeModule::pop_back() {
    if (!data_.empty()) {
        int val = data_.back();
        data_.pop_back();
        current_operation_ = "pop_back() -> " + std::to_string(val);
    } else {
        current_operation_ = "pop_back() -> Deque is empty!";
    }
}

void DequeModule::pop_front() {
    if (!data_.empty()) {
        int val = data_.front();
        data_.pop_front();
        current_operation_ = "pop_front() -> " + std::to_string(val);
    } else {
        current_operation_ = "pop_front() -> Deque is empty!";
    }
}

ftxui::Component DequeModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
