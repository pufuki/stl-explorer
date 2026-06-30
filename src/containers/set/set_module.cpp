#include "containers/set_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"

namespace stl_explorer {
namespace containers {

using namespace ftxui;

SetModule::SetModule() {
    for (int i = 0; i < 5; ++i) {
        data_.insert(utils::randomInt(1, 99));
    }
    buildUI();
}

containers::ModuleInfo SetModule::info() const {
    return {
        .name = "std::set",
        .header_file = "<set>",
        .description = "Sorted unique elements (Red-Black tree)"
    };
}

void SetModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    highlight_value_ = -1;
    for (int i = 0; i < 5; ++i) {
        data_.insert(utils::randomInt(1, 99));
    }
}

void SetModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    highlight_value_ = -1;
    for (int v : vals) {
        data_.insert(v);
    }
}

void SetModule::buildUI() {
    InputOption input_opt;
    input_opt.placeholder = "Enter value...";
    input_opt.on_enter = [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        insert(val);
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

    auto ins_btn = Button("insert()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        insert(val);
        input_str_.clear();
    }, btn_opt);
    auto find_btn = Button("find()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? 0 : std::stoi(input_str_); }
        catch(...) { val = 0; }
        find(val);
        input_str_.clear();
    }, btn_opt);
    auto erase_btn = Button("erase()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? 0 : std::stoi(input_str_); }
        catch(...) { val = 0; }
        erase(val);
        input_str_.clear();
    }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({input_renderer, ins_btn, find_btn, erase_btn, back_btn});

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            if (highlight_value_ != -1 && data_.count(highlight_value_)) {
                erase(highlight_value_);
            } else if (!data_.empty()) {
                erase(*data_.begin());
            }
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

Element SetModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> elements;
    for (const auto& val : data_) {
        bool hil = (highlight_value_ == val);
        Color c = hil ? Color{0x55, 0xB5, 0xE8} : theme.colors().muted;
        auto elem = text(" " + std::to_string(val) + " ") | borderLight | color(c);
        if (hil) elem = elem | bold;
        elements.push_back(elem);
    }

    if (elements.empty()) elements.push_back(text("(empty)") | dim);

    return vbox({
        text("std::set") | bold | color(theme.colors().primary) | center,
        text("Sorted Unique Elements (Red-Black Tree)") | dim | center,
        separator(),
        text("Elements (auto-sorted, duplicates rejected):") | dim | center,
        hbox({
            text("{ ") | dim,
            hbox(elements),
            text(" }") | dim
        }) | center,
        separator(),
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("insert/find/erase: O(log n) | Duplicates auto-rejected") | dim | center,
        text("Implemented as self-balancing BST (Red-Black tree)") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: insert(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): erase highlighted element or first element") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void SetModule::insert(int value) {
    auto [it, inserted] = data_.insert(value);
    highlight_value_ = value;
    current_operation_ = inserted
        ? "insert(" + std::to_string(value) + ") — added"
        : "insert(" + std::to_string(value) + ") — duplicate rejected!";
}

void SetModule::erase(int value) {
    auto it = data_.find(value);
    if (it != data_.end()) {
        data_.erase(it);
        highlight_value_ = -1;
        current_operation_ = "erase(" + std::to_string(value) + ") — removed";
    } else {
        current_operation_ = "erase(" + std::to_string(value) + ") — not found!";
    }
}

void SetModule::find(int value) {
    auto it = data_.find(value);
    if (it != data_.end()) {
        highlight_value_ = value;
        current_operation_ = "find(" + std::to_string(value) + ") — FOUND";
    } else {
        highlight_value_ = -1;
        current_operation_ = "find(" + std::to_string(value) + ") — NOT FOUND";
    }
}

ftxui::Component SetModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
