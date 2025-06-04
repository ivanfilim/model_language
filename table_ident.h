//
// Created by ivan on 6/4/25.
//

#ifndef TABLE_IDENT_H
#define TABLE_IDENT_H
#include "ident.h"


class table_ident {
    ident * p;
    int size;
    int top;
public:
    explicit table_ident (const int max_size) {
        p = new ident[size = max_size];
        top = 1;
    }
    ~table_ident() { delete []p; }
    ident& operator[] (const int k) const { return p[k]; }
    int put (const string &buf);
};



#endif //TABLE_IDENT_H
