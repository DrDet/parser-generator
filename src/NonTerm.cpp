#include <iostream>
#include <memory>
#include "NonTerm.h"

void NonTerm::add_rule(Rule const &rule) {
    rules.push_back(rule);
}

NonTerm::NonTerm(std::string const &name) : Unit(name) {

}

bool NonTerm::is_non_term() {
    return true;
}

void NonTerm::print() {
    using std::cout;
    using std::endl;
    cout << name << ":\n";
    for (auto &rule : rules) {
        cout << "| ";
        for (auto &unit : rule) {
            cout << unit << " ";
        }
        cout << endl;
    }
}
