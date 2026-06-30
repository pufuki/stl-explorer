#include "containers/queue_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"

namespace stl_explorer {
namespace containers {

using namespace ftxui;

QueueModule::QueueModule() {
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
    buildUI();
}

containers::ModuleInfo QueueModule::info() const {
    return {
        .name = "std::queue",
        .header_file = "<queue>",
        .description = "FIFO container adapter"
    };
}

void QueueModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_str_.clear();
    for (int i = 0; i < 3; ++i) {
        data_.push_back(utils::randomInt(1, 99));
    }
}

void QueueModule::resetWithInput(const std::string& input) {
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

void QueueModule::buildUI() {
    InputOption input_opt;
    input_opt.placeholder = "Enter value...";
    input_opt.on_enter = [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        enqueue(val);
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

    auto enq_btn = Button("enqueue()", [this]() {
        int val = 0;
        try { val = input_str_.empty() ? utils::randomInt(1, 99) : std::stoi(input_str_); }
        catch(...) { val = utils::randomInt(1, 99); }
        enqueue(val);
        input_str_.clear();
    }, btn_opt);
    auto deq_btn = Button("dequeue()", [this]() { dequeue(); }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({input_renderer, enq_btn, deq_btn, back_btn});

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            dequeue();
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

Element QueueModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    // Queue visualization (horizontal)
    std::vector<Element> queue_elements;
    for (size_t i = 0; i < data_.size(); ++i) {
        bool is_front = (i == 0);
        bool is_back = (i == data_.size() - 1);
        Color c = is_front ? Color{0x4E, 0xCF, 0x8E} : (is_back ? Color{0xFF, 0xAA, 0x00} : theme.colors().muted);

        auto block = text(" " + std::to_string(data_[i]) + " ") | borderLight | color(c);
        if (is_front || is_back) block = block | bold;
        queue_elements.push_back(block);
        if (i < data_.size() - 1) queue_elements.push_back(text(" -> ") | dim);
    }

    if (queue_elements.empty()) {
        queue_elements.push_back(text("(empty)") | dim);
    }

    std::string front_str = data_.empty() ? "N/A" : std::to_string(data_.front());
    std::string back_str = data_.empty() ? "N/A" : std::to_string(data_.back());

    return vbox({
        text("std::queue") | bold | color(theme.colors().primary) | center,
        text("FIFO — First In, First Out") | dim | center,
        separator(),
        hbox({
            text("FRONT-> ") | bold | color(Color{0x4E, 0xCF, 0x8E}),
            hbox(queue_elements),
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
        text("enqueue() adds to back, dequeue() removes from front — both O(1)") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: enqueue(val) | Empty input = random value (1-99)") | dim | center,
        text("Press Backspace (when input is empty): dequeue()") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void QueueModule::enqueue(int value) {
    data_.push_back(value);
    current_operation_ = "enqueue(" + std::to_string(value) + ")";
}

void QueueModule::dequeue() {
    if (!data_.empty()) {
        int val = data_.front();
        data_.pop_front();
        current_operation_ = "dequeue() -> " + std::to_string(val);
    } else {
        current_operation_ = "dequeue() -> Queue is empty!";
    }
}

ftxui::Component QueueModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
