#pragma once

#include <unordered_map>
#include <unordered_set>
#include "Term.h"
#include "NonTerm.h"

struct Grammar {
    std::string start;
    std::unordered_map<std::string, non_term_t> non_terms;
    std::unordered_map<std::string, term_t> terms;
    std::vector<char> skip_symbols;

    std::unordered_map<std::string, std::unordered_set<std::string>> first;
    std::unordered_map<std::string, std::unordered_set<std::string>> follow;

    void gen_parser();

    Grammar() = default;
    explicit Grammar(std::string const & start);
    void add_term(Term const &);
    void add_non_term(NonTerm const &);
    void print();
    void calc_first();
    void calc_follow();
    std::unordered_set<std::string> get_first(Rule const& alpha);
};
