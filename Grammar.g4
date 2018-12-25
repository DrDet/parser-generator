grammar Grammar;

@parser::context {
    #include "Grammar.h"
    #include "Unit.h"
    #include "NonTerm.h"
    #include "Term.h"
}

@parser::members {
//int myAction() { return 5; }
//bool doesItBlend() { return true; }
//void cleanUp() {}
//void doInit() {}
//void doAfter() {}
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

//skip_symbols:
//    '@skip:' '('  ')'
//;

TERM   : [A-Z][a-zA-Z0-9_]*;
NTERM  : [a-z][a-zA-Z0-9_]*;

STRING : '"' .*? '"';
SKIP_SYMBOLS: '[' .+? ']';
WS      :(' ' | '\t' | '\r'| '\n') -> skip;
//CODE   : '{' (~[{}]+ CODE?)* '}';