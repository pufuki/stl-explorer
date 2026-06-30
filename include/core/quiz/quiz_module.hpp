#pragma once

#include <string>
#include <vector>
#include <ftxui/component/component.hpp>
#include <functional>

namespace stl_explorer {
namespace quiz {

struct Question {
    std::string question;
    std::vector<std::string> options;
    int correct_index;
    std::string explanation;
};

class QuizModule {
public:
    QuizModule();

    ftxui::Component getComponent();

    void startQuiz();
    void nextQuestion();
    void selectAnswer(int index);
    void endQuiz();

    int score() const { return score_; }
    bool quizComplete() const { return quiz_complete_; }

void setBackCallback(std::function<void()> cb) { back_callback_ = cb; }

private:
    void buildUI();
    void generateQuestions();
    ftxui::Element renderQuestion(ftxui::Element buttons = ftxui::text("")) const;

    std::vector<Question> questions_;
    size_t current_question_{0};
    int selected_answer_{-1};
    int score_{0};
    int answered_questions_{0};
    bool quiz_complete_{false};
    bool answer_revealed_{false};

    ftxui::Component component_;
    std::function<void()> back_callback_;
};

} // namespace quiz
} // namespace stl_explorer
