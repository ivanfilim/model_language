//
// Created by ivan on 6/4/25.
//

#ifndef SCANNER_H
#define SCANNER_H
#include <iostream>

#include "Lex.h"


class scanner {
    FILE *fp;
    char c;

    static int look(const string& buf, const string * list) {
        int i = 0;
        while (list[i] != "end_of_array") {
            if (buf == list [ i ])
                return i;
            i++;
        }
        return 0;
    }
    void gc () { c = fgetc (fp); }
    static string TW [], TD [];
    static type_of_lex words [];
    static type_of_lex dlms [];
public:
    scanner (const char * program) {
        fp = fopen ( program, "r" );
    }
    Lex get_lex ();

    static void print_word_or_delim (const int TYPE) {
        if (TYPE <= LEX_STRING) cout << "Сама лексема: " << "'" << TW[TYPE] << "'; " << "тип лексемы: " << TYPE << "." << endl;
        else if (TYPE <= LEX_GEQ) cout << "Сама лексема: " << "'" << TD[TYPE - LEX_STRING] << "'; " << "тип лексемы: " << TYPE << "." << endl;
    }
};



#endif //SCANNER_H
