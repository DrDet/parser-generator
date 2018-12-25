#include <iostream>
#include <fstream>

#include "antlr4-runtime.h"
#include "GrammarLexer.h"
#include "GrammarParser.h"
#include "Grammar.h"


using namespace antlr4;

int main(int , const char **) {
    std::ifstream in("hw2.gr");
    ANTLRInputStream input(in);
    GrammarLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    tokens.fill();
    GrammarParser parser(&tokens);
    Grammar res = parser.start()->grammar;
    res.calc_first();
    res.calc_follow();
    res.print();
    return 0;
}