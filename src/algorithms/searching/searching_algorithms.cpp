#include "algorithms/searching/searching_algorithms.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"
#include <algorithm>

namespace stl_explorer {
namespace algorithms {

using namespace ftxui;

SearchModule::SearchModule() {
    randomizeData(15);
    buildUI();
}

containers::ModuleInfo SearchModule::info() const {
    return {
        .name = "Searching",
        .header_file = "<algorithm>",
        .description = "Search algorithms visualization"
    };
}

void SearchModule::reset() {
    randomizeData(15);
    steps_.clear();
    current_step_ = 0;
    input_str_.clear();
}

void SearchModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    data_ = vals;
    std::sort(data_.begin(), data_.end());
    steps_.clear();
    current_step_ = 0;
    input_str_.clear();
}

void SearchModule::buildUI() {
    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();
    auto menu_opt = theme.menuOption();

    algo_names_ = {"Linear Search", "Binary Search"};
    auto algo_menu = Menu(&algo_names_, &selected_algorithm_, menu_opt);

    InputOption input_opt;
    input_opt.placeholder = "Enter target value...";
    input_opt.on_enter = [this]() {
        generateSteps();
    };
    auto input = Input(&input_str_, input_opt);

    auto input_renderer = Renderer(input, [input]() {
        auto& theme = utils::Theme::instance();
        auto e = input->Render();
        if (input->Focused()) {
            return e | border | color(theme.colors().primary) | bold;
        }
        return e | border | color(theme.colors().muted);
    });

    auto search_btn = Button("Search!", [this]() { generateSteps(); }, btn_opt);
    auto prev_btn = Button("< Prev Step", [this]() { stepBackward(); }, btn_opt);
    auto next_btn = Button("Next Step >", [this]() { stepForward(); }, btn_opt);
    auto rand_btn = Button("New Data", [this]() {
        randomizeData(15);
        steps_.clear();
        current_step_ = 0;
    }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({
        algo_menu,
        input_renderer,
        search_btn,
        prev_btn,
        next_btn,
        rand_btn,
        back_btn
    });

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            randomizeData(15);
            steps_.clear();
            current_step_ = 0;
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
                text("Controls") | bold | color(theme.colors().primary) | center,
                separator(),
                events_btns->Render()
            }) | size(WIDTH, EQUAL, 25)
        }) | bgcolor(theme.colors().background);
    });
}

