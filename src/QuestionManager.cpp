#include "QuestionManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <random>
#include <chrono>
#include <iostream>

using json = nlohmann::json;

QuestionManager::QuestionManager() {}

bool QuestionManager::loadQuestions(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open questions file: " << filepath << std::endl;
        return false;
    }

    json data;
    file >> data;

    for (auto& q : data["questions"]) {
        Question question;
        question.questionText = q["question"].get<std::string>();
        question.optionA = q["optionA"].get<std::string>();
        question.optionB = q["optionB"].get<std::string>();
        question.optionC = q["optionC"].get<std::string>();
        question.optionD = q["optionD"].get<std::string>();
        question.correctAnswer = q["answer"].get<std::string>()[0];
        question.category = q["category"].get<std::string>();
        question.difficulty = q["difficulty"].get<std::string>();
        questions.push_back(question);
    }

    std::cout << "Loaded " << questions.size() << " questions." << std::endl;
    return true;
}

Question QuestionManager::getRandomQuestion(const std::string& category, const std::string& difficulty) {
    static auto rng = std::mt19937(
        std::chrono::steady_clock::now().time_since_epoch().count());

    // Build candidates: matching questions that haven't been used yet
    std::vector<int> candidates;
    for (int i = 0; i < (int)questions.size(); i++) {
        if (usedIndices.count(i)) continue;
        if (questions[i].category == category && questions[i].difficulty == difficulty)
            candidates.push_back(i);
    }

    // Fallback 1: any unused question in that category
    if (candidates.empty()) {
        for (int i = 0; i < (int)questions.size(); i++) {
            if (usedIndices.count(i)) continue;
            if (questions[i].category == category)
                candidates.push_back(i);
        }
    }

    // Fallback 2: any unused question
    if (candidates.empty()) {
        for (int i = 0; i < (int)questions.size(); i++) {
            if (usedIndices.count(i)) continue;
            candidates.push_back(i);
        }
    }

    // Fallback 3: all questions exhausted — reset and try again
    if (candidates.empty()) {
        usedIndices.clear();
        for (int i = 0; i < (int)questions.size(); i++)
            candidates.push_back(i);
    }

    std::uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
    int chosen = candidates[dist(rng)];
    usedIndices.insert(chosen);
    return questions[chosen];
}

void QuestionManager::resetUsed() {
    usedIndices.clear();
}
