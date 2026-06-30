#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "utils/theme.hpp"
#include "utils/logger.hpp"
#include "core/renderer/renderer.hpp"
#include "containers/vector_module.hpp"
#include "containers/stack_module.hpp"
#include "containers/queue_module.hpp"
#include "containers/deque_module.hpp"
#include "containers/list_module.hpp"
#include "containers/map_module.hpp"
#include "containers/unordered_map_module.hpp"
#include "containers/set_module.hpp"
#include "containers/priority_queue_module.hpp"
#include "algorithms/sorting/sorting_algorithms.hpp"
#include "algorithms/searching/searching_algorithms.hpp"
#include "core/quiz/quiz_module.hpp"
#include "core/compare/compare_module.hpp"

using namespace ftxui;

namespace stl_explorer {

class Application {
public:
    Application() {
        setupTheme();
        
        vector_module_ = std::make_shared<containers::VectorModule>();
        stack_module_ = std::make_shared<containers::StackModule>();
        queue_module_ = std::make_shared<containers::QueueModule>();
        deque_module_ = std::make_shared<containers::DequeModule>();
        list_module_ = std::make_shared<containers::ListModule>();
        map_module_ = std::make_shared<containers::MapModule>();
        unordered_map_module_ = std::make_shared<containers::UnorderedMapModule>();
        set_module_ = std::make_shared<containers::SetModule>();
        priority_queue_module_ = std::make_shared<containers::PriorityQueueModule>();
        sorting_module_ = std::make_shared<algorithms::SortingModule>();
        searching_module_ = std::make_shared<algorithms::SearchModule>();
        quiz_module_ = std::make_shared<quiz::QuizModule>();
        compare_module_ = std::make_shared<compare::CompareModule>();

        auto go_back = [this]() {
            tab_index_ = 0;
            in_module_ = false;
        };

        vector_module_->setBackCallback(go_back);
        stack_module_->setBackCallback(go_back);
        queue_module_->setBackCallback(go_back);
        deque_module_->setBackCallback(go_back);
        list_module_->setBackCallback(go_back);
        map_module_->setBackCallback(go_back);
        unordered_map_module_->setBackCallback(go_back);
        set_module_->setBackCallback(go_back);
        priority_queue_module_->setBackCallback(go_back);
        sorting_module_->setBackCallback(go_back);
        searching_module_->setBackCallback(go_back);
        quiz_module_->setBackCallback(go_back);
        compare_module_->setBackCallback(go_back);

        buildMainMenu();
    }

    void run() {
        auto screen = ScreenInteractive::Fullscreen();
        exit_closure_ = screen.ExitLoopClosure();
        screen.Loop(main_renderer_);
    }

private:
    void setupTheme() {
        utils::Theme::instance().setTheme(utils::ThemeType::Dark);
    }