Element SearchModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> elements;
    int current_idx = -1;
    bool found = false;
    std::string desc = "Select algorithm, enter target, click Search!";
    int low_bound = -1, high_bound = -1;

    if (!steps_.empty() && current_step_ < steps_.size()) {
        const auto& step = steps_[current_step_];
        current_idx = step.current_index;
        found = step.found;
        desc = step.description;
        low_bound = step.low;
        high_bound = step.high;
    }

    for (size_t i = 0; i < data_.size(); ++i) {
        Color c = theme.colors().muted;

        // Dim out-of-range elements for binary search
        if (low_bound >= 0 && high_bound >= 0) {
            if (static_cast<int>(i) < low_bound || static_cast<int>(i) > high_bound) {
                auto elem = text(" " + std::to_string(data_[i]) + " ") | borderLight | dim;
                elements.push_back(elem);
                continue;
            }
        }

        if (found && static_cast<int>(i) == current_idx) {
            c = Color{0x4C, 0xAF, 0x50}; // Green = found
        } else if (static_cast<int>(i) == current_idx) {
            c = Color{0xFF, 0xAA, 0x00}; // Orange = checking
        }

        auto elem = text(" " + std::to_string(data_[i]) + " ") | borderLight | color(c);
        if (static_cast<int>(i) == current_idx) elem = elem | bold;
        elements.push_back(elem);
    }

    // Index labels
    std::vector<Element> idx_labels;
    for (size_t i = 0; i < data_.size(); ++i) {
        idx_labels.push_back(text(" " + std::to_string(i) + " ") | dim);
    }

    std::string step_counter = steps_.empty() ? "No steps" :
        "Step " + std::to_string(current_step_ + 1) + "/" + std::to_string(steps_.size());

    auto legend = hbox({
        text("■ ") | color(Color{0xFF, 0xAA, 0x00}), text("Checking  ") | dim,
        text("■ ") | color(Color{0x4C, 0xAF, 0x50}), text("Found!") | dim,
    });

    return vbox({
        text("Searching Algorithms") | bold | color(theme.colors().primary) | center,
        text("Algorithm: " + algo_names_[selected_algorithm_]) | dim | center,
        text("Target: " + std::to_string(target_)) | bold | center,
        separator(),
        hbox(elements) | center,
        hbox(idx_labels) | center,
        separator(),
        text(step_counter) | dim | center,
        text(desc) | color(found ? Color{0x4C, 0xAF, 0x50} : theme.colors().warning) | center,
        legend | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: Run search | Empty input = uses random target") | dim | center,
        text("Press Backspace (when input is empty): Generate new random data") | dim | center,
        text("1. Select Linear or Binary Search from menu (highlighted = active)") | dim | center,
        text("2. Type target value, then press Enter or click 'Search!'") | dim | center,
        text("3. Step through with Prev/Next buttons") | dim | center,
        text("Note: Binary Search requires sorted data (auto-sorted)") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void SearchModule::randomizeData(size_t size, int min, int max) {
    data_.clear();
    for (size_t i = 0; i < size; ++i) {
        data_.push_back(utils::randomInt(min, max));
    }
    std::sort(data_.begin(), data_.end());
    target_ = data_[utils::randomInt(0, static_cast<int>(data_.size()) - 1)];
}

void SearchModule::generateSteps() {
    steps_.clear();
    current_step_ = 0;

    // Parse target from input
    try {
        if (!input_str_.empty()) {
            target_ = std::stoi(input_str_);
        }
    } catch(...) {}

    if (selected_algorithm_ == 0) {
        linearSearchSteps();
    } else {
        binarySearchSteps();
    }
}

void SearchModule::linearSearchSteps() {
    for (size_t i = 0; i < data_.size(); ++i) {
        SearchStep step;
        step.current_index = static_cast<int>(i);
        step.target = target_;
        step.description = "Check index " + std::to_string(i) + ": " + std::to_string(data_[i]);

        if (data_[i] == target_) {
            step.found = true;
            step.description = "FOUND " + std::to_string(target_) + " at index " + std::to_string(i) + "!";
            steps_.push_back(step);
            return;
        }

        step.description += (data_[i] < target_ ? " < " : " > ") + std::to_string(target_) + " — continue";
        steps_.push_back(step);
    }

    SearchStep not_found;
    not_found.description = "NOT FOUND! " + std::to_string(target_) + " is not in the array.";
    steps_.push_back(not_found);
}

void SearchModule::binarySearchSteps() {
    int low = 0, high = static_cast<int>(data_.size()) - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        SearchStep step;
        step.current_index = mid;
        step.target = target_;
        step.low = low;
        step.high = high;
        step.description = "Range [" + std::to_string(low) + ".." + std::to_string(high) +
            "] mid=" + std::to_string(mid) + " value=" + std::to_string(data_[mid]);

        if (data_[mid] == target_) {
            step.found = true;
            step.description = "FOUND " + std::to_string(target_) + " at index " + std::to_string(mid) + "!";
            steps_.push_back(step);
            return;
        }

        if (data_[mid] < target_) {
            step.description += " < " + std::to_string(target_) + " — search RIGHT";
            steps_.push_back(step);
            low = mid + 1;
        } else {
            step.description += " > " + std::to_string(target_) + " — search LEFT";
            steps_.push_back(step);
            high = mid - 1;
        }
    }

    SearchStep not_found;
    not_found.description = "NOT FOUND! " + std::to_string(target_) + " is not in the array.";
    steps_.push_back(not_found);
}

void SearchModule::stepForward() {
    if (current_step_ + 1 < steps_.size()) current_step_++;
}

void SearchModule::stepBackward() {
    if (current_step_ > 0) current_step_--;
}

ftxui::Component SearchModule::getComponent() {
    return component_;
}

} // namespace algorithms
} // namespace stl_explorer
