#include "core/quiz/quiz_module.hpp"
#include "utils/theme.hpp"
#include <algorithm>

namespace stl_explorer {
namespace quiz {

using namespace ftxui;

QuizModule::QuizModule() {
    generateQuestions();
    buildUI();
}

void QuizModule::buildUI() {
    auto& theme = utils::Theme::instance();
    auto btn_opt = theme.buttonOption();

    auto a_btn = Button("A", [this]() { selectAnswer(0); }, btn_opt);
    auto b_btn = Button("B", [this]() { selectAnswer(1); }, btn_opt);
    auto c_btn = Button("C", [this]() { selectAnswer(2); }, btn_opt);
    auto d_btn = Button("D", [this]() { selectAnswer(3); }, btn_opt);
    auto next_btn = Button("Next Question", [this]() { nextQuestion(); }, btn_opt);
    auto back_btn = Button("[Back]", [this]() { if (back_callback_) back_callback_(); }, btn_opt);

    auto btns = Container::Vertical({
        Container::Horizontal({a_btn, b_btn, c_btn, d_btn}),
        next_btn,
        back_btn
    });

    component_ = Renderer(btns, [this, btns]() {
        return renderQuestion(btns->Render());
    });
}

Element QuizModule::renderQuestion(Element buttons) const {
    auto& theme = utils::Theme::instance();

    if (quiz_complete_) {
        return vbox({
            text("Quiz Complete!") | bold | color(Color{0x4C, 0xAF, 0x50}) | center,
            separator(),
            text("Score: " + std::to_string(score_) + "/" + std::to_string(answered_questions_)) | bold | center,
            separator(),
            text("Excellent! You've completed the C++ STL visual quiz.") | dim | center,
            separator(),
            buttons | center,
            separator(),
            text("ESC/q Back to Main Menu") | dim | center
        }) | bgcolor(theme.colors().background);
    }

    if (current_question_ >= questions_.size()) {
        return text("No questions available.") | dim;
    }

    const auto& q = questions_[current_question_];

    std::vector<Element> option_elements;
    char labels[] = {'A', 'B', 'C', 'D'};

    for (size_t i = 0; i < q.options.size() && i < 4; ++i) {
        Color opt_color = theme.colors().text;

        if (answer_revealed_) {
            if (static_cast<int>(i) == q.correct_index) {
                opt_color = Color{0x4C, 0xAF, 0x50}; // Green for correct answer
            } else if (static_cast<int>(i) == selected_answer_) {
                opt_color = Color{0xF4, 0x43, 0x36}; // Red for wrong selected answer
            } else {
                opt_color = theme.colors().muted; // Muted for unselected wrong answers
            }
        } else {
            if (static_cast<int>(i) == selected_answer_) {
                opt_color = theme.colors().accent; // Highlight current selected (but not yet revealed) option
            }
        }

        auto opt_line = text(std::string(1, labels[i]) + ". " + q.options[i]) | color(opt_color);
        if (static_cast<int>(i) == selected_answer_ || (answer_revealed_ && static_cast<int>(i) == q.correct_index)) {
            opt_line = opt_line | bold;
        }
        option_elements.push_back(opt_line);
    }

    // Build Progress Bar
    float progress = static_cast<float>(current_question_) / questions_.size();
    int filled_width = static_cast<int>(progress * 30);
    std::string bar = std::string(filled_width, '=') + std::string(30 - filled_width, ' ');
    auto progress_bar = hbox({
        text("[") | dim,
        text(bar) | color(theme.colors().success),
        text("] ") | dim,
        text(std::to_string(current_question_ + 1) + "/" + std::to_string(questions_.size())) | dim
    });

    std::vector<Element> content_rows = {
        text("Quiz Mode — Test your STL knowledge") | bold | color(theme.colors().primary) | center,
        progress_bar | center,
        separator(),
        text(q.question) | bold | center,
        separator(),
        vbox(option_elements) | center,
        separator(),
        buttons | center,
        separator()
    };

    // Show Explanation
    if (answer_revealed_) {
        bool correct = (selected_answer_ == q.correct_index);
        std::string result_text = correct ? "CORRECT! " : "WRONG! ";
        Color result_color = correct ? Color{0x4C, 0xAF, 0x50} : Color{0xF4, 0x43, 0x36};

        content_rows.push_back(
            vbox({
                hbox({
                    text(result_text) | bold | color(result_color),
                    text("Correct answer: " + std::string(1, labels[q.correct_index])) | bold
                }) | center,
                text("Explanation: " + q.explanation) | dim | center
            })
        );
        content_rows.push_back(separator());
    }

    content_rows.push_back(text("Score: " + std::to_string(score_) + "/" + std::to_string(answered_questions_)) | dim | center);
    content_rows.push_back(separator());
    content_rows.push_back(text("─── Controls ───") | bold | center);
    content_rows.push_back(text("Tab / Up / Down Arrow: Switch active elements/buttons (bright blue hover)") | dim | center);
    content_rows.push_back(text("Enter: Activate the currently focused button (A/B/C/D or Next)") | dim | center);
    content_rows.push_back(text("1. Navigate to A/B/C/D and press Enter to choose your answer") | dim | center);
    content_rows.push_back(text("2. Navigate to 'Next Question' and press Enter to reveal explanation") | dim | center);
    content_rows.push_back(text("3. Press Enter on 'Next Question' again to advance") | dim | center);
    content_rows.push_back(text("ESC or q: Return to Main Menu") | dim | center);

    return vbox(content_rows) | bgcolor(theme.colors().background);
}

void QuizModule::startQuiz() {
    score_ = 0;
    answered_questions_ = 0;
    current_question_ = 0;
    quiz_complete_ = false;
    selected_answer_ = -1;
    answer_revealed_ = false;
}

void QuizModule::nextQuestion() {
    if (answer_revealed_) {
        if (current_question_ + 1 < questions_.size()) {
            current_question_++;
            selected_answer_ = -1;
            answer_revealed_ = false;
        } else {
            quiz_complete_ = true;
        }
    } else if (selected_answer_ >= 0) {
        answer_revealed_ = true;
        if (selected_answer_ == questions_[current_question_].correct_index) {
            score_++;
        }
        answered_questions_++;
    }
}

void QuizModule::selectAnswer(int index) {
    if (!answer_revealed_) {
        selected_answer_ = index;
    } else {
        nextQuestion();
    }
}

void QuizModule::endQuiz() {
    quiz_complete_ = true;
}

void QuizModule::generateQuestions() {
    questions_ = {
        Question{
            .question = "What is std::vector::push_back() average (amortized) complexity?",
            .options = {"O(1)", "O(n)", "O(log n)", "O(n^2)"},
            .correct_index = 0,
            .explanation = "Amortized complexity is O(1) because reallocations occur exponentially."
        },
        Question{
            .question = "Which container is a Last-In First-Out (LIFO) adapter?",
            .options = {"std::vector", "std::queue", "std::stack", "std::deque"},
            .correct_index = 2,
            .explanation = "std::stack restricts access to only the top element, yielding LIFO behavior."
        },
        Question{
            .question = "What data structure does std::map use internally?",
            .options = {"Hash table", "Dynamic Array", "Red-Black tree", "Doubly Linked list"},
            .correct_index = 2,
            .explanation = "std::map keeps elements sorted using a self-balancing binary search tree (Red-Black tree)."
        },
        Question{
            .question = "What is std::unordered_map average lookup complexity?",
            .options = {"O(1)", "O(n)", "O(log n)", "O(n^2)"},
            .correct_index = 0,
            .explanation = "Unordered map is implemented as a Hash Table, providing O(1) average lookup."
        },
        Question{
            .question = "What is the primary visual difference of std::list compared to std::vector?",
            .options = {"It is sorted", "It uses linked nodes", "It has O(1) random access", "It takes less memory"},
            .correct_index = 1,
            .explanation = "std::list is a doubly linked list; nodes are scattered in memory connected by pointers."
        },
        Question{
            .question = "Which complexity represents std::priority_queue's push/pop operations?",
            .options = {"O(1)", "O(n)", "O(log n)", "O(n log n)"},
            .correct_index = 2,
            .explanation = "Priority queue uses binary heaps; restoring heap property takes O(log n) operations."
        },
        Question{
            .question = "Under Lomuto partition scheme in Quick Sort, what is typically the pivot?",
            .options = {"First element", "Middle element", "Last element", "Random element"},
            .correct_index = 2,
            .explanation = "Lomuto partition scheme traditionally uses the last element of the subarray as pivot."
        },
        Question{
            .question = "Which sorting algorithm is guaranteed to be stable and run in O(n log n) worst-case?",
            .options = {"Quick Sort", "Merge Sort", "Heap Sort", "Bubble Sort"},
            .correct_index = 1,
            .explanation = "Merge Sort is a stable Divide & Conquer sort with O(n log n) time complexity in all cases."
        },
        Question{
            .question = "Why is Binary Search faster than Linear Search?",
            .options = {"It ignores duplicates", "It splits search range in half", "It searches both ends", "It is O(1)"},
            .correct_index = 1,
            .explanation = "Binary Search halves the search space each step, resulting in O(log n) time complexity."
        },
        Question{
            .question = "Which container adapter does std::stack wrap by default?",
            .options = {"std::vector", "std::list", "std::deque", "std::forward_list"},
            .correct_index = 2,
            .explanation = "By default, std::stack uses std::deque as its underlying sequence container."
        },
        Question{
            .question = "What is the worst-case time complexity of std::unordered_map::find()?",
            .options = {"O(1)", "O(log n)", "O(n)", "O(n^2)"},
            .correct_index = 2,
            .explanation = "In the worst case (e.g. hash collision of all keys into one bucket), lookup degrades to O(n)."
        },
        Question{
            .question = "Which sort is NOT in-place?",
            .options = {"Bubble Sort", "Insertion Sort", "Heap Sort", "Merge Sort"},
            .correct_index = 3,
            .explanation = "Merge Sort requires auxiliary space of O(n) to merge subarrays, so it's not in-place."
        }
    };
}

ftxui::Component QuizModule::getComponent() {
    return component_;
}

} // namespace quiz
} // namespace stl_explorer
