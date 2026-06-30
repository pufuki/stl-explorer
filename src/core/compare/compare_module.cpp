#include "core/compare/compare_module.hpp"
#include "utils/theme.hpp"

namespace stl_explorer {
namespace compare {

using namespace ftxui;

CompareModule::CompareModule() {
    buildUI();
}

void CompareModule::buildUI() {
    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();
    auto menu_opt = theme.menuOption();

    names_ = {
        "Vector vs List",
        "Map vs Unordered Map",
        "Stack vs Queue"
    };

    auto menu = Menu(&names_, &selected_comparison_, menu_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({menu, back_btn});

    component_ = Renderer(btns, [this, btns]() {
        auto& theme = utils::Theme::instance();
        return hbox({
            vbox({
                text("Comparisons") | bold | color(theme.colors().primary) | center,
                separator(),
                btns->Render()
            }) | size(WIDTH, EQUAL, 25),
            separator(),
            renderComparison() | flex
        }) | bgcolor(theme.colors().background);
    });
}

Element CompareModule::renderComparison() const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> rows;
    std::string title;
    std::string recommendation;
    
    // Set up comparison columns depending on choice
    if (selected_comparison_ == 0) {
        title = "Vector vs List Comparison";
        recommendation = "Recommendation: Use vector for random access, list for frequent inserts/deletes at arbitrary places.";
        
        auto header = hbox({
            text("Metric") | bold | size(WIDTH, EQUAL, 20),
            text("std::vector") | bold | size(WIDTH, EQUAL, 20) | color(Color{0x55, 0xB5, 0xE8}),
            text("std::list") | bold | size(WIDTH, EQUAL, 20) | color(Color{0xFF, 0xAA, 0x00})
        });
        
        rows.push_back(text(title) | bold | color(theme.colors().primary) | center);
        rows.push_back(separator());
        rows.push_back(header);
        rows.push_back(separator());

        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> comparisons = {
            {"Access", {"O(1) Random Access", "O(n) Sequential Access"}},
            {"Insert/Delete End", {"O(1) Amortized", "O(1)"}},
            {"Insert/Delete Front", {"O(n) (Shifts element)", "O(1)"}},
            {"Insert/Delete Mid", {"O(n)", "O(1) (once node is found)"}},
            {"Memory Layout", {"Contiguous blocks", "Scattered (linked nodes)"}},
            {"Cache Friendly", {"Excellent (contiguous)", "Poor (cache misses)"}},
            {"Memory Overhead", {"Low (just capacity surplus)", "High (2 pointers per node)"}}
        };

        for (const auto& [metric, values] : comparisons) {
            rows.push_back(hbox({
                text(metric) | size(WIDTH, EQUAL, 20),
                text(values.first) | color(Color{0x55, 0xB5, 0xE8}) | size(WIDTH, EQUAL, 20),
                text(values.second) | color(Color{0xFF, 0xAA, 0x00}) | size(WIDTH, EQUAL, 20)
            }));
        }
    } else if (selected_comparison_ == 1) {
        title = "Map vs Unordered Map Comparison";
        recommendation = "Recommendation: Use unordered_map by default for faster O(1) lookups; use map if sorted order is needed.";
        
        auto header = hbox({
            text("Metric") | bold | size(WIDTH, EQUAL, 20),
            text("std::map") | bold | size(WIDTH, EQUAL, 20) | color(Color{0x55, 0xB5, 0xE8}),
            text("std::unordered_map") | bold | size(WIDTH, EQUAL, 20) | color(Color{0xFF, 0xAA, 0x00})
        });
        
        rows.push_back(text(title) | bold | color(theme.colors().primary) | center);
        rows.push_back(separator());
        rows.push_back(header);
        rows.push_back(separator());

        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> comparisons = {
            {"Ordering", {"Sorted by Key", "No guaranteed order"}},
            {"Implementation", {"Red-Black Tree (BST)", "Hash Table"}},
            {"Search Time", {"O(log n)", "O(1) Average / O(n) Worst"}},
            {"Insertion Time", {"O(log n)", "O(1) Average / O(n) Worst"}},
            {"Deletion Time", {"O(log n)", "O(1) Average / O(n) Worst"}},
            {"Key Requirements", {"operator< defined", "std::hash & operator=="}},
            {"Memory Overhead", {"3 pointers per node", "Buckets array + node links"}}
        };

        for (const auto& [metric, values] : comparisons) {
            rows.push_back(hbox({
                text(metric) | size(WIDTH, EQUAL, 20),
                text(values.first) | color(Color{0x55, 0xB5, 0xE8}) | size(WIDTH, EQUAL, 20),
                text(values.second) | color(Color{0xFF, 0xAA, 0x00}) | size(WIDTH, EQUAL, 20)
            }));
        }
    } else {
        title = "Stack vs Queue Comparison";
        recommendation = "Recommendation: Use stack for depth-first searches or undo operations; use queue for breadth-first searches or buffering.";
        
        auto header = hbox({
            text("Metric") | bold | size(WIDTH, EQUAL, 20),
            text("std::stack") | bold | size(WIDTH, EQUAL, 20) | color(Color{0x55, 0xB5, 0xE8}),
            text("std::queue") | bold | size(WIDTH, EQUAL, 20) | color(Color{0xFF, 0xAA, 0x00})
        });
        
        rows.push_back(text(title) | bold | color(theme.colors().primary) | center);
        rows.push_back(separator());
        rows.push_back(header);
        rows.push_back(separator());

        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> comparisons = {
            {"Access Policy", {"LIFO (Last In First Out)", "FIFO (First In First Out)"}},
            {"Insert Operation", {"push() at top [O(1)]", "push()/enqueue() at back [O(1)]"}},
            {"Remove Operation", {"pop() from top [O(1)]", "pop()/dequeue() from front [O(1)]"}},
            {"Inspect Element", {"top() [O(1)]", "front() & back() [O(1)]"}},
            {"Default Underlying", {"std::deque", "std::deque"}},
            {"Allowed Underlying", {"vector, deque, list", "deque, list"}},
            {"Iterator Support", {"No (adapter only)", "No (adapter only)"}}
        };

        for (const auto& [metric, values] : comparisons) {
            rows.push_back(hbox({
                text(metric) | size(WIDTH, EQUAL, 20),
                text(values.first) | color(Color{0x55, 0xB5, 0xE8}) | size(WIDTH, EQUAL, 20),
                text(values.second) | color(Color{0xFF, 0xAA, 0x00}) | size(WIDTH, EQUAL, 20)
            }));
        }
    }

    rows.push_back(separator());
    rows.push_back(text(recommendation) | dim | center);
    rows.push_back(separator());
    
    rows.push_back(text("─── Controls ───") | bold | center);
    rows.push_back(text("Tab / Up / Down Arrow: Switch comparison topics (bright blue hover)") | dim | center);
    rows.push_back(text("Enter: Select highlighted comparison topic") | dim | center);
    rows.push_back(text("Navigate to [Back] and press Enter to return") | dim | center);
    rows.push_back(text("ESC or q: Return to Main Menu") | dim | center);

    return vbox(rows) | bgcolor(theme.colors().background);
}

ftxui::Component CompareModule::getComponent() {
    return component_;
}

} // namespace compare
} // namespace stl_explorer
