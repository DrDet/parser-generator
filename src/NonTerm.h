#pragma once

#include <vector>
#include "Unit.h"

using unit_name_t = std::string;
using Rule = std::vector<unit_name_t>;

struct NonTerm : public Unit {
    std::vector<Rule> rules;
    std::vector<std::vector<std::string> > exp_lists; // rule_num -> std::vector<exp_list>

    std::vector<std::string> code;

    std::string arg_list;
    std::string ret_type;

    NonTerm();
    explicit NonTerm(std::string const & name);

    void add_rule(Rule const & rule, std::string const & code, const std::vector<std::string> & exp_list);
    bool is_non_term() override;
    void print();
};

using non_term_t = std::shared_ptr<NonTerm>;