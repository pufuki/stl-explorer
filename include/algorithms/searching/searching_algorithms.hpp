#pragma once

#include "containers/module_base.hpp"
#include <vector>
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>

namespace stl_explorer {
namespace algorithms {

struct SearchStep {
    int current_index{-1};
    int target{-1};
    std::string description;
    bool found{false};
    int low{-1};
    int high{-1};
};

class SearchModule : public containers::IContainerModule {
public:
    SearchModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Searching"; }
    containers::ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void randomizeData(size_t size = 15, int min = 1, int max = 99);
    void generateSteps();
    void stepForward();
    void stepBackward();

    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderFullUI() const;
    void linearSearchSteps();
    void binarySearchSteps();

    std::vector<int> data_;
    std::vector<SearchStep> steps_;
    size_t current_step_{0};
    int target_{0};
    int selected_algorithm_{0};
    ftxui::Component component_;
    std::vector<std::string> algo_names_;
    std::string input_str_;
    std::function<void()> back_callback_;
};

} // namespace algorithms
} // namespace stl_explorer
