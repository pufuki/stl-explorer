#pragma once

#include <string>
#include <ftxui/dom/elements.hpp>

namespace stl_explorer {
namespace complexity {

struct ComplexityInfo {
    std::string best{"O(1)"};
    std::string average{"O(1)"};
    std::string worst{"O(1)"};
    std::string space{"O(1)"};
    std::string operation{"N/A"};
    std::string explanation;
};

class ComplexityPanel {
public:
    ComplexityPanel();

    void setOperation(const std::string& operation);
    void setTimeComplexity(const std::string& best, const std::string& average, const std::string& worst);
    void setSpaceComplexity(const std::string& space);
    void setExplanation(const std::string& explanation);
    void setInfo(const ComplexityInfo& info);
    void reset();

    ftxui::Element render() const;
    ftxui::Element renderCompact() const;
    ftxui::Element renderInline() const;

private:
    ftxui::Element renderRow(const std::string& label, const std::string& value, ftxui::Color clr) const;

    ComplexityInfo info_;
};

// Predefined complexities for common operations
namespace operations {

ComplexityInfo vector_push_back();
ComplexityInfo vector_insert();
ComplexityInfo vector_erase();
ComplexityInfo vector_access();
ComplexityInfo map_insert();
ComplexityInfo map_find();
ComplexityInfo unordered_map_insert();
ComplexityInfo list_insert();
ComplexityInfo list_erase();
ComplexityInfo heap_push();
ComplexityInfo heap_pop();
ComplexityInfo bubble_sort_step();
ComplexityInfo quick_sort_step();
ComplexityInfo binary_search_step();
ComplexityInfo linear_search_step();

} // namespace operations

} // namespace complexity
} // namespace stl_explorer
