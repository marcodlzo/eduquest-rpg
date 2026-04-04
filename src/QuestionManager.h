#ifndef QUESTIONMANAGER_H
#define QUESTIONMANAGER_H

#include <string>
#include <vector>
#include <set>

struct Question {
    std::string questionText;
    std::string optionA, optionB, optionC, optionD;
    char correctAnswer;
    std::string category;
    std::string difficulty;
};

class QuestionManager {
public:
    QuestionManager();

    bool loadQuestions(const std::string& filepath);
    Question getRandomQuestion(const std::string& category, const std::string& difficulty);
    void resetUsed(); // Call on game reset to allow all questions again

private:
    std::vector<Question> questions;
    std::set<int> usedIndices; // Tracks which questions have been asked
};

#endif
