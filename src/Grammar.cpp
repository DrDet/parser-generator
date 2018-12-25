#include <iostream>
#include <fstream>
#include <sstream>
#include "Grammar.h"

void Grammar::add_term(Term const & term) {
    terms[term.name] = std::make_shared<Term>(term);
}

void Grammar::add_non_term(NonTerm const & non_term) {
    non_terms[non_term.name] = std::make_shared<NonTerm>(non_term);
}

void Grammar::print() {
    using std::cout;
    using std::endl;
    cout << "start-rule: " << start << endl;
    cout << "Non-terms:\n";
    for (auto &non_term : non_terms) {
        non_term.second->print();
    }
    cout << "~~~~~~~~~~~~\n";
    cout << "Terms:\n";
    for (auto &term : terms) {
        term.second->print();
    }
    cout << "~~~~~~~~~~~~\n";
    cout << "FIRST:\n";
    for (auto &non_term : non_terms) {
        cout << non_term.second->name << ":" << endl;
        for (auto &e: first[non_term.second->name]) {
            cout << e << "; ";
        }
        cout << endl;
    }
    cout << "~~~~~~~~~~~~\n";
    cout << "FOLLOW:\n";
    for (auto &non_term : non_terms) {
        cout << non_term.second->name << ":" << endl;
        for (auto &e: follow[non_term.second->name]) {
            cout << e << "; ";
        }
        cout << endl;
    }
    cout << "~~~~~~~~~~~~\n";
}

void Grammar::calc_first() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &non_term : non_terms) {
            std::string& rule_name = non_term.second->name;
            for (auto &rule : non_term.second->rules) {
                auto res = get_first(rule);
                size_t before_size = first[rule_name].size();
                first[rule_name].insert(res.begin(), res.end());
                changed = changed || (before_size != first[rule_name].size());
            }
        }
    }
}

void Grammar::calc_follow() {
    follow[start] = {"$"};
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &non_term : non_terms) {
            std::string& rule_name = non_term.second->name;
            for (auto &rule : non_term.second->rules) {
                for (size_t i = 0; i < rule.size(); ++i) {
                    size_t before_size = follow[rule[i]].size();
                    if (islower(rule[i][0])) {
                        auto tmp = get_first(Rule(rule.begin() + i + 1, rule.end()));
                        if (tmp.find("#") != tmp.end()) {
                            tmp.erase("#");
                            follow[rule[i]].insert(tmp.begin(), tmp.end());
                            follow[rule[i]].insert(follow[rule_name].begin(), follow[rule_name].end());
                        } else {
                            follow[rule[i]].insert(tmp.begin(), tmp.end());
                        }
                        changed = changed || (before_size != follow[rule[i]].size());
                    }
                }
            }
        }
    }
}

std::unordered_set<std::string> Grammar::get_first(Rule const &alpha) {
    if (alpha.empty()) {
        return {"#"};
    }
    if (isupper(alpha[0][0])) {
        return { alpha[0] };
    } else {
        auto lhs = first[alpha[0]];
        if (lhs.find("#") != lhs.end()) {
            lhs.erase("#");
            auto rhs = get_first(Rule(alpha.begin() + 1, alpha.end()));
            lhs.insert(rhs.begin(), rhs.end());
            return lhs;
        } else {
            return lhs;
        }
    }
}

Grammar::Grammar(std::string const &start) : start(start) {
}

void Grammar::gen_parser() {
    std::ofstream parser_h_code("gen/Parser.h");
    parser_h_code <<
    "#pragma once\n"
    "\n"
    "#include \"Lexer.h\"\n"
    "#include \"Tree.h\"\n"
    "\n"
    "class Parser {\n"
    "public:\n"
    "    Parser() = default;\n"
    "    node_t parse(const std::string & s);\n"
    "private:\n"
    "    Lexer lexer;";
    for (auto &non_term : non_terms) {
        parser_h_code << "    node_t " << non_term.second->name << "();\n";
    }
    parser_h_code <<
    "    node_t term_symbol(std::string const & s);\n"
    "    node_t eps_symbol();\n"
    "    void parser_error();\n"
    "};\n"
    "\n"
    "class parser_exception : public std::exception {\n"
    "    std::string message;\n"
    "public:\n"
    "    explicit parser_exception(const std::string &);\n"
    "    virtual const char* what() const noexcept override;\n"
    "};";
    std::ofstream parser_cpp_code("gen/Parser.cpp");
    parser_cpp_code <<
    "#include \"Parser.h\"\n"
    "#include <memory>\n"
    "\n"
    "using std::unique_ptr;\n"
    "using std::string;\n"
    "\n"
    "node_t Parser::parse(const string &s) {\n"
    "    lexer = Lexer(s);\n"
    "    node_t root = " << start << "();\n"
    "    if (lexer.get_cur_tok() == END) {\n"
    "        return root;\n"
    "    }\n"
    "    parser_error();\n"
    "}";
    for (auto &non_term : non_terms) {
        parser_cpp_code <<
        "node_t Parser::" << non_term.second->name << "() {\n"
        "    node_t root(new Node(\"" << non_term.second->name << "\"));\n"
        "    switch (lexer.get_cur_tok()) {\n";
    }
}
