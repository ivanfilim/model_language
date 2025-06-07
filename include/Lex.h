//
// Created by ivan on 6/4/25.
//

#ifndef LEX_H
#define LEX_H
#include <ostream>

#include "type_of_lex.h"
#include <string>

#include "table_ident.h"


using namespace std;

inline string TW [ ] = {
    "nullword",
    "and",
    "do",
    "else",
    "if",
    "false",
    "int",
    "not",
    "or",
    "program",
    "read",
    "real",
    "true",
    "while",
    "write",
    "for",
    "goto",
    "string",
    "end_of_array"
};

inline string TD [ ] = {
    "nulldelim",
    ";",
    "{",
    "}",
    ",",
    ":",
    "=",
    "(",
    ")",
    "==",
    "<",
    ">",
    "+",
    "-",
    "*",
    "/",
    "<=",
    "!=",
    ">=",
    "end_of_array"
};

inline table_ident TID (100);

class Lex {
    type_of_lex t_lex;
    int int_field;
    double real_field;
    string str_field;
public:
    Lex(const type_of_lex t = LEX_NULL,
        const int v = 0,
        double vreal = 0,
        const string &str = "not_string") {
        t_lex = t;
        int_field = v;
        real_field = vreal;
        str_field = str;
    }
    void put_int_value (const int v) { int_field = v; }
    void put_real_value (const double v) { real_field = v; }
    void put_string_value (const string &v) { str_field = v; }
    void put_type (const type_of_lex lex_type) { t_lex = lex_type; }
    [[nodiscard]] type_of_lex get_type () const { return t_lex; }
    int get_int_value () const { return int_field; }
    double get_real_value() const { return real_field; }
    string get_string_value() { return str_field; }
    friend ostream& operator << (ostream &s, const Lex& l) {
        if (l.t_lex == LEX_ID) s << TID[l.int_field].get_name() << endl;
        else if (l.t_lex < LEX_SEMICOLON) s << TW[l.t_lex] << endl;
        else if (l.t_lex < LEX_ID) s << TD[l.t_lex - 17] << endl;
        else if (l.t_lex == LEX_INT_NUM) s << l.int_field << endl;
        else if (l.t_lex == LEX_REAL_NUM) s << l.real_field << endl;
        else if (l.t_lex == LEX_STR) s << l.str_field << endl;
        else if (l.t_lex == POLIZ_GO) s << "!" << endl;
        else if (l.t_lex == POLIZ_FGO) s << "!F" << endl;
        else if (l.t_lex == POLIZ_ADDRESS) s << "_" << TID[l.int_field].get_name() << endl;
        else if (l.t_lex == POLIZ_LABEL) s << l.int_field << endl;
        else if (l.t_lex == POLIZ_UN_MINUS) s << "Unary - " << endl;
        else if (l.t_lex == POLIZ_UN_PLUS) s << "Unary + " << endl;
        return s;
    }
    void operator=(const Lex & lexeme)
    {
        t_lex = lexeme.t_lex;
        int_field = lexeme.int_field;
        real_field = lexeme.real_field;
        str_field = lexeme.str_field;
    }
};



#endif //LEX_H
