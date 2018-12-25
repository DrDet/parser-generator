#pragma once

#include <vector>

#include "Unit.h"

using unit_name_t = std::string;
using Rule = std::vector<unit_name_t>;

struct NonTerm : public Unit {
    std::vector<Rule> rules;

    NonTerm() = default;
    explicit NonTerm(std::string const & name);

    void add_rule(Rule const & rule);
    bool is_non_term() override;
    void print();
};

using non_term_t = std::shared_ptr<NonTerm>;