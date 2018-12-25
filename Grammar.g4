grammar Grammar;

@parser::context {
    #include <iostream>
    #include <string>
    #include <unordered_set>
    #include "Grammar.h"
    #include "Unit.h"
    #include "NonTerm.h"
    #include "Term.h"
}

@parser::members {
    char escape(char c) {
        switch(c) {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            case '\\':
                return '\\';
            case 'b':
                return '\b';
            case 'a':
                return '\a';
            case 'f':
                return '\f';
            case 'v':
                return '\v';
            default:
                assert(false && "Unknown escape sequence");
        }
    }

    char get_char(const std::string& s) {
        if (s.length() == 1)
            return s[0];
        return escape(s[1]);
    }
}

start returns [Grammar grammar]
    :
    '@start:' NTERM ';'     { $grammar = Grammar($NTERM.text); }
    (
        non_terminal
        {
            $grammar.add_non_term($non_terminal.non_term);
        }
    )+
    (
        terminal
        {
            $grammar.add_term($terminal.term);
        }
    )+
    (
        skip_symbols
        {
            $grammar.skip_symbols = $skip_symbols.skip_chars;
        }
    )?
;

non_terminal returns [NonTerm non_term]
    : NTERM ':'     { $non_term = NonTerm($NTERM.text); }
        ('|' production
            {
                $non_term.add_rule($production.rule);
            }
        )+
        ';'
;

production returns [Rule rule]
    : (   NTERM      { $rule.push_back($NTERM.text); }
        | TERM       { $rule.push_back($TERM.text); }
      )+
    |                { $rule; }
;

terminal returns [Term term]
    : TERM ':' STRING ';'   { $term = Term($TERM.text, $STRING.text); }
;

skip_symbols returns [std::unordered_set<char> skip_chars]:
    '@skip:'
     (
        SYMBOL    { $skip_chars.insert(get_char($SYMBOL.text.substr(1, $SYMBOL.text.length()-2))); }
     )+
     ';'
;

TERM   : [A-Z][a-zA-Z0-9_]*;
NTERM  : [a-z][a-zA-Z0-9_]*;

STRING : '"' .*? '"';
SYMBOL : '\'' ('\\')? . '\'';
WS     : (' ' | '\t' | '\r'| '\n') -> skip;
//CODE   : '{' (~[{}]+ CODE?)* '}';