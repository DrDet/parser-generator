#include "Unit.h"
#include "Term.h"
#include "NonTerm.h"

Unit::Unit(std::string const &s) : name(s) {

}

Term* Unit::as_term() {
    return dynamic_cast<Term*>(this);
}

NonTerm* Unit::as_non_term() {
    return dynamic_cast<NonTerm*>(this);
}

bool Unit::is_term() {
    return false;
}

bool Unit::is_non_term() {
    return false;
}

