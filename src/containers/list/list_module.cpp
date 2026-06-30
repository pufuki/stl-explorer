#include "containers/list_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"

namespace stl_explorer {
namespace containers {

using namespace ftxui;

ListModule::ListModule() {
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
    buildUI();
}

containers::ModuleInfo ListModule::info() const {
    return {
        .name = "std::list",
        .header_file = "<list>",
        .description = "Doubly linked list"
    };
}

void ListModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
}

void ListModule::resetWithInput(const std::string& input) {
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

void ListModule::buildUI() {
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

Element ListModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> elements;

    if (data_.empty()) {
        elements.push_back(text("NULL <-> (empty) <-> NULL") | dim);
    } else {
        elements.push_back(text("NULL") | dim);
        for (const auto& val : data_) {
            elements.push_back(text(" <-> ") | color(Color{0x55, 0xB5, 0xE8}));
            elements.push_back(text("[" + std::to_string(val) + "]") | borderLight);
        }
        elements.push_back(text(" <-> ") | color(Color{0x55, 0xB5, 0xE8}));
        elements.push_back(text("NULL") | dim);
    }

    std::string front_str = data_.empty() ? "N/A" : std::to_string(data_.front());
    std::string back_str = data_.empty() ? "N/A" : std::to_string(data_.back());

    return vbox({
        text("std::list") | bold | color(theme.colors().primary) | center,
        text("Doubly Linked List — non-contiguous memory") | dim | center,
        separator(),
        hbox(elements) | center,
        separator(),
        hbox({
            text("Front: " + front_str) | color(Color{0x4E, 0xCF, 0x8E}),
            text("  |  ") | dim,
            text("Back: " + back_str) | color(Color{0xFF, 0xAA, 0x00}),
        }) | center,
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("push/pop front & back: O(1) | Random access: O(n)") | dim | center,
        text("Each node stores data + pointers to prev & next") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: push_back(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): pop_back()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void ListModule::push_back(int value) {
    data_.push_back(value);
    current_operation_ = "push_back(" + std::to_string(value) + ")";
}

void ListModule::push_front(int value) {
    data_.push_front(value);
    current_operation_ = "push_front(" + std::to_string(value) + ")";
}

void ListModule::pop_back() {
    if (!data_.empty()) {
        int val = data_.back();
        data_.pop_back();
        current_operation_ = "pop_back() -> " + std::to_string(val);
    } else {
        current_operation_ = "pop_back() -> List is empty!";
    }
}

void ListModule::pop_front() {
    if (!data_.empty()) {
        int val = data_.front();
        data_.pop_front();
        current_operation_ = "pop_front() -> " + std::to_string(val);
    } else {
        current_operation_ = "pop_front() -> List is empty!";
    }
}

ftxui::Component ListModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
