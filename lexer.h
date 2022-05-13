/*
 * Copyright (C) Rida Bazzi, 2017
 * Do not copy or repackage
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    VAR, FOR, IF, WHILE, SWITCH, CASE, DEFAULT, INPUT, OUTPUT, ARRAY,
    PLUS, MINUS, DIV, MULT,
    EQUAL, COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS,
    NUM, ID, ERROR
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    Token peek(int);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokenList;
    Token GetTokenMain();
    int line_no;
    int index;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    int FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

#endif  //__LEXER__H__
