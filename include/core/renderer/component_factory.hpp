#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <string>

namespace stl_explorer {
namespace renderer {

// Factory for creating common UI components with consistent styling
class ComponentFactory {
public:
    static ComponentFactory& instance();

    // Menu components
    ftxui::Component menuWithNavigation(
        const std::vector<std::string>& entries,
        int& selected,
        std::function<void()> on_enter = nullptr
    );

    // Button components
    ftxui::Component button(
        const std::string& label,
        std::function<void()> callback,
        bool& focusable
    );

    ftxui::Component backButton(std::function<void()> callback);
    ftxui::Component actionButton(const std::string& label, std::function<void()> callback);

    // Input components
    ftxui::Component integerInput(int& value, int min, int max);
    ftxui::Component stringInput(std::string& value);

    // Info panels
    ftxui::Element infoPanel(const std::string& title, ftxui::Element content);
    ftxui::Element statsPanel(const std::vector<std::pair<std::string, std::string>>& stats);
    ftxui::Element helpPanel(const std::vector<std::string>& shortcuts);

    // Layout helpers
    ftxui::Element centeredText(const std::string& text);
    ftxui::Element titledBox(const std::string& title, ftxui::Element content);
    ftxui::Element horizontalSeparator();
    ftxui::Element verticalLine();

    // Animations
    ftxui::Element animatedProgressBar(float progress, int width = 30);
    ftxui::Element loadingIndicator();

private:
    ComponentFactory() = default;
    ComponentFactory(const ComponentFactory&) = delete;
    ComponentFactory& operator=(const ComponentFactory&) = delete;
};

// Constants for UI dimensions
namespace layout {
    constexpr int kMenuWidth = 40;
    constexpr int kPanelWidth = 35;
    constexpr int kMaxContentWidth = 80;
    constexpr int kAnimationWidth = 60;
    constexpr int kMemoryBlockWidth = 6;
}

// Color palette for animated visualizations
namespace colors {
    const ftxui::Color kDefault{0x8B, 0x8B, 0x8B};
    const ftxui::Color kHighlight{0x55, 0xB5, 0xE8};
    const ftxui::Color kComparing{0xFF, 0xAA, 0x00};
    const ftxui::Color kSwapping{0x66, 0xBB, 0x6A};
    const ftxui::Color kSorted{0x4E, 0xCF, 0x8E};
    const ftxui::Color kError{0xF4, 0x43, 0x36};
    const ftxui::Color kSuccess{0x4C, 0xAF, 0x50};
    const ftxui::Color kPivot{0xAB, 0x47, 0xBC};
    const ftxui::Color kSelected{0xFF, 0x61, 0x95};
}

} // namespace renderer
} // namespace stl_explorer
