#include "containers/unordered_map_module.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"

namespace stl_explorer {
namespace containers {

using namespace ftxui;

UnorderedMapModule::UnorderedMapModule() {
    for (int i = 0; i < 3; ++i) {
        data_[utils::randomInt(1, 50)] = utils::randomInt(100, 999);
    }
    buildUI();
}

containers::ModuleInfo UnorderedMapModule::info() const {
    return {
        .name = "std::unordered_map",
        .header_file = "<unordered_map>",
        .description = "Hash table based associative container"
    };
}

void UnorderedMapModule::reset() {
    data_.clear();
    current_operation_.clear();
    input_key_str_.clear();
    input_val_str_.clear();
    highlight_key_ = -1;
    for (int i = 0; i < 3; ++i) {
        data_[utils::randomInt(1, 50)] = utils::randomInt(100, 999);
    }
}

void UnorderedMapModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    data_.clear();
    current_operation_.clear();
    input_key_str_.clear();
    input_val_str_.clear();
    highlight_key_ = -1;
    for (size_t i = 0; i < vals.size(); i += 2) {
        int k = vals[i];
        int v = (i + 1 < vals.size()) ? vals[i + 1] : utils::randomInt(100, 999);
        data_[k] = v;
    }
}

void UnorderedMapModule::buildUI() {
    InputOption key_opt;
    key_opt.placeholder = "Key...";
    key_opt.on_enter = [this]() {
        int k = 0, v = 0;
        try { k = input_key_str_.empty() ? utils::randomInt(1, 50) : std::stoi(input_key_str_); }
        catch(...) { k = utils::randomInt(1, 50); }
        try { v = input_val_str_.empty() ? utils::randomInt(100, 999) : std::stoi(input_val_str_); }
        catch(...) { v = utils::randomInt(100, 999); }
        insert(k, v);
        input_key_str_.clear();
        input_val_str_.clear();
    };

    InputOption val_opt;
    val_opt.placeholder = "Value...";
    val_opt.on_enter = key_opt.on_enter;

    auto key_input = Input(&input_key_str_, key_opt);
    auto val_input = Input(&input_val_str_, val_opt);
    
    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();

    auto key_renderer = Renderer(key_input, [key_input]() {
        auto& theme = utils::Theme::instance();
        auto e = key_input->Render();
        if (key_input->Focused()) {
            return e | border | color(theme.colors().primary) | bold;
        }
        return e | border | color(theme.colors().muted);
    });
    auto val_renderer = Renderer(val_input, [val_input]() {
        auto& theme = utils::Theme::instance();
        auto e = val_input->Render();
        if (val_input->Focused()) {
            return e | border | color(theme.colors().primary) | bold;
        }
        return e | border | color(theme.colors().muted);
    });

    auto ins_btn = Button("insert(k,v)", [this]() {
        int k = 0, v = 0;
        try { k = input_key_str_.empty() ? utils::randomInt(1, 50) : std::stoi(input_key_str_); }
        catch(...) { k = utils::randomInt(1, 50); }
        try { v = input_val_str_.empty() ? utils::randomInt(100, 999) : std::stoi(input_val_str_); }
        catch(...) { v = utils::randomInt(100, 999); }
        insert(k, v);
        input_key_str_.clear();
        input_val_str_.clear();
    }, btn_opt);
    auto erase_btn = Button("erase(k)", [this]() {
        int k = 0;
        try { k = input_key_str_.empty() ? 0 : std::stoi(input_key_str_); }
        catch(...) { k = 0; }
        erase(k);
        input_key_str_.clear();
    }, btn_opt);
    auto find_btn = Button("find(k)", [this]() {
        int k = 0;
        try { k = input_key_str_.empty() ? 0 : std::stoi(input_key_str_); }
        catch(...) { k = 0; }
        find(k);
        input_key_str_.clear();
    }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({key_renderer, val_renderer, ins_btn, find_btn, erase_btn, back_btn});

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_key_str_.empty() && input_val_str_.empty()) {
            if (highlight_key_ != -1 && data_.count(highlight_key_)) {
                erase(highlight_key_);
            } else if (!data_.empty()) {
                erase(data_.begin()->first);
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

Element UnorderedMapModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> elements;
    for (const auto& [k, v] : data_) {
        bool hil = (highlight_key_ == k);
        Color c = hil ? Color{0x55, 0xB5, 0xE8} : theme.colors().muted;
        auto elem = text(" " + std::to_string(k) + ":" + std::to_string(v) + " ") | borderLight | color(c);
        if (hil) elem = elem | bold;
        elements.push_back(elem);
    }

    if (elements.empty()) elements.push_back(text("(empty)") | dim);

    return vbox({
        text("std::unordered_map") | bold | color(theme.colors().primary) | center,
        text("Hash Table Container") | dim | center,
        separator(),
        text("Key:Value pairs (unordered — hash-based):") | dim | center,
        hbox(elements) | center,
        separator(),
        text("Buckets: " + std::to_string(data_.bucket_count()) +
             " | Load factor: " + std::to_string(data_.load_factor()).substr(0, 4)) | dim | center,
        text("Size: " + std::to_string(data_.size())) | dim | center,
        text("Last: " + current_operation_) | color(theme.colors().warning) | center,
        separator(),
        text("insert/find/erase: O(1) average, O(n) worst case") | dim | center,
        text("Uses hash function — faster than map but unordered") | dim | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: insert(key, val) | Empty inputs = random values") | dim | center,
        text("Press Backspace (when inputs are empty): erase highlighted key or first key") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void UnorderedMapModule::insert(int key, int value) {
    data_[key] = value;
    highlight_key_ = key;
    current_operation_ = "insert(" + std::to_string(key) + ", " + std::to_string(value) + ")";
}

void UnorderedMapModule::erase(int key) {
    auto it = data_.find(key);
    if (it != data_.end()) {
        data_.erase(it);
        highlight_key_ = -1;
        current_operation_ = "erase(" + std::to_string(key) + ") — removed";
    } else {
        current_operation_ = "erase(" + std::to_string(key) + ") — key not found!";
    }
}

void UnorderedMapModule::find(int key) {
    auto it = data_.find(key);
    if (it != data_.end()) {
        highlight_key_ = key;
        current_operation_ = "find(" + std::to_string(key) + ") -> value=" + std::to_string(it->second);
    } else {
        highlight_key_ = -1;
        current_operation_ = "find(" + std::to_string(key) + ") -> NOT FOUND";
    }
}

ftxui::Component UnorderedMapModule::getComponent() {
    return component_;
}

} // namespace containers
} // namespace stl_explorer
