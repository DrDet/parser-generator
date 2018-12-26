#include <iostream>
#include <memory>
#include "NonTerm.h"

void NonTerm::add_rule(Rule const &rule, std::string const & code, const std::vector<std::string> & exp_list) {
    rules.push_back(rule);
    this->code.push_back(code);
    exp_lists.push_back(exp_list);
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
    cout << "ret type: " << ret_type << endl;
    cout << "code:\n";
    for (auto &c: code) {
        cout << c << endl;
    }
}

NonTerm::NonTerm() : ret_type("int") {

}
