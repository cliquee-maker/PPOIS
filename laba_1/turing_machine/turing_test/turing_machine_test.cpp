#include "turing_machine.hpp"
#include <iostream>
#include <string>

void printMenu() {
    std::cout << "\nTuring Machine Menu\n";
    std::cout << "1. Add rule ('q0 1 -> q1 0 R')\n";
    std::cout << "2. Clear rules\n";
    std::cout << "3. Show rules\n";
    std::cout << "4. Set tape string (applies rules & resets machine)\n";
    std::cout << "5. Step once\n";
    std::cout << "6. Run until halt\n";
    std::cout << "7. Show machine status\n";
    std::cout << "8. Rule count\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

int main() {
    TMRuleSet activeRules;
    TuringMachine tm;      
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
                if (std::cin >> rule) {
                    activeRules.addRule(rule);
                    std::cout << "Rule added to configuration!\n";
                    std::cout << "Note: Use option 4 to apply new rules to the machine.\n";
                } else {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
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
                std::cout << "Enter input tape string: ";
                std::string tapeStr;
                std::cin >> tapeStr;
                
                // Пересоздаем машину с актуальным набором правил и начальным состоянием q0
                tm = TuringMachine(tapeStr, activeRules, "q0");
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