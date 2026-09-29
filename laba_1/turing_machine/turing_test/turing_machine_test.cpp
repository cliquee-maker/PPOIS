#include "turing.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

void printMenu() {
    std::cout << "\nTuring Machine Menu\n";
    std::cout << "1. Add rule ('q0 1 -> q1 0 R')\n";
    std::cout << "2. Clear rules\n";
    std::cout << "3. Show rules\n";
    std::cout << "4. Set tape string & start state (applies rules & resets machine)\n";
    std::cout << "5. Step once\n";
    std::cout << "6. Run until halt\n";
    std::cout << "7. Show machine status\n";
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

    std::string initialTape;
    std::string initialState;
    
    if (!(file >> initialTape >> initialState)) {
        std::cerr << "Ошибка: неверный формат файла (ожидается лента и начальное состояние)\n";
        return 1;
    }

    TMRuleSet activeRules;
    try {
        file >> activeRules; 
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при чтении правил из файла: " << e.what() << "\n";
        return 1;
    }

    TuringMachine tm(initialTape, activeRules, initialState);

    if (logEnabled) {
        std::cout << "Стартовое состояние\n" << tm << "\n\n";
    }

    while (tm.step()) {
        if (logEnabled) {
            std::cout << tm << "\n\n";
        }
    }

    std::cout << "Машина остановилась\n";
    std::cout << "Финальный результат:\n" << tm << "\n";

    return 0;
}

int runInteractiveMode() {
    TMRuleSet activeRules;
    TuringMachine tm;      
    std::string startState = "q0";
    int choice;

    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                std::cout << "Enter rule (format: state read -> newState write dir): ";
                TMRule rule;
                try {
                    if (std::cin >> rule) {
                        activeRules.addRule(rule);
                        std::cout << "Rule added to configuration!\n";
                        std::cout << "Note: Re-run option 4 to update active machine with new rules.\n";
                    } else {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        std::cout << "Invalid rule format!\n";
                    }
                } catch (const std::exception& e) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Error: " << e.what() << "\n";
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
                std::cout << "Enter input tape string: ";
                std::string tapeStr;
                std::cin >> tapeStr;
                
                std::cout << "Enter start state (default 'q0'): ";
                std::string st;
                std::cin >> st;
                if (!st.empty()) startState = st;

                tm = TuringMachine(tapeStr, activeRules, startState);
                std::cout << "Machine reset and tape initialized!\n";
                break;
            }
            case 5: {
                bool executed = tm.step();
                if (executed) {
                    std::cout << "Executed 1 step.\n";
                } else {
                    std::cout << "Machine halted or no rule matched!\n";
                }
                std::cout << tm << "\n";
                break;
            }
            case 6:
                tm.run();
                std::cout << "Execution finished.\n";
                std::cout << tm << "\n";
                break;
            case 7:
                std::cout << tm << "\n";
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