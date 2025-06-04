//
// Created by ivan on 6/4/25.
//

#include "table_ident.h"

#include <string>

using namespace std;

int table_ident::put (const string &buf) {
    for (int j = 1; j < top; j++)
        if (buf == p[j].get_name()) return j;
    p[top].put_name(buf);
    top++;
    return top-1;
};