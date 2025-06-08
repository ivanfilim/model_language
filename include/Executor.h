//
// Created by ivan on 6/8/25.
//

#ifndef EXECUTOR_H
#define EXECUTOR_H
#include "Lex.h"


class Poliz;

class Executor
{
    Lex pc_el;
public:
    void execute(Poliz & prog);
};


#endif //EXECUTOR_H
