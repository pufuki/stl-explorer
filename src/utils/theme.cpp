#include "utils/theme.hpp"

namespace stl_explorer {
namespace utils {

Theme& Theme::instance() {
    static Theme instance;
    return instance;
}

void Theme::setTheme(ThemeType type) {
    current_theme_ = type;
    switch (type) {
        case ThemeType::Dark:
            applyDarkTheme();
            break;
        case ThemeType::Light:
            applyLightTheme();
            break;
        case ThemeType::Monochrome:
            applyMonochromeTheme();
            break;
    }
}

void Theme::applyDarkTheme() {
    colors_ = {
        .primary = ftxui::Color{0x55, 0xB5, 0xE8},
        .secondary = ftxui::Color{0x7A, 0xA2, 0xF7},
        .accent = ftxui::Color{0xFF, 0x61, 0x95},
        .background = ftxui::Color{0x1A, 0x1B, 0x26},
        .text = ftxui::Color{0xD5, 0xD6, 0xEB},
        .highlight = ftxui::Color{0xFF, 0xFF, 0xFF},
        .success = ftxui::Color{0x4E, 0xCF, 0x8E},
        .warning = ftxui::Color{0xFF, 0xAA, 0xFF},
        .error = ftxui::Color{0xF4, 0x43, 0x36},
        .muted = ftxui::Color{0x6C, 0x6E, 0x88}
    };
}

void Theme::applyLightTheme() {
    colors_ = {
        .primary = ftxui::Color{0x00, 0x6B, 0xA5},
        .secondary = ftxui::Color{0x5E, 0x3B, 0xBC},
        .accent = ftxui::Color{0xD3, 0x2F, 0x65},
        .background = ftxui::Color{0xFF, 0xFF, 0xFF},
        .text = ftxui::Color{0x1A, 0x1B, 0x26},
        .highlight = ftxui::Color{0xFF, 0xFF, 0xFF},
        .success = ftxui::Color{0x0D, 0x8A, 0x51},
        .warning = ftxui::Color{0xC1, 0x7E, 0x00},
        .error = ftxui::Color{0xB7, 0x1C, 0x1C},
        .muted = ftxui::Color{0x9E, 0x9E, 0x9E}
    };
}

void Theme::applyMonochromeTheme() {
    colors_ = {
        .primary = ftxui::Color::White,
        .secondary = ftxui::Color::GrayLight,
        .accent = ftxui::Color::White,
        .background = ftxui::Color::Black,
        .text = ftxui::Color::GrayLight,
        .highlight = ftxui::Color::White,
        .success = ftxui::Color::GrayLight,
        .warning = ftxui::Color::GrayLight,
        .error = ftxui::Color::GrayLight,
        .muted = ftxui::Color::GrayDark
    };
}

ftxui::Element Theme::styledText(const std::string& text, ftxui::Color color) const {
    return ftxui::text(text) | ftxui::color(color);
}

ftxui::Element Theme::header(const std::string& text) const {
    return ftxui::text(text)
        | ftxui::bold
        | ftxui::center
        | ftxui::color(colors_.primary);
}

ftxui::Element Theme::paragraph(const std::string& text) const {
    return ftxui::paragraph(text) | ftxui::color(colors_.text);
}

ftxui::Element Theme::separator() const {
    return ftxui::separator() | ftxui::color(colors_.muted);
}

ftxui::Element Theme::panel(ftxui::Element content, const std::string& title) const {
    if (title.empty()) {
        return ftxui::window(ftxui::text(""), std::move(content));
    }
    return ftxui::window(ftxui::text(title) | ftxui::color(colors_.primary),
           std::move(content));
}

ftxui::Decorator Theme::borderDecorator() const {
    return ftxui::borderLight;
}

ftxui::Decorator Theme::focusDecorator() const {
    return ftxui::focus;
}

ftxui::Decorator Theme::selectedDecorator() const {
    return ftxui::inverted;
}

ftxui::ButtonOption Theme::buttonOption() const {
    auto option = ftxui::ButtonOption::Animated();
    option.transform = [this](const ftxui::EntryState& state) {
        auto e = ftxui::text(state.label) | ftxui::center;
        if (state.focused) {
            e = e | ftxui::bgcolor(colors_.primary) | ftxui::color(colors_.background) | ftxui::bold;
        } else {
            e = e | ftxui::color(colors_.primary);
        }
        return e | ftxui::borderLight;
    };
    return option;
}

ftxui::MenuOption Theme::menuOption() const {
    auto option = ftxui::MenuOption::Vertical();
    option.entries_option.transform = [this](const ftxui::EntryState& state) {
        auto e = ftxui::text(state.label);
        if (state.focused) {
            e = e | ftxui::bgcolor(colors_.primary) | ftxui::color(colors_.background) | ftxui::bold;
        } else if (state.active) {
            e = e | ftxui::bgcolor(colors_.muted) | ftxui::color(colors_.highlight) | ftxui::bold;
        }
        return e;
    };
    return option;
}

} // namespace utils
} // namespace stl_explorer
