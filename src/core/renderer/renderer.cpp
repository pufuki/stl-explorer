#include "core/renderer/renderer.hpp"
#include "utils/theme.hpp"

namespace stl_explorer {
namespace renderer {

using namespace ftxui;

Renderer& Renderer::instance() {
    static Renderer instance;
    return instance;
}

void Renderer::setMainComponent(Component component) {
    main_component_ = std::move(component);
}

void Renderer::setMainElement(Element element) {
    main_element_ = std::move(element);
}

Component Renderer::component() {
    return main_component_;
}

Element Renderer::renderHeader() const {
    auto& theme = utils::Theme::instance();

    return vbox({
        text("========================================") | color(theme.colors().primary) | center,
        text("            STL Explorer              ") | bold | color(theme.colors().highlight) | center,
        text("        Learn C++ STL Visually       ") | dim | center,
        text("========================================") | color(theme.colors().primary) | center
    });
}

Element Renderer::renderFooter(const std::vector<std::string>& hints) const {
    auto& theme = utils::Theme::instance();

    std::vector<Element> hint_elements;
    for (const auto& hint : hints) {
        hint_elements.push_back(text(hint) | dim | size(WIDTH, EQUAL, 20));
    }

    return vbox({
        separator(),
        hbox(hint_elements) | center,
        text("Press 'q' to quit | Arrow keys to navigate")
            | dim | center
    });
}

Element Renderer::renderTitle(const std::string& title) const {
    auto& theme = utils::Theme::instance();

    return hbox({
        text("== "),
        text(title) | bold | color(theme.colors().accent),
        text(" ==")
    }) | center;
}

void Renderer::setFullscreen(bool fullscreen) {
    fullscreen_ = fullscreen;
}

Component wrapWithRenderer(Component component) {
    return component | ftxui::Renderer([](Element e) {
        return createMainLayout(std::move(e));
    });
}

Element createMainLayout(Element content) {
    auto& renderer = Renderer::instance();
    auto& theme = utils::Theme::instance();

    Element layout = vbox({
        renderer.renderHeader(),
        separator(),
        std::move(content),
        renderer.renderFooter({
            "Up/Down Navigate",
            "Enter Select",
            "Tab Next"
        })
    });

    return layout | bgcolor(theme.colors().background);
}

} // namespace renderer
} // namespace stl_explorer
