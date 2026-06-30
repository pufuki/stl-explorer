#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <memory>

namespace stl_explorer {
namespace containers {

enum class ContainerType {
    Vector,
    Stack,
    Queue,
    Deque,
    List,
    Map,
    UnorderedMap,
    Set,
    PriorityQueue
};

struct ModuleInfo {
    std::string name;
    std::string header_file;
    std::string description;
};

struct ModuleVisualState {
    bool animation_active{false};
    size_t current_step{0};
    size_t total_steps{1};
    std::string current_operation;
};

class IContainerModule {
public:
    virtual ~IContainerModule() = default;

    virtual ftxui::Component getComponent() = 0;
    virtual std::string name() const = 0;
    virtual ModuleInfo info() const = 0;
    virtual void reset() = 0;
    virtual void resetWithInput(const std::string& input) { reset(); }
};

} // namespace containers
} // namespace stl_explorer
