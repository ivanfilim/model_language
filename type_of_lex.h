//
// Created by ivan on 6/4/25.
//

#ifndef TYPE_OF_LEX_H
#define TYPE_OF_LEX_H

enum type_of_lex {
    LEX_NULL,        // 0
    LEX_AND,         // 1
    LEX_DO,          // 2
    LEX_ELSE,        // 3
    LEX_IF,          // 4
    LEX_FALSE,       // 5
    LEX_INT,         // 6
    LEX_NOT,         // 7
    LEX_OR,          // 8
    LEX_PROGRAM,     // 9
    LEX_READ,        // 10
    LEX_REAL,        // 11
    LEX_TRUE,        // 12
    LEX_WHILE,       // 13
    LEX_WRITE,       // 14
    LEX_FOR,         // 15
    LEX_GOTO,        // 16
    LEX_STRING,      // 17
    LEX_SEMICOLON,   // 18
    LEX_LBRACE,      // 19
    LEX_RBRACE,      // 20
    LEX_COMMA,       // 21
    LEX_COLON,       // 22
    LEX_ASSIGN,      // 23
    LEX_LPAREN,      // 24
    LEX_RPAREN,      // 25
    LEX_EQ,          // 26
    LEX_LSS,         // 27
    LEX_GTR,         // 28
    LEX_PLUS,        // 29
    LEX_MINUS,       // 30
    LEX_TIMES,       // 31
    LEX_SLASH,       // 32
    LEX_LEQ,         // 33
    LEX_NEQ,         // 34
    LEX_GEQ,         // 35
    LEX_ID,          // 36
    LEX_INT_NUM,     // 37
    LEX_REAL_NUM,    // 38
    LEX_STR,         // 39
    LEX_EOF,         // 40
    POLIZ_GO,        // 41
    POLIZ_FGO,       // 42
    POLIZ_LABEL,     // 43
    POLIZ_ADDRESS,   // 44
    POLIZ_UN_PLUS,   // 45
    POLIZ_UN_MINUS,  // 46
};

#endif //TYPE_OF_LEX_H
