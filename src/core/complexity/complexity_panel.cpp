#include "core/complexity/complexity_panel.hpp"
#include "utils/theme.hpp"

namespace stl_explorer {
namespace complexity {

using namespace ftxui;

ComplexityPanel::ComplexityPanel() = default;

void ComplexityPanel::setOperation(const std::string& operation) {
    info_.operation = operation;
}

void ComplexityPanel::setTimeComplexity(const std::string& best,
                                         const std::string& average,
                                         const std::string& worst) {
    info_.best = best;
    info_.average = average;
    info_.worst = worst;
}

void ComplexityPanel::setSpaceComplexity(const std::string& space) {
    info_.space = space;
}

void ComplexityPanel::setExplanation(const std::string& explanation) {
    info_.explanation = explanation;
}

void ComplexityPanel::setInfo(const ComplexityInfo& info) {
    info_ = info;
}

void ComplexityPanel::reset() {
    info_ = ComplexityInfo{};
}

Element ComplexityPanel::render() const {
    auto& theme = utils::Theme::instance();

    return vbox({
        text("COMPLEXITY ANALYSIS") | bold | color(theme.colors().primary) | center,
        separator(),
        renderRow("Operation:", info_.operation, theme.colors().text),
        separator(),
        renderRow("Best:", info_.best, theme.colors().success),
        renderRow("Average:", info_.average, theme.colors().warning),
        renderRow("Worst:", info_.worst, theme.colors().error),
        separator(),
        renderRow("Space:", info_.space, theme.colors().secondary),
        separator(),
        vbox({
            text("Explanation:") | bold,
            paragraph(info_.explanation) | dim | size(WIDTH, LESS_THAN, 35)
        })
    });
}

Element ComplexityPanel::renderCompact() const {
    auto& theme = utils::Theme::instance();

    return vbox({
        text(info_.operation) | bold | color(theme.colors().primary),
        hbox({
            text("Best: ") | dim,
            text(info_.best) | color(theme.colors().success),
            text("  "),
            text("Worst: ") | dim,
            text(info_.worst) | color(theme.colors().error)
        })
    });
}

Element ComplexityPanel::renderInline() const {
    return hbox({
        text(info_.operation) | bold,
        text(" "),
        text("[") | dim,
        text(info_.worst) | color(ftxui::Color{0x55, 0xB5, 0xE8}),
        text("]") | dim
    });
}

Element ComplexityPanel::renderRow(const std::string& label,
                                    const std::string& value,
                                    Color clr) const {
    Elements elems;
    elems.push_back(text(label) | bold | size(WIDTH, EQUAL, 12));
    elems.push_back(text(value) | color(clr));
    return hbox(std::move(elems));
}

// Predefined operations
namespace operations {

ComplexityInfo vector_push_back() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(n)",
        .space = "O(1)",
        .operation = "push_back()",
        .explanation = "Amortized O(1). When capacity is exceeded, reallocation copies all elements (O(n))."
    };
}

ComplexityInfo vector_insert() {
    return {
        .best = "O(1)",
        .average = "O(n)",
        .worst = "O(n)",
        .space = "O(1)",
        .operation = "insert()",
        .explanation = "Elements after insertion point must shift. O(n) for middle insertions."
    };
}

ComplexityInfo vector_erase() {
    return {
        .best = "O(1)",
        .average = "O(n)",
        .worst = "O(n)",
        .space = "O(1)",
        .operation = "erase()",
        .explanation = "Elements after erased position shift left. O(n) for middle deletions."
    };
}

ComplexityInfo vector_access() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(1)",
        .space = "O(1)",
        .operation = "operator[] / at()",
        .explanation = "Direct pointer arithmetic access. Constant time regardless of size."
    };
}

ComplexityInfo map_insert() {
    return {
        .best = "O(log n)",
        .average = "O(log n)",
        .worst = "O(log n)",
        .space = "O(1)",
        .operation = "insert()",
        .explanation = "Red-Black tree maintains balance. Height is always log n."
    };
}

ComplexityInfo map_find() {
    return {
        .best = "O(log n)",
        .average = "O(log n)",
        .worst = "O(log n)",
        .space = "O(1)",
        .operation = "find()",
        .explanation = "Binary search traversal on balanced tree."
    };
}

ComplexityInfo unordered_map_insert() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(n)",
        .space = "O(1)",
        .operation = "insert()",
        .explanation = "Hash collision causes worst case. Rehashing can be expensive."
    };
}

ComplexityInfo list_insert() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(1)",
        .space = "O(1)",
        .operation = "insert()",
        .explanation = "Only updates adjacent node pointers. No shifting needed."
    };
}

ComplexityInfo list_erase() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(1)",
        .space = "O(1)",
        .operation = "erase()",
        .explanation = "Adjusts two pointers and deallocates. Constant time."
    };
}

ComplexityInfo heap_push() {
    return {
        .best = "O(log n)",
        .average = "O(log n)",
        .worst = "O(log n)",
        .space = "O(1)",
        .operation = "push()",
        .explanation = "Percolates up the heap. Maximum height is log n."
    };
}

ComplexityInfo heap_pop() {
    return {
        .best = "O(log n)",
        .average = "O(log n)",
        .worst = "O(log n)",
        .space = "O(1)",
        .operation = "pop()",
        .explanation = "Moves last element to root and percolates down."
    };
}

ComplexityInfo bubble_sort_step() {
    return {
        .best = "O(1)",
        .average = "O(1)",
        .worst = "O(1)",
        .space = "O(1)",
        .operation = "Compare & Swap",
        .explanation = "Each step compares adjacent elements. O(n^2) total for complete sort."
    };
}

ComplexityInfo quick_sort_step() {
    return {
        .best = "O(log n)",
        .average = "O(n log n)",
        .worst = "O(n^2)",
        .space = "O(log n)",
        .operation = "Partition",
        .explanation = "Divides around pivot. Worst case: already sorted with bad pivot."
    };
}

ComplexityInfo binary_search_step() {
    return {
        .best = "O(1)",
        .average = "O(log n)",
        .worst = "O(log n)",
        .space = "O(1)",
        .operation = "Binary Search",
        .explanation = "Halves search space each iteration. Best: first guess hits."
    };
}

ComplexityInfo linear_search_step() {
    return {
        .best = "O(1)",
        .average = "O(n)",
        .worst = "O(n)",
        .space = "O(1)",
        .operation = "Linear Search",
        .explanation = "Scans elements sequentially. Best: element is first."
    };
}

} // namespace operations

} // namespace complexity
} // namespace stl_explorer
