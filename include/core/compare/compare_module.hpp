#pragma once

#include <string>
#include <ftxui/component/component.hpp>
#include <functional>

namespace stl_explorer {
namespace compare {

class CompareModule {
public:
    CompareModule();

    ftxui::Component getComponent();

void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderComparison() const;

    int selected_comparison_{0};
    ftxui::Component component_;
    std::vector<std::string> names_;
    std::function<void()> back_callback_;
};

} // namespace compare
} // namespace stl_explorer
