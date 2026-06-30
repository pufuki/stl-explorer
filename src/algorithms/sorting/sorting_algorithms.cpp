#include "algorithms/sorting/sorting_algorithms.hpp"
#include "utils/theme.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <sstream>

namespace stl_explorer {
namespace algorithms {

using namespace ftxui;

SortingModule::SortingModule() {
    randomizeData(10);
    buildUI();
}

containers::ModuleInfo SortingModule::info() const {
    return {
        .name = "Sorting Algorithms",
        .header_file = "<algorithm>",
        .description = "Visual comparison of sorting algorithms"
    };
}

void SortingModule::reset() {
    randomizeData(10);
    steps_.clear();
    current_step_ = 0;
    input_str_.clear();
}

void SortingModule::resetWithInput(const std::string& input) {
    auto vals = utils::parseCommaSeparated(input);
    if (vals.empty()) {
        reset();
        return;
    }
    data_ = vals;
    steps_.clear();
    current_step_ = 0;
    input_str_.clear();
}

void SortingModule::buildUI() {
    algo_names_ = {
        "Bubble Sort",
        "Selection Sort",
        "Insertion Sort",
        "Merge Sort",
        "Quick Sort",
        "Heap Sort"
    };

    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();
    auto menu_opt = theme.menuOption();

    auto algo_menu = Menu(&algo_names_, &selected_algorithm_, menu_opt);

    InputOption input_opt;
    input_opt.placeholder = "e.g. 5,3,8,1,9";
    input_opt.on_enter = [this]() {
        if (!input_str_.empty()) {
            data_.clear();
            std::istringstream ss(input_str_);
            std::string token;
            while (std::getline(ss, token, ',')) {
                try { data_.push_back(std::stoi(token)); }
                catch(...) {}
            }
            if (data_.empty()) randomizeData(10);
        } else {
            randomizeData(10);
        }
        steps_.clear();
        current_step_ = 0;
        input_str_.clear();
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

    auto set_btn = Button("Set Data", [this]() {
        if (!input_str_.empty()) {
            data_.clear();
            std::istringstream ss(input_str_);
            std::string token;
            while (std::getline(ss, token, ',')) {
                try { data_.push_back(std::stoi(token)); }
                catch(...) {}
            }
            if (data_.empty()) randomizeData(10);
        } else {
            randomizeData(10);
        }
        steps_.clear();
        current_step_ = 0;
        input_str_.clear();
    }, btn_opt);
    auto run_btn = Button("Run Sort", [this]() {
        generateSteps();
    }, btn_opt);
    auto prev_btn = Button("< Prev Step", [this]() { stepBackward(); }, btn_opt);
    auto next_btn = Button("Next Step >", [this]() { stepForward(); }, btn_opt);
    auto reset_btn = Button("Randomize", [this]() {
        randomizeData(10);
        steps_.clear();
        current_step_ = 0;
    }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({
        algo_menu,
        input_renderer,
        set_btn,
        run_btn,
        prev_btn,
        next_btn,
        reset_btn,
        back_btn
    });

    auto events_btns = CatchEvent(btns, [this](Event event) {
        if (event == Event::Backspace && input_str_.empty()) {
            randomizeData(10);
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

Element SortingModule::renderFullUI() const {
    auto& theme = utils::Theme::instance();

    // Bar chart visualization
    std::vector<Element> bars;
    int max_val = data_.empty() ? 1 : *std::max_element(data_.begin(), data_.end());
    if (max_val == 0) max_val = 1;

    for (size_t i = 0; i < data_.size(); ++i) {
        int val = data_[i];
        int height = (val * 8) / max_val + 1;

        Color bar_color = theme.colors().muted;

        if (!steps_.empty() && current_step_ < steps_.size()) {
            const auto& step = steps_[current_step_];
            if (step.highlight_a == static_cast<int>(i) || step.highlight_b == static_cast<int>(i)) {
                bar_color = Color{0xFF, 0xAA, 0x00}; // Comparing
            } else if (step.swap_index == static_cast<int>(i)) {
                bar_color = Color{0x66, 0xBB, 0x6A}; // Swapping
            } else if (step.pivot_index == static_cast<int>(i)) {
                bar_color = Color{0xAB, 0x47, 0xBC}; // Pivot
            }
        }

        std::vector<Element> bar_parts;
        bar_parts.push_back(text(std::to_string(val)) | dim | center);
        for (int h = 0; h < height; ++h) {
            bar_parts.push_back(text("██") | color(bar_color));
        }

        bars.push_back(vbox(bar_parts) | center);
        if (i < data_.size() - 1) bars.push_back(text(" "));
    }

    // Current data as array
    std::string arr_str = "[";
    for (size_t i = 0; i < data_.size(); ++i) {
        arr_str += std::to_string(data_[i]);
        if (i < data_.size() - 1) arr_str += ", ";
    }
    arr_str += "]";

    std::string step_desc = "Select algorithm and click 'Run Sort'";
    if (!steps_.empty() && current_step_ < steps_.size()) {
        step_desc = steps_[current_step_].description;
    }

    std::string step_counter = steps_.empty() ? "No steps yet" :
        "Step " + std::to_string(current_step_ + 1) + "/" + std::to_string(steps_.size());

    // Color legend
    auto legend = hbox({
        text("■ ") | color(Color{0xFF, 0xAA, 0x00}), text("Comparing  ") | dim,
        text("■ ") | color(Color{0x66, 0xBB, 0x6A}), text("Swapped  ") | dim,
        text("■ ") | color(Color{0xAB, 0x47, 0xBC}), text("Pivot") | dim,
    });

    return vbox({
        text("Sorting Algorithms") | bold | color(theme.colors().primary) | center,
        text("Algorithm: " + algo_names_[selected_algorithm_]) | dim | center,
        separator(),
        hbox(bars) | center,
        separator(),
        text(arr_str) | dim | center,
        separator(),
        text(step_counter) | dim | center,
        text(step_desc) | color(theme.colors().warning) | center,
        legend | center,
        separator(),
        text("─── Controls ───") | bold | center,
        text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center,
        text("Press Enter in input field: Set data | Empty input = randomize") | dim | center,
        text("Press Backspace (when input is empty): Randomize data") | dim | center,
        text("1. Select algorithm from menu (highlighted = active)") | dim | center,
        text("2. Type comma-separated values or press Enter for random") | dim | center,
        text("3. Click 'Run Sort', then step through with Prev/Next") | dim | center,
        text("ESC or q: Return to Main Menu") | dim | center,
    }) | bgcolor(theme.colors().background);
}

void SortingModule::selectAlgorithm(SortType type) {
    current_algorithm_ = type;
    resetAlgorithm();
    generateSteps();
}

void SortingModule::randomizeData(size_t size, int min, int max) {
    data_.clear();
    for (size_t i = 0; i < size; ++i) {
        data_.push_back(utils::randomInt(min, max));
    }
}

void SortingModule::generateSteps() {
    current_algorithm_ = static_cast<SortType>(selected_algorithm_);
    steps_.clear();
    current_step_ = 0;

    steps_.push_back({
        .data = data_,
        .description = "Initial array"
    });

    switch (current_algorithm_) {
        case SortType::Bubble: bubbleSortSteps(); break;
        case SortType::Selection: selectionSortSteps(); break;
        case SortType::Insertion: insertionSortSteps(); break;
        case SortType::Merge: mergeSortSteps(); break;
        case SortType::Quick: quickSortSteps(); break;
        case SortType::Heap: heapSortSteps(); break;
    }

    if (!steps_.empty()) {
        steps_.back().description = "Sorted!";
    }
}

void SortingModule::applyStep(size_t step_index) {
    if (step_index < steps_.size()) {
        data_ = steps_[step_index].data;
    }
}

void SortingModule::stepForward() {
    if (current_step_ + 1 < steps_.size()) {
        current_step_++;
        applyStep(current_step_);
    }
}

void SortingModule::stepBackward() {
    if (current_step_ > 0) {
        current_step_--;
        applyStep(current_step_);
    }
}

void SortingModule::resetAlgorithm() {
    current_step_ = 0;
    steps_.clear();
}

void SortingModule::bubbleSortSteps() {
    auto arr = data_;
    size_t n = arr.size();

    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n - i - 1; ++j) {
            steps_.push_back({
                .data = arr,
                .description = "Compare " + std::to_string(arr[j]) + " and " + std::to_string(arr[j+1]),
                .highlight_a = static_cast<int>(j),
                .highlight_b = static_cast<int>(j + 1)
            });

            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                steps_.push_back({
                    .data = arr,
                    .description = "Swapped " + std::to_string(arr[j]) + " and " + std::to_string(arr[j+1]),
                    .swap_index = static_cast<int>(j)
                });
            }
        }
    }
    data_ = arr;
}

void SortingModule::selectionSortSteps() {
    auto arr = data_;
    size_t n = arr.size();

    for (size_t i = 0; i < n - 1; ++i) {
        size_t min_idx = i;

        for (size_t j = i + 1; j < n; ++j) {
            steps_.push_back({
                .data = arr,
                .description = "Finding min: comparing [" + std::to_string(min_idx) + "]=" +
                    std::to_string(arr[min_idx]) + " with [" + std::to_string(j) + "]=" + std::to_string(arr[j]),
                .highlight_a = static_cast<int>(min_idx),
                .highlight_b = static_cast<int>(j)
            });

            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            std::swap(arr[i], arr[min_idx]);
            steps_.push_back({
                .data = arr,
                .description = "Swap min " + std::to_string(arr[i]) + " to position " + std::to_string(i),
                .swap_index = static_cast<int>(i)
            });
        }
    }
    data_ = arr;
}

void SortingModule::insertionSortSteps() {
    auto arr = data_;
    size_t n = arr.size();

    for (size_t i = 1; i < n; ++i) {
        int key = arr[i];
        int j = static_cast<int>(i) - 1;

        steps_.push_back({
            .data = arr,
            .description = "Inserting " + std::to_string(key) + " from position " + std::to_string(i),
            .pivot_index = static_cast<int>(i)
        });

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            steps_.push_back({
                .data = arr,
                .description = "Shift " + std::to_string(arr[j]) + " right",
                .highlight_a = j,
                .highlight_b = j + 1
            });
            j--;
        }
        arr[j + 1] = key;

        steps_.push_back({
            .data = arr,
            .description = "Placed " + std::to_string(key) + " at position " + std::to_string(j + 1),
            .swap_index = j + 1
        });
    }
    data_ = arr;
}

void SortingModule::mergeSortSteps() {
    auto arr = data_;
    if (arr.size() > 1) {
        mergeSortHelper(arr, 0, static_cast<int>(arr.size()) - 1);
    }
    data_ = arr;
}

void SortingModule::mergeSortHelper(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;

    steps_.push_back({
        .data = arr,
        .description = "Divide: [" + std::to_string(left) + ".." + std::to_string(mid) +
            "] and [" + std::to_string(mid+1) + ".." + std::to_string(right) + "]",
        .highlight_a = left,
        .highlight_b = right
    });

    mergeSortHelper(arr, left, mid);
    mergeSortHelper(arr, mid + 1, right);
    mergeArrays(arr, left, mid, right);
}

void SortingModule::mergeArrays(std::vector<int>& arr, int left, int mid, int right) {
    std::vector<int> temp(arr.begin() + left, arr.begin() + right + 1);
    int i = 0, j = mid - left + 1, k = left;

    while (i <= mid - left && j <= right - left) {
        steps_.push_back({
            .data = arr,
            .description = "Merge: compare " + std::to_string(temp[i]) + " and " + std::to_string(temp[j]),
            .highlight_a = k,
            .highlight_b = left + j
        });

        if (temp[i] <= temp[j]) {
            arr[k++] = temp[i++];
        } else {
            arr[k++] = temp[j++];
        }

        steps_.push_back({
            .data = arr,
            .description = "Placed " + std::to_string(arr[k-1]),
            .swap_index = k - 1
        });
    }

    while (i <= mid - left) {
        arr[k++] = temp[i++];
        steps_.push_back({
            .data = arr,
            .description = "Merge remaining left",
            .swap_index = k - 1
        });
    }
    while (j <= right - left) {
        arr[k++] = temp[j++];
        steps_.push_back({
            .data = arr,
            .description = "Merge remaining right",
            .swap_index = k - 1
        });
    }
}

void SortingModule::quickSortSteps() {
    auto arr = data_;
    if (arr.size() > 1) {
        quickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1);
    }
    data_ = arr;
}

void SortingModule::quickSortHelper(std::vector<int>& arr, int low, int high) {
    if (low >= high) return;

    int pivot = arr[high];
    steps_.push_back({
        .data = arr,
        .description = "Pivot = " + std::to_string(pivot) + " at index " + std::to_string(high),
        .pivot_index = high
    });

    int i = low - 1;
    for (int j = low; j < high; ++j) {
        steps_.push_back({
            .data = arr,
            .description = "Compare " + std::to_string(arr[j]) + " with pivot " + std::to_string(pivot),
            .highlight_a = j,
            .highlight_b = high
        });

        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
            if (i != j) {
                steps_.push_back({
                    .data = arr,
                    .description = "Swap " + std::to_string(arr[i]) + " and " + std::to_string(arr[j]),
                    .swap_index = i
                });
            }
        }
    }

    std::swap(arr[i + 1], arr[high]);
    steps_.push_back({
        .data = arr,
        .description = "Pivot " + std::to_string(arr[i+1]) + " placed at position " + std::to_string(i+1),
        .pivot_index = i + 1
    });

    quickSortHelper(arr, low, i);
    quickSortHelper(arr, i + 2, high);
}

void SortingModule::heapSortSteps() {
    auto arr = data_;
    int n = static_cast<int>(arr.size());

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        steps_.push_back({
            .data = arr,
            .description = "Build heap: heapify from index " + std::to_string(i)
        });
        heapify(arr, n, i);
    }

    steps_.push_back({
        .data = arr,
        .description = "Max heap built! Now extracting..."
    });

    // Extract elements
    for (int i = n - 1; i > 0; i--) {
        steps_.push_back({
            .data = arr,
            .description = "Swap max " + std::to_string(arr[0]) + " to position " + std::to_string(i),
            .highlight_a = 0,
            .highlight_b = i
        });

        std::swap(arr[0], arr[i]);

        steps_.push_back({
            .data = arr,
            .description = "After swap, heapify remaining " + std::to_string(i) + " elements",
            .swap_index = i
        });

        heapify(arr, i, 0);
    }
    data_ = arr;
}

void SortingModule::heapify(std::vector<int>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;

    if (largest != i) {
        steps_.push_back({
            .data = arr,
            .description = "Heapify: swap " + std::to_string(arr[i]) + " and " + std::to_string(arr[largest]),
            .highlight_a = i,
            .highlight_b = largest
        });

        std::swap(arr[i], arr[largest]);

        steps_.push_back({
            .data = arr,
            .description = "After heapify swap",
            .swap_index = largest
        });

        heapify(arr, n, largest);
    }
}

ftxui::Component SortingModule::getComponent() {
    return component_;
}

} // namespace algorithms
} // namespace stl_explorer