    void buildMainMenu() {
        menu_entries_ = {
            "1. Vector",
            "2. Stack",
            "3. Queue",
            "4. Deque",
            "5. List",
            "6. Map",
            "7. Unordered Map",
            "8. Set",
            "9. Priority Queue",
            "10. Sorting Algorithms",
            "11. Searching Algorithms",
            "12. Quiz Mode",
            "13. Compare Containers",
            "14. Exit"
        };

        menu_option_ = Menu(&menu_entries_, &selected_menu_, utils::Theme::instance().menuOption());

        auto menu_component = Container::Vertical({
            menu_option_
        });

        auto on_enter_key = CatchEvent(menu_component, [&](Event event) {
            if (event == Event::Return) {
                if (selected_menu_ == 13) {
                    if (exit_closure_) exit_closure_();
                    return true;
                }
                if (selected_menu_ <= 10) {
                    pending_selected_menu_ = selected_menu_;
                    init_prompt_input_.clear();
                    menu_tab_index_ = 1;
                    return true;
                }
                enterSelectedModule();
                return true;
            }
            if (event == Event::Escape) {
                if (exit_closure_) exit_closure_();
                return true;
            }
            if (event == Event::Character('q')) {
                if (exit_closure_) exit_closure_();
                return true;
            }
            return false;
        });

        auto menu_renderer = Renderer(on_enter_key, [&]() {
            return renderMainMenu();
        });

        // Initialize Prompt UI setup
        auto& theme = utils::Theme::instance();
        auto btn_opt = theme.buttonOption();

        InputOption init_opt;
        init_opt.placeholder = "e.g. 5,12,7 or leave empty for random...";
        init_opt.on_enter = [this]() {
            submitInitInput();
        };
        init_input_ = Input(&init_prompt_input_, init_opt);

        auto styled_init_input = Renderer(init_input_, [this]() {
            auto& theme = utils::Theme::instance();
            auto e = init_input_->Render();
            if (init_input_->Focused()) {
                return e | border | color(theme.colors().primary) | bold;
            }
            return e | border | color(theme.colors().muted);
        });

        auto proceed_btn = Button("Proceed", [this]() { submitInitInput(); }, btn_opt);
        auto cancel_btn = Button("Cancel", [this]() {
            menu_tab_index_ = 0;
        }, btn_opt);

        auto prompt_buttons = Container::Horizontal({proceed_btn, cancel_btn});
        auto prompt_container = Container::Vertical({
            styled_init_input,
            prompt_buttons
        });

        auto prompt_events = CatchEvent(prompt_container, [this](Event event) {
            if (event == Event::Escape) {
                menu_tab_index_ = 0;
                return true;
            }
            return false;
        });

        prompt_renderer_ = Renderer(prompt_events, [this, proceed_btn, cancel_btn]() {
            auto& theme = utils::Theme::instance();
            std::string dsa_name = menu_entries_[pending_selected_menu_];
            size_t dot = dsa_name.find(". ");
            if (dot != std::string::npos) {
                dsa_name = dsa_name.substr(dot + 2);
            }

            auto title = text("Initialize " + dsa_name) | bold | color(theme.colors().primary) | center;
            auto label = text("Enter initial comma-separated values (numbers only):") | center;
            auto sublabel = text("Or leave empty and press Enter to generate random values.") | dim | center;

            return vbox({
                title,
                separator(),
                label,
                sublabel,
                separator(),
                init_input_->Render() | center,
                separator(),
                hbox({
                    proceed_btn->Render(),
                    text("   "),
                    cancel_btn->Render()
                }) | center,
                separator(),
                text("─── Controls ───") | bold | center,
                text("Tab / Arrow keys: Navigate between elements (bright blue hover)") | dim | center,
                text("Press Enter in input field or select Proceed to continue") | dim | center,
                text("ESC or select Cancel to return to main menu") | dim | center
            }) | border | center | size(WIDTH, EQUAL, 60) | size(HEIGHT, EQUAL, 18) | bgcolor(theme.colors().background);
        });

        auto menu_and_prompt = Container::Tab({
            menu_renderer,
            prompt_renderer_
        }, &menu_tab_index_);

        auto tab_container = Container::Tab({
            menu_and_prompt,
            vector_module_->getComponent(),
            stack_module_->getComponent(),
            queue_module_->getComponent(),
            deque_module_->getComponent(),
            list_module_->getComponent(),
            map_module_->getComponent(),
            unordered_map_module_->getComponent(),
            set_module_->getComponent(),
            priority_queue_module_->getComponent(),
            sorting_module_->getComponent(),
            searching_module_->getComponent(),
            quiz_module_->getComponent(),
            compare_module_->getComponent()
        }, &tab_index_);

        auto global_event_handler = CatchEvent(tab_container, [&](Event event) {
            if (event == Event::Escape || event == Event::Character('q')) {
                if (tab_index_ > 0) {
                    tab_index_ = 0;
                    in_module_ = false;
                    return true;
                }
            }
            return false;
        });

        main_renderer_ = global_event_handler;
    }

    void enterSelectedModule() {
        if (selected_menu_ == 13) {
            return;
        }
        tab_index_ = selected_menu_ + 1;
        in_module_ = true;

        switch (selected_menu_) {
            case 0: vector_module_->reset(); break;
            case 1: stack_module_->reset(); break;
            case 2: queue_module_->reset(); break;
            case 3: deque_module_->reset(); break;
            case 4: list_module_->reset(); break;
            case 5: map_module_->reset(); break;
            case 6: unordered_map_module_->reset(); break;
            case 7: set_module_->reset(); break;
            case 8: priority_queue_module_->reset(); break;
            case 9: sorting_module_->reset(); break;
            case 10: searching_module_->reset(); break;
            case 11: quiz_module_->startQuiz(); break;
        }
    }

