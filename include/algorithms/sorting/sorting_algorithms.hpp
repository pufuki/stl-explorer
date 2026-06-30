#pragma once

#include "containers/module_base.hpp"
#include <vector>
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>

namespace stl_explorer {
namespace algorithms {

struct SortStep {
    std::vector<int> data;
    std::string description;
    int highlight_a{-1};
    int highlight_b{-1};
    int swap_index{-1};
    int pivot_index{-1};
};

enum class SortType {
    Bubble = 0,
    Selection,
    Insertion,
    Merge,
    Quick,
    Heap
};

class SortingModule : public containers::IContainerModule {
public:
    SortingModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Sorting"; }
    containers::ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void randomizeData(size_t size = 10, int min = 1, int max = 99);
    void generateSteps();
    void stepForward();
    void stepBackward();
    void resetAlgorithm();
    void selectAlgorithm(SortType type);

    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderFullUI() const;
    void applyStep(size_t step_index);
    void bubbleSortSteps();
    void selectionSortSteps();
    void insertionSortSteps();
    void mergeSortSteps();
    void quickSortSteps();
    void heapSortSteps();
    void mergeSortHelper(std::vector<int>& arr, int left, int right);
    void mergeArrays(std::vector<int>& arr, int left, int mid, int right);
    void quickSortHelper(std::vector<int>& arr, int low, int high);
    void heapify(std::vector<int>& arr, int n, int i);

    std::vector<int> data_;
    std::vector<SortStep> steps_;
    size_t current_step_{0};
    int selected_algorithm_{0};
    SortType current_algorithm_{SortType::Bubble};
    ftxui::Component component_;
    std::vector<std::string> algo_names_;
    std::string input_str_;
    std::function<void()> back_callback_;
};

} // namespace algorithms
} // namespace stl_explorer
