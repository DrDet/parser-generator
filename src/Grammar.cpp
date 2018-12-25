#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
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
    follow[start].insert("END$");
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
    "    Lexer lexer;\n";
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
    "node_t Parser::parse(const string &__str) {\n"
    "    lexer = Lexer(__str);\n"
    "    node_t root = " << start << "();\n"
    "    if (lexer.get_cur_tok() == END$) {\n"
    "        return root;\n"
    "    }\n"
    "    parser_error();\n"
    "}\n\n";
    for (auto &non_term : non_terms) {
        std::string& name = non_term.second->name;
        parser_cpp_code <<
        "node_t Parser::" << name << "() {\n"
        "    node_t root(new Node(\"" << name << "\"));\n"
        "    switch (lexer.get_cur_tok()) {\n";
        bool has_eps = false;
        for (auto &term_name: first[name]) {
            if (term_name == "#") {
                has_eps = true;
                continue;
            }
            parser_cpp_code <<
            "        case " << term_name << ": {\n";
            Rule rule = choose_rule(name, term_name);
            for (auto &unit: rule) {
                parser_cpp_code << "            ";
                if (islower(unit[0])) {
                    parser_cpp_code << "root->append_child(" << unit << "());\n";
                } else {
                    parser_cpp_code << "root->append_child(term_symbol(lexer.get_cur_tok_text()));\n";
                }
            }
            parser_cpp_code << "            break;\n        }\n";
        }
        if (has_eps) {
            for (auto &term_name: follow[name]) {
                parser_cpp_code << "        case " << term_name << ":\n";
            }
            parser_cpp_code <<
            "        {\n"
            "            root->append_child(eps_symbol());\n"
            "            break;\n"
            "        }\n";
        }
        parser_cpp_code <<
        "        default: {\n"
        "            parser_error();\n"
        "        }\n"
        "    }\n"
        "    return root;\n"
        "}\n\n"
        "\n";
    }
    parser_cpp_code <<
    "node_t Parser::term_symbol(string const & s) {\n"
    "    node_t node(new Node(s));\n"
    "    lexer.next_token();\n"
    "    return node;\n"
    "}\n"
    "\n"
    "node_t Parser::eps_symbol() {\n"
    "    return node_t(new Node(\"\"));\n"
    "}\n"
    "\n"
    "void Parser::parser_error() {\n"
    "    size_t pos = lexer.get_cur_pos();\n"
    "    char buf[50];\n"
    "    sprintf(buf, \"Unexpected token: %s at position: %zu\", lexer.get_cur_tok_text().c_str(), pos);\n"
    "    throw parser_exception(buf);\n"
    "}\n"
    "\n"
    "parser_exception::parser_exception(const string & s) : message(s) {}\n"
    "\n"
    "const char *parser_exception::what() const noexcept {\n"
    "    return message.c_str();\n"
    "}\n";
}

void Grammar::gen_lexer() {
    std::ofstream lexer_h_code("gen/Lexer.h");
    lexer_h_code <<
    "#pragma once\n"
    "#include <string>\n"
    "#include <regex>\n"
    "#include <unordered_set>\n\n"
    "enum Token {\n";
    for (auto &term : terms) {
        std::string& name = term.second->name;
        lexer_h_code <<
        "    " << name << ",\n";
    }
    lexer_h_code <<
    "    END$\n"
    "};\n\n"
    "class Lexer {\n"
    "public:\n"
    "    Lexer() = default;\n"
    "    explicit Lexer(const std::string & s);\n"
    "\n"
    "    void next_token();\n"
    "    Token get_cur_tok();\n"
    "    size_t get_cur_pos();\n"
    "    std::string get_cur_tok_text();\n"
    "private:\n"
    "    static const int tokens_num = " << terms.size() << ";\n"
    "    std::regex token_regexps[tokens_num];\n"
    "    std::unordered_set<char> skip_symbols;\n"
    "\n"
    "    Token cur_tok;\n"
    "    std:: string cur_tok_text;\n"
    "    size_t cur_pos;\n"
    "    std::string s;\n"
    "};\n"
    "\n"
    "class lexer_exception : public std::exception {\n"
    "    std::string message;\n"
    "public:\n"
    "    explicit lexer_exception(const std::string &);\n"
    "    virtual const char* what() const noexcept override;\n"
    "};\n";

    std::ofstream lexer_cpp_code("gen/Lexer.cpp");
    lexer_cpp_code <<
    "#include \"Lexer.h\"\n"
    "#include <cctype>\n"
    "#include <stdexcept>\n"
    "\n"
    "using std::string;\n"
    "\n"
    "inline bool starts_with(std::string const & s, std::string const & start) {\n"
    "    if (s.length() < start.length()) {\n"
    "        return false;\n"
    "    }\n"
    "    return s.substr(0, start.length()) == start;\n"
    "}\n"
    "\n"
    "Lexer::Lexer(const string & s) : s(s), cur_pos(0) {\n";
    lexer_cpp_code << "    skip_symbols = {";
    for (char c : skip_symbols) {
        if (c != *skip_symbols.begin()) {
            lexer_cpp_code << ", ";
        }
        lexer_cpp_code << "(char) " << static_cast<int>(c);
    }
    lexer_cpp_code << "};\n";
    for (auto &term: terms) {
        std::string& name = term.second->name;
        lexer_cpp_code << "    token_regexps[" << name << "] = std::regex(" << term.second->regex << ");\n";
    }
    lexer_cpp_code <<
    "    next_token();\n"
    "}\n"
    "\n"
    "void Lexer::next_token() {\n"
    "    while (true) {\n"
    "        if (cur_pos >= s.length()) {\n"
    "            cur_tok = END$;\n"
    "            return;\n"
    "        }\n"
    "        std::sregex_token_iterator no_match;\n"
    "        for (int i = 0; i < tokens_num; ++i) {\n"
    "            std::sregex_token_iterator it(s.begin() + cur_pos, s.end(), token_regexps[i]);\n"
    "            if (it != no_match && starts_with(s.substr(cur_pos), it->str())) {\n"
    "                cur_tok = static_cast<Token>(i);\n"
    "                cur_tok_text = it->str();\n"
    "                cur_pos += it->str().length();\n"
    "                return;\n"
    "            }\n"
    "        }\n"
    "        if (skip_symbols.find(s[cur_pos]) != skip_symbols.end()) {\n"
    "            char skip = s[cur_pos];\n"
    "            while (cur_pos < s.length() && s[cur_pos] == skip) {\n"
    "                cur_pos++;\n"
    "            }\n"
    "        } else {\n"
    "            char buf[50];\n"
    "            sprintf(buf, \"Unexpected symbol: '%c' at position: %zu\", s[cur_pos], cur_pos);\n"
    "            throw lexer_exception(buf);\n"
    "        }\n"
    "    }\n"
    "}\n"
    "\n"
    "Token Lexer::get_cur_tok() {\n"
    "    return cur_tok;\n"
    "}\n"
    "\n"
    "size_t Lexer::get_cur_pos() {\n"
    "    return cur_pos;\n"
    "}\n"
    "\n"
    "std::string Lexer::get_cur_tok_text() {\n"
    "    return cur_tok_text;\n"
    "}\n"
    "\n"
    "lexer_exception::lexer_exception(const std::string & s) : message(s) {}\n"
    "\n"
    "const char *lexer_exception::what() const noexcept {\n"
    "    return message.c_str();\n"
    "}\n";
}

