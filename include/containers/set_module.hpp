#pragma once

#include "containers/module_base.hpp"
#include <set>
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>

namespace stl_explorer {
namespace containers {

class SetModule : public IContainerModule {
public:
    SetModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Set"; }
    ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void insert(int value);
    void erase(int value);
    void find(int value);
    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderFullUI() const;

    std::set<int> data_;
    std::string current_operation_;
    int highlight_value_{-1};
    ftxui::Component component_;
    std::string input_str_;
    std::function<void()> back_callback_;
};

} // namespace containers
} // namespace stl_explorer
