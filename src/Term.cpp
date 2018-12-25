#include <iostream>
#include "Term.h"

Term::Term(std::string const &name, std::string const &regex) : Unit(name), regex(regex) {

}

bool Term::is_term() {
    return true;
}

void Term::print() {
    using std::cout;
    using std::endl;
    cout << name << ": " << regex << endl;
}