void Grammar::gen_code() {
    system("if [ ! -d \"gen\" ]; then\n"
           "  mkdir gen\n"
           "fi");
    calc_first();
    calc_follow();
    gen_parser();
    gen_lexer();
    gen_tree();
}

Rule Grammar::choose_rule(std::string const &non_term_name, std::string const &first_e) {
    auto &rules = non_terms[non_term_name]->rules;
    int ans = -1;
    for (size_t i = 0; i < rules.size(); ++i) {
        auto first = get_first(rules[i]);
        if (first.find(first_e) != first.end()) {
            assert(ans == -1);
            ans = static_cast<int>(i);
        }
    }
    return rules[ans];
}

void Grammar::gen_tree() {
    std::ofstream tree_h_code("gen/Tree.h");
    tree_h_code <<
    "#pragma once\n"
    "#include <memory>\n"
    "#include <list>\n"
    "\n"
    "class Node;\n"
    "using node_t = std::unique_ptr<Node>;\n"
    "\n"
    "class Node {\n"
    "public:\n"
    "    explicit Node(std::string type);\n"
    "    Node(const Node & other) = delete;\n"
    "    void append_child(std::unique_ptr<Node> node);\n"
    "    std::string to_string();\n"
    "    std::string to_dot();\n"
    "private:\n"
    "    std::string to_dot_impl();\n"
    "    std::list<std::unique_ptr<Node>> children;\n"
    "    std::string type;\n"
    "};";
    std::ofstream tree_cpp_code("gen/Tree.cpp");
    tree_cpp_code <<
    "#include \"Tree.h\"\n"
    "#include <utility>\n"
    "#include <iostream>\n"
    "#include <queue>\n"
    "#include <sstream>\n"
    "\n"
    "using std::string;\n"
    "using std::queue;\n"
    "\n"
    "Node::Node(std::string type) : type(std::move(type)) {}\n"
    "\n"
    "void Node::append_child(node_t node) {\n"
    "    children.emplace_back(std::move(node));\n"
    "}\n"
    "\n"
    "string Node::to_string() {\n"
    "    string res(type);\n"
    "    for (auto &it : children) {\n"
    "        res += it->to_string();\n"
    "    }\n"
    "    return res;\n"
    "}\n"
    "\n"
    "std::string Node::to_dot() {\n"
    "    string res;\n"
    "    res += \"digraph G {\\n\";\n"
    "    res += to_dot_impl();\n"
    "    res += \"}\";\n"
    "    return res;\n"
    "}\n"
    "\n"
    "std::string Node::to_dot_impl() {\n"
    "    static int id = 0;\n"
    "    std::stringstream ss;\n"
    "    int v_id = id++;\n"
    "    string color = (children.empty() ? \"; color = red\" : \"\");\n"
    "    ss << \"\\t\" << v_id << \" [label = \\\"\" << type << \"\\\"\" << color << \"];\\n\";\n"
    "    for (auto &c: children) {\n"
    "        int c_id = id;\n"
    "        ss << c->to_dot_impl();\n"
    "        ss << \"\\t\" << v_id << \" -> \" << c_id << \";\\n\";\n"
    "    }\n"
    "    return ss.str();\n"
    "}\n"
    "\n";
}
