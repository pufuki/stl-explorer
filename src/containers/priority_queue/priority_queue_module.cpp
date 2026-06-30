#include "containers/priority_queue_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"
#include <algorithm>

namespace stl_explorer {
namespace containers {

using namespace ftxui;

PriorityQueueModule::PriorityQueueModule() {
    for (int i = 0; i < 5; ++i) {
        data_.push_back(utils::randomInt(1, 99));
        std::push_heap(data_.begin(), data_.end());
    }
    buildUI();
}

containers::ModuleInfo PriorityQueueModule::info() const {
    return {
        .name = "std::priority_queue",
        .header_file = "<queue>",
        .description = "Max-heap container adapter"
    };
}

void PriorityQueueModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int i = 0; i < 5; ++i) {
        data_.push_back(utils::randomInt(1, 99));
        std::push_heap(data_.begin(), data_.end());
    }
}

void PriorityQueueModule::resetWithInput(const std::string& input) {
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
        std::push_heap(data_.begin(), data_.end());
    }
}

void PriorityQueueModule::buildUI() {
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

Element PriorityQueueModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    // Tree-like visualization of the heap
    std::vector<Element> level_elements;
    if (!data_.empty()) {
        size_t idx = 0;
        int level = 0;
        while (idx < data_.size()) {
            int count = 1 << level; // 2^level nodes at this level
            std::vector<Element> row;
            for (int j = 0; j < count && idx < data_.size(); ++j, ++idx) {
                bool is_root = (idx == 0);
                Color c = is_root ? Color{0x4E, 0xCF, 0x8E} : theme.colors().muted;
                auto node = text(" " + std::to_string(data_[idx]) + " ") | borderLight | color(c);
                if (is_root) node = node | bold;
                row.push_back(node);
                if (j < count - 1 && idx + 1 < data_.size()) row.push_back(text("  ") | dim);
            }
            level_elements.push_back(hbox(row) | center);
            level++;
        }
    } else {
        level_elements.push_back(text("(empty)") | dim | center);
    }

    std::string top_str = data_.empty() ? "N/A" : std::to_string(data_[0]);

    return vbox({
        text("std::priority_queue") | bold | color(theme.colors().primary) | center,
        text("Max-Heap Container Adapter") | dim | center,
        separator(),
        text("Heap (tree levels — root=max):") | dim | center,
        vbox(level_elements) | center,
        separator(),
        text("Top (max): " + top_str) | bold | color(Color{0x4E, 0xCF, 0x8E}) | center,
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("push/pop: O(log n) | top: O(1) | Always max at top") | dim | center,
        text("Internally stored as array, structured as binary heap") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: push(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): pop()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void PriorityQueueModule::push(int value) {
    data_.push_back(value);
    std::push_heap(data_.begin(), data_.end());
    current_operation_ = "push(" + std::to_string(value) + ") — top is now " + std::to_string(data_[0]);
}

void PriorityQueueModule::pop() {
    if (!data_.empty()) {
        int val = data_[0];
        std::pop_heap(data_.begin(), data_.end());
        data_.pop_back();
        current_operation_ = "pop() -> " + std::to_string(val);
        if (!data_.empty()) {
            current_operation_ += " — new top: " + std::to_string(data_[0]);
        }
    } else {
        current_operation_ = "pop() -> Heap is empty!";
    }
}

ftxui::Component PriorityQueueModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
