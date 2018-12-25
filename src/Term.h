#pragma once

#include <string>
#include "Unit.h"

struct Term : public Unit {
    std::string regex;

    Term() = default;
    Term(std::string const & name, std::string const & regex);

    bool is_term() override;
    void print();
};

using term_t = std::shared_ptr<Term>;
