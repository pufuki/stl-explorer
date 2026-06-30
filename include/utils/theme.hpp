#pragma once

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <unordered_map>
#include <functional>

namespace stl_explorer {
namespace utils {

enum class ThemeType {
    Dark,
    Light,
    Monochrome
};

struct ColorScheme {
    ftxui::Color primary;
    ftxui::Color secondary;
    ftxui::Color accent;
    ftxui::Color background;
    ftxui::Color text;
    ftxui::Color highlight;
    ftxui::Color success;
    ftxui::Color warning;
    ftxui::Color error;
    ftxui::Color muted;
};

class Theme {
public:
    static Theme& instance();

    void setTheme(ThemeType type);
    ThemeType currentTheme() const { return current_theme_; }
    const ColorScheme& colors() const { return colors_; }

    // Convenience methods for common styling
    ftxui::Element styledText(const std::string& text, ftxui::Color color) const;
    ftxui::Element header(const std::string& text) const;
    ftxui::Element paragraph(const std::string& text) const;
    ftxui::Element separator() const;
    ftxui::Element panel(ftxui::Element content, const std::string& title = "") const;

    // Box decorators
    ftxui::Decorator borderDecorator() const;
    ftxui::Decorator focusDecorator() const;
    ftxui::Decorator selectedDecorator() const;

    ftxui::ButtonOption buttonOption() const;
    ftxui::MenuOption menuOption() const;

private:
    Theme() = default;
    Theme(const Theme&) = delete;
    Theme& operator=(const Theme&) = delete;

    void applyDarkTheme();
    void applyLightTheme();
    void applyMonochromeTheme();

    ThemeType current_theme_{ThemeType::Dark};
    ColorScheme colors_{};
};

} // namespace utils
} // namespace stl_explorer
