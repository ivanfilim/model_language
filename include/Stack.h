//
// Created by ivan on 6/8/25.
//

#ifndef STACK_H
#define STACK_H

template <class T, int max_size>
class Stack {
    T s[max_size];
    int top;
public:
    Stack() { top = 0; }
    void reset() { top = 0; }
    void push(T i);
    T pop();
    [[nodiscard]] bool is_empty() const { return top == 0; }
    [[nodiscard]] bool is_full() const { return top == max_size; }
};

template <class T, int max_size>
void Stack <T, max_size >::push(T i) {
    if ( !is_full() )
    {
        s[top] = i;
        ++top;
    }
    else throw "Stack_is_full";
}

template <class T, int max_size>
T Stack <T, max_size >::pop() {
    if (!std::is_empty<T>())
    {
        --top;
        return s[top];
    }
    throw "Stack_is_empty";
}

#endif //STACK_H
