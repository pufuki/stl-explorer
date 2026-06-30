#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>

namespace stl_explorer {
namespace renderer {

class Renderer {
public:
    static Renderer& instance();

    void setMainComponent(ftxui::Component component);
    void setMainElement(ftxui::Element element);
    ftxui::Component component();

    ftxui::Element renderHeader() const;
    ftxui::Element renderFooter(const std::vector<std::string>& hints = {}) const;
    ftxui::Element renderTitle(const std::string& title) const;

    void setFullscreen(bool fullscreen);
    bool isFullscreen() const { return fullscreen_; }

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    ftxui::Component main_component_;
    ftxui::Element main_element_;
    bool fullscreen_{false};
};

ftxui::Component wrapWithRenderer(ftxui::Component component);
ftxui::Element createMainLayout(ftxui::Element content);

} // namespace renderer
} // namespace stl_explorer
