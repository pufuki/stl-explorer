#pragma once

#include "containers/module_base.hpp"
#include <map>
#include <ftxui/component/component.hpp>
#include <string>
#include <functional>

namespace stl_explorer {
namespace containers {

class MapModule : public IContainerModule {
public:
    MapModule();

    ftxui::Component getComponent() override;
    std::string name() const override { return "Map"; }
    ModuleInfo info() const override;
    void reset() override;
    void resetWithInput(const std::string& input) override;

    void insert(int key, int value);
    void erase(int key);
    void find(int key);
    void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    ftxui::Element renderFullUI() const;

    std::map<int, int> data_;
    std::string current_operation_;
    int highlight_key_{-1};
    ftxui::Component component_;
    std::string input_key_str_;
    std::string input_val_str_;
    std::function<void()> back_callback_;
};

} // namespace containers
} // namespace stl_explorer
