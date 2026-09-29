/**
 * @file markov_test.cpp
 * @brief Тестовое приложение для Нормальных Алгоритмов Маркова (НАМ) с поддержкой CLI и интерактивного меню.
 */
#include "markov.hpp"
#include <iostream>
#include <fstream>
#include <limits>
#include <string>

void printMenu() {
    std::cout << "\nMarkov Algorithm Menu\n";
    std::cout << "1. Add rule ('ab -> c' or 'ab ->. c' for a final rule)\n";
    std::cout << "2. Clear rules\n";
    std::cout << "3. Show rules\n";
    std::cout << "4. Set input word (applies rules & resets algorithm)\n";
    std::cout << "5. Step once\n";
    std::cout << "6. Run until halt\n";
    std::cout << "7. Show algorithm status\n";
    std::cout << "8. Rule count\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int runFileMode(int argc, char* argv[]) {
    std::string filename;
    bool logEnabled = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-log") {
            logEnabled = true;
        } else if (filename.empty()) {
            filename = arg;
        }
    }

    if (filename.empty()) {
        std::cerr << "Ошибка: не указан путь к файлу.\n";
        return 1;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << "\n";
        return 1;
    }

    std::string initialWord;
    if (!(file >> initialWord)) {
        std::cerr << "Ошибка: неверный формат файла (ожидается начальное слово)\n";
        return 1;
    }

    RuleSet activeRules;
    try {
        file >> activeRules;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при чтении правил из файла: " << e.what() << "\n";
        return 1;
    }

    MarkovAlgorithm ma(initialWord, activeRules);

    if (logEnabled) {
        std::cout << "Стартовое состояние\n" << ma << "\n\n";
    }

    while (ma.step()) {
        if (logEnabled) {
            std::cout << ma << "\n\n";
        }
    }

    std::cout << "Алгоритм остановился\n";
    std::cout << "Финальный результат:\n" << ma << "\n";

    return 0;
}

int runInteractiveMode() {
    RuleSet activeRules;
    MarkovAlgorithm ma;
    int choice;

    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                std::cout << "Enter rule (format: left -> right, or left ->. right): ";
                Rule rule;
                if (std::cin >> rule) {
                    activeRules.addRule(rule);
                    std::cout << "Rule added to configuration!\n";
                    std::cout << "Note: Use option 4 to apply new rules to the algorithm.\n";
                } else {
                    if (std::cin.eof()) return 0;
                    std::cin.clear();
                    std::cout << "Invalid rule format!\n";
                }
                break;
            }
            case 2:
                activeRules.clear();
                std::cout << "Configuration rules cleared!\n";
                break;
            case 3:
                std::cout << "Configured Rules:\n" << activeRules;
                break;
            case 4: {
                std::cout << "Enter input word (may be empty): ";
                std::string word;
                std::getline(std::cin, word);
                if (!word.empty() && word.back() == '\r') word.pop_back();

                ma = MarkovAlgorithm(word, activeRules);
                std::cout << "Algorithm reset and word initialized!\n";
                break;
            }
            case 5: {
                bool executed = ma.step();
                if (executed) {
                    std::cout << "Executed 1 step.\n";
                } else {
                    std::cout << "Algorithm halted or no rule matched!\n";
                }
                std::cout << ma << "\n";
                break;
            }
            case 6:
                ma.run();
                if (ma.isHalted()) {
                    std::cout << "Execution finished.\n";
                } else {
                    std::cout << "Step limit reached, algorithm is still running.\n";
                }
                std::cout << ma << "\n";
                break;
            case 7:
                std::cout << ma << "\n";
                break;
            case 8:
                std::cout << "Rule count: " << activeRules.size() << "\n";
                break;
            default:
                std::cout << "Invalid choice!\n";
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return runFileMode(argc, argv);
    } 
    else {
        std::cout << "Запуск в интерактивном режиме (аргументы не переданы)...\n";
        return runInteractiveMode();
    }
}