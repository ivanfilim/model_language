//
// Created by ivan on 6/4/25.
//

#ifndef IDENT_H
#define IDENT_H
#include <string>

#include "type_of_lex.h"

using namespace std;

class ident {
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int int_value;
    double real_value;
    string str_value;
public:
    ident () { declare = false; assign = false; }
    ident ( const char * n ) {
        name = n; declare = false; assign = false; }
    bool operator== (const string& s) const {
        return name == s;
    }
    void put_name (const string &n) { name = n; }
    string get_name ( ) { return name; }
    bool get_declare ( ) const { return declare; }
    void put_declare ( ) { declare = true; }
    type_of_lex get_type ( ) const { return type; }
    void put_type ( type_of_lex t ) { type = t; }
    bool get_assign ( ) const { return assign; }
    void put_assign ( ) { assign = true; }
    int get_int_value ( ) const { return int_value; }
    double get_real_value ( ) const { return real_value; }
    string get_str_value ( ) { return str_value; }
    void put_int_value (const int v ) { int_value = v; }
    void put_real_value (const double v ) { real_value = v; }
    void put_str_value (const string &v ) { str_value = v; }
};



#endif //IDENT_H
