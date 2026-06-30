#pragma once

#include "containers/module_base.hpp"
#include <deque>
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>

namespace stl_explorer {
namespace containers {

class DequeModule : public IContainerModule {
public:
    DequeModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Deque"; }
    ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void push_back(int value);
    void push_front(int value);
    void pop_back();
    void pop_front();
    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderFullUI() const;

    std::deque<int> data_;
    std::string current_operation_;
    ftxui::Component component_;
    std::string input_str_;
    std::function<void()> back_callback_;
};

} // namespace containers
} // namespace stl_explorer