    void submitInitInput() {
        menu_tab_index_ = 0;
        tab_index_ = pending_selected_menu_ + 1;
        in_module_ = true;

        switch (pending_selected_menu_) {
            case 0: vector_module_->resetWithInput(init_prompt_input_); break;
            case 1: stack_module_->resetWithInput(init_prompt_input_); break;
            case 2: queue_module_->resetWithInput(init_prompt_input_); break;
            case 3: deque_module_->resetWithInput(init_prompt_input_); break;
            case 4: list_module_->resetWithInput(init_prompt_input_); break;
            case 5: map_module_->resetWithInput(init_prompt_input_); break;
            case 6: unordered_map_module_->resetWithInput(init_prompt_input_); break;
            case 7: set_module_->resetWithInput(init_prompt_input_); break;
            case 8: priority_queue_module_->resetWithInput(init_prompt_input_); break;
            case 9: sorting_module_->resetWithInput(init_prompt_input_); break;
            case 10: searching_module_->resetWithInput(init_prompt_input_); break;
        }
        init_prompt_input_.clear();
    }

    Element renderModule() const {
        auto& theme = utils::Theme::instance();

        // Show placeholder for each module
        std::string module_name;
        switch (current_module_) {
            case 0: module_name = "Vector"; break;
            case 1: module_name = "Stack"; break;
            case 2: module_name = "Queue"; break;
            case 3: module_name = "Deque"; break;
            case 4: module_name = "List"; break;
            case 5: module_name = "Map"; break;
            case 6: module_name = "Unordered Map"; break;
            case 7: module_name = "Set"; break;
            case 8: module_name = "Priority Queue"; break;
            case 9: module_name = "Algorithms"; break;
            case 10: module_name = "Quiz Mode"; break;
            case 11: module_name = "Compare Containers"; break;
            default: module_name = "Unknown"; break;
        }

        return vbox({
            text(module_name + " Module") | bold | color(theme.colors().primary) | center,
            separator(),
            text("Module content goes here") | dim | center,
            separator(),
            text("Press ESC to go back") | dim | center
        }) | bgcolor(theme.colors().background) | center;
    }

    Element renderMainMenu() const {
        auto& theme = utils::Theme::instance();

        auto header = vbox({
            text("========================================") | color(theme.colors().primary) | center,
            text("          STL Explorer                 ") | bold | color(theme.colors().highlight) | center,
            text("      Learn C++ STL Visually          ") | dim | center,
            text("========================================") | color(theme.colors().primary) | center
        });

        auto menu_title = text("--- Select a Container or Feature ---")
            | color(theme.colors().accent) | center;

        return vbox({
            header,
            separator(),
            menu_title,
            separator(),
            menu_option_->Render() | frame | size(WIDTH, LESS_THAN, 40),
            separator(),
            vbox({
                text("Use Up/Down to navigate, Enter to select") | dim | center,
                text("Press ESC or q to go back/exit") | dim | center,
                text("Version 1.0.0 | C++20 | FTXUI") | dim | center
            }),
        }) | center | bgcolor(theme.colors().background);
    }

    // Components
    Component main_renderer_;
    Component menu_option_;
    Component prompt_renderer_;
    Component init_input_;

    // State
    int selected_menu_{0};
    int current_module_{0};
    bool in_module_{false};
    int tab_index_{0};
    int menu_tab_index_{0};
    int pending_selected_menu_{0};
    std::string init_prompt_input_;
    std::vector<std::string> menu_entries_;
    std::function<void()> exit_closure_;

    // Module instances
    std::shared_ptr<containers::VectorModule> vector_module_;
    std::shared_ptr<containers::StackModule> stack_module_;
    std::shared_ptr<containers::QueueModule> queue_module_;
    std::shared_ptr<containers::DequeModule> deque_module_;
    std::shared_ptr<containers::ListModule> list_module_;
    std::shared_ptr<containers::MapModule> map_module_;
    std::shared_ptr<containers::UnorderedMapModule> unordered_map_module_;
    std::shared_ptr<containers::SetModule> set_module_;
    std::shared_ptr<containers::PriorityQueueModule> priority_queue_module_;
    std::shared_ptr<algorithms::SortingModule> sorting_module_;
    std::shared_ptr<algorithms::SearchModule> searching_module_;
    std::shared_ptr<quiz::QuizModule> quiz_module_;
    std::shared_ptr<compare::CompareModule> compare_module_;
};

} // namespace stl_explorer

int main() {
    try {
        stl_explorer::Application app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
