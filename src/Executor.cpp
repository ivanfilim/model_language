//
// Created by ivan on 6/8/25.
//

#include "iostream"
#include "../include/Executor.h"
#include <string>
#include "../include/Lex.h"
#include "../include/type_of_lex.h"
#include "../include/Stack.h"
#include "../include/Poliz.h"

using namespace std;

void Executor::execute(Poliz & prog)
{
	int int_num;
	double real_num;
	string str;
	type_of_lex type2, type1;
	Stack<Lex, 100> args;
	Lex pop_lex1, pop_lex2;
	int i, index = 0, size = prog.get_free();
	while (index < size)
	{
		pc_el = prog[index];
		switch ( pc_el.get_type() )
		{
			case LEX_INT_NUM:
			case LEX_REAL_NUM:
			case LEX_STR:
			case POLIZ_ADDRESS:
			case POLIZ_LABEL:
				args.push ( pc_el );
				break;





			case LEX_ID:

				i = pc_el.get_int_value();
				if ( TID[i].get_assign() )
				{
					args.push ( pc_el );
					break;
				}
				throw "Ошибка! Неинициализированная переменная.";
			case LEX_NOT:
				pop_lex1 = args.pop();
				if (pop_lex1.get_int_value() != 0) pop_lex1.put_int_value (0);
				else pop_lex1.put_int_value (1);
				args.push ( pop_lex1 );
				break;
			case LEX_OR:
				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				if (pop_lex1.get_int_value() != 0 || pop_lex2.get_int_value() != 0)
					pop_lex1.put_int_value(1);
				else
					pop_lex1.put_int_value(0);
				args.push ( pop_lex1 );
				break;
			case LEX_AND:
				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				if (pop_lex1.get_int_value() == 0 || pop_lex2.get_int_value() == 0)
					pop_lex1.put_int_value(0);
				else
					pop_lex1.put_int_value(1);
				args.push ( pop_lex1 );
				break;
			case POLIZ_GO:
				pop_lex1 = args.pop();
				index = pop_lex1.get_int_value() - 1;
				break;
			case POLIZ_FGO:
				i = args.pop().get_int_value();
				if ( !args.pop().get_int_value() )
					index = i - 1;
				break;
			case LEX_WRITE:
				pop_lex2 = args.pop();
				if (pop_lex2.get_type() == LEX_INT_NUM) cout << pop_lex2.get_int_value() << endl;
				else if (pop_lex2.get_type() == LEX_REAL_NUM) cout << pop_lex2.get_real_value() << endl;
				else if (pop_lex2.get_type() == LEX_STR) cout << pop_lex2.get_string_value() << endl;
				else if (pop_lex2.get_type() == LEX_ID)
				{
					type_of_lex id_type = TID[pop_lex2.get_int_value()].get_type();
					if (id_type == LEX_INT) cout << TID[pop_lex2.get_int_value()].get_int_value() << endl;
					if (id_type == LEX_REAL) cout << TID[pop_lex2.get_int_value()].get_real_value() << endl;
					if (id_type == LEX_STRING) cout << TID[pop_lex2.get_int_value()].get_str_value() << endl;
				}
				break;
			case LEX_READ:
				pop_lex1 = args.pop();
				i = pop_lex1.get_int_value();
				if ( TID[i].get_type() == LEX_INT )
				{
					cin >> int_num;
					TID[i].put_int_value(int_num);
				}
				else if ( TID[i].get_type() == LEX_STRING )
				{
					cin >> str;
					TID[i].put_str_value(str);
				}
				else if ( TID[i].get_type() == LEX_REAL )
				{
					cin >> real_num;
					TID[i].put_real_value(real_num);
				}
				TID[i].put_assign();
				break;





			case LEX_PLUS:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				if (( type2 == LEX_INT || type2 == LEX_INT_NUM ) && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
				{
					pop_lex2.put_type(LEX_INT_NUM);
					pop_lex2.put_int_value(pop_lex2.get_int_value() + pop_lex1.get_int_value());
				}
				else if (( type2 == LEX_INT || type2 == LEX_INT_NUM ) && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(pop_lex2.get_int_value() + pop_lex1.get_real_value());
				}
				else if (( type2 == LEX_REAL || type2 == LEX_REAL_NUM ) && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(pop_lex2.get_real_value() + pop_lex1.get_int_value());
				}
				else if (( type2 == LEX_REAL || type2 == LEX_REAL_NUM ) && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(pop_lex2.get_real_value() + pop_lex1.get_real_value());
				}
				else if (( type2 == LEX_STR || type2 == LEX_STRING ) && ( type1 == LEX_STR || type1 == LEX_STRING ))
				{
					pop_lex2.put_type(LEX_STR);
					pop_lex2.put_string_value(pop_lex2.get_string_value() + pop_lex1.get_string_value());
				}
				else if ( type2 == LEX_ID && type1 != LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type2 == LEX_INT && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_int_value() + pop_lex1.get_int_value());
					}
					else if ( type2 == LEX_INT && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex2.get_int_value()].get_int_value() + pop_lex1.get_real_value());
					}
					else if ( type2 == LEX_REAL && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex2.get_int_value()].get_real_value() + pop_lex1.get_int_value());
					}
					else if ( type2 == LEX_REAL && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex2.get_int_value()].get_real_value() + pop_lex1.get_real_value());
					}
					else if ( type2 == LEX_STR && ( type1 == LEX_STR || type1 == LEX_STRING ))
					{
						pop_lex2.put_type(LEX_STR);
						pop_lex2.put_string_value(TID[pop_lex2.get_int_value()].get_str_value() + pop_lex1.get_string_value());
					}
				}
				else if ( type1 == LEX_ID && type2 != LEX_ID )
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && ( type2 == LEX_INT || type2 == LEX_INT_NUM ))
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() + pop_lex2.get_int_value());
					}
					else if ( type1 == LEX_INT && ( type2 == LEX_REAL || type2 == LEX_REAL_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() + pop_lex2.get_real_value());
					}
					else if ( type1 == LEX_REAL && ( type2 == LEX_INT || type2 == LEX_INT_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() + pop_lex2.get_int_value());
					}
					else if ( type1 == LEX_REAL && ( type2 == LEX_REAL || type2 == LEX_REAL_NUM ))
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() + pop_lex2.get_real_value());
					}
					else if ( type1 == LEX_STR && ( type2 == LEX_STR || type2 == LEX_STRING ))
					{
						pop_lex2.put_type(LEX_STR);
						pop_lex2.put_string_value(TID[pop_lex1.get_int_value()].get_str_value() + pop_lex2.get_string_value());
					}
				}
				else
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if (type2 == LEX_INT && type1 == LEX_INT)
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value( TID[pop_lex2.get_int_value()].get_int_value() + TID[pop_lex1.get_int_value()].get_int_value() );
					}
					else if (type2 == LEX_INT && type1 == LEX_REAL)
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value( TID[pop_lex2.get_int_value()].get_int_value() + TID[pop_lex1.get_int_value()].get_real_value() );
					}
					else if ( type2 == LEX_REAL && type1 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value( TID[pop_lex2.get_int_value()].get_real_value() + TID[pop_lex1.get_int_value()].get_int_value() );
					}
					else if (type2 == LEX_REAL && type1 == LEX_REAL)
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value( TID[pop_lex2.get_int_value()].get_real_value() + TID[pop_lex1.get_int_value()].get_real_value() );
					}
					else if (type2 == LEX_STRING && type1 == LEX_STRING)
					{
						pop_lex2.put_type(LEX_STR);
						pop_lex2.put_string_value( TID[pop_lex2.get_int_value()].get_str_value() + TID[pop_lex1.get_int_value()].get_str_value() );
					}
				}
				args.push(pop_lex2);
				break;





			case LEX_MINUS:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_INT_NUM);
					pop_lex2.put_int_value(  pop_lex1.get_int_value() - pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_int_value() - pop_lex2.get_real_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() - pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() - pop_lex2.get_real_value()  );
				}
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() - pop_lex2.get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() - pop_lex2.get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() - pop_lex2.get_int_value()  );
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() - pop_lex2.get_real_value()  );
					}
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(pop_lex1.get_int_value() - TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_int_value() - TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() - TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() - TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() - TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() - TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() - TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
					{
					    pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() - TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				args.push(pop_lex2);
				break;





			case LEX_TIMES:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_INT_NUM);
					pop_lex2.put_int_value(  pop_lex1.get_int_value() * pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_int_value() * pop_lex2.get_real_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() * pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() * pop_lex2.get_real_value()  );
				}
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() * pop_lex2.get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() * pop_lex2.get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() * pop_lex2.get_int_value()  );
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() * pop_lex2.get_real_value()  );
					}
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(pop_lex1.get_int_value() * TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_int_value() * TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() * TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() * TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() * TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() * TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() * TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
					{
					    pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() * TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				args.push(pop_lex2);
				break;





			case LEX_SLASH:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_INT_NUM);
					pop_lex2.put_int_value(  pop_lex1.get_int_value() / pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_int_value() / pop_lex2.get_real_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() / pop_lex2.get_int_value()  );
				}
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
				{
					pop_lex2.put_type(LEX_REAL_NUM);
					pop_lex2.put_real_value(  pop_lex1.get_real_value() / pop_lex2.get_real_value()  );
				}
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() / pop_lex2.get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() / pop_lex2.get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() / pop_lex2.get_int_value()  );
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(  TID[pop_lex1.get_int_value()].get_real_value() / pop_lex2.get_real_value()  );
					}
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(pop_lex1.get_int_value() / TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_int_value() / TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() / TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(pop_lex1.get_real_value() / TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
					{
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() / TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_int_value() / TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
					{
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() / TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
					{
					    pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_real_value(TID[pop_lex1.get_int_value()].get_real_value() / TID[pop_lex2.get_int_value()].get_real_value());
					}
				}
				args.push(pop_lex2);
				break;





			case LEX_EQ:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (( type2 == LEX_INT || type2 == LEX_INT_NUM ) && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
					pop_lex2.put_int_value(pop_lex2.get_int_value() == pop_lex1.get_int_value());
				else if (( type2 == LEX_INT || type2 == LEX_INT_NUM ) && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
					pop_lex2.put_int_value(pop_lex2.get_int_value() == pop_lex1.get_real_value());
				else if (( type2 == LEX_REAL || type2 == LEX_REAL_NUM ) && ( type1 == LEX_INT || type1 == LEX_INT_NUM ))
					pop_lex2.put_int_value(pop_lex2.get_real_value() == pop_lex1.get_int_value());
				else if (( type2 == LEX_REAL || type2 == LEX_REAL_NUM ) && ( type1 == LEX_REAL || type1 == LEX_REAL_NUM ))
					pop_lex2.put_int_value(pop_lex2.get_real_value() == pop_lex1.get_real_value());
				else if ( type2 == LEX_STR && type1 == LEX_STR )
					pop_lex2.put_int_value(pop_lex2.get_string_value() == pop_lex1.get_string_value());
				else if ( type2 == LEX_ID && type1 != LEX_ID)
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type2 == LEX_INT && type1 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_int_value() == pop_lex1.get_int_value());
					else if ( type2 == LEX_INT && type1 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_int_value() == pop_lex1.get_real_value());
					else if ( type2 == LEX_REAL && type1 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_real_value() == pop_lex1.get_int_value());
					else if ( type2 == LEX_REAL && type1 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_real_value() == pop_lex1.get_real_value());
					else if ( type2 == LEX_STRING && type1 == LEX_STR )
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_str_value() == pop_lex1.get_string_value());
				}
				else if ( type2 != LEX_ID && type1 == LEX_ID )
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() == pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() == pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() == pop_lex2.get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() == pop_lex2.get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() == pop_lex2.get_string_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() == TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() == TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() == TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() == TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() == TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_LSS:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() < pop_lex2.get_int_value()  );
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() < pop_lex2.get_real_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() < pop_lex2.get_int_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() < pop_lex2.get_real_value()  );
				else if (type1 == LEX_STR && type2 == LEX_STR)
					pop_lex2.put_int_value(  pop_lex1.get_string_value() < pop_lex2.get_string_value()  );
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() < pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() < pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() < pop_lex2.get_int_value()  );
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() < pop_lex2.get_real_value()  );
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_str_value() < pop_lex2.get_string_value()  );
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_int_value() < TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
						pop_lex2.put_int_value(pop_lex1.get_int_value() < TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_real_value() < TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(pop_lex1.get_real_value() < TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(pop_lex1.get_string_value() < TID[pop_lex2.get_int_value()].get_str_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() < TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() < TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() < TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() < TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() < TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_LEQ:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() <= pop_lex2.get_int_value()  );
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() <= pop_lex2.get_real_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() <= pop_lex2.get_int_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() <= pop_lex2.get_real_value()  );
				else if (type1 == LEX_STR && type2 == LEX_STR)
					pop_lex2.put_int_value(  pop_lex1.get_string_value() <= pop_lex2.get_string_value()  );
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() <= pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() <= pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() <= pop_lex2.get_int_value()  );
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() <= pop_lex2.get_real_value()  );
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_str_value() <= pop_lex2.get_string_value()  );
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_int_value() <= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
						pop_lex2.put_int_value(pop_lex1.get_int_value() <= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_real_value() <= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(pop_lex1.get_real_value() <= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(pop_lex1.get_string_value() <= TID[pop_lex2.get_int_value()].get_str_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() <= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() <= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() <= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() <= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() <= TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_GTR:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() > pop_lex2.get_int_value()  );
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() > pop_lex2.get_real_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() > pop_lex2.get_int_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() > pop_lex2.get_real_value()  );
				else if (type1 == LEX_STR && type2 == LEX_STR)
					pop_lex2.put_int_value(  pop_lex1.get_string_value() > pop_lex2.get_string_value()  );
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() > pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() > pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() > pop_lex2.get_int_value()  );
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() > pop_lex2.get_real_value()  );
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_str_value() > pop_lex2.get_string_value()  );
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_int_value() > TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
						pop_lex2.put_int_value(pop_lex1.get_int_value() > TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_real_value() > TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(pop_lex1.get_real_value() > TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(pop_lex1.get_string_value() > TID[pop_lex2.get_int_value()].get_str_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() > TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() > TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() > TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() > TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() > TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_GEQ:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() >= pop_lex2.get_int_value()  );
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() >= pop_lex2.get_real_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() >= pop_lex2.get_int_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() >= pop_lex2.get_real_value()  );
				else if (type1 == LEX_STR && type2 == LEX_STR)
					pop_lex2.put_int_value(  pop_lex1.get_string_value() >= pop_lex2.get_string_value()  );
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() >= pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() >= pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() >= pop_lex2.get_int_value()  );
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() >= pop_lex2.get_real_value()  );
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_str_value() >= pop_lex2.get_string_value()  );
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_int_value() >= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
						pop_lex2.put_int_value(pop_lex1.get_int_value() >= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_real_value() >= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(pop_lex1.get_real_value() >= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(pop_lex1.get_string_value() >= TID[pop_lex2.get_int_value()].get_str_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() >= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() >= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() >= TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() >= TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() >= TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_NEQ:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				pop_lex2.put_type(LEX_INT_NUM);
				if (type1 == LEX_INT_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() != pop_lex2.get_int_value()  );
				else if (type1 == LEX_INT_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_int_value() != pop_lex2.get_real_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_INT_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() != pop_lex2.get_int_value()  );
				else if (type1 == LEX_REAL_NUM && type2 == LEX_REAL_NUM)
					pop_lex2.put_int_value(  pop_lex1.get_real_value() != pop_lex2.get_real_value()  );
				else if (type1 == LEX_STR && type2 == LEX_STR)
					pop_lex2.put_int_value(  pop_lex1.get_string_value() != pop_lex2.get_string_value()  );
				else if ( type1 == LEX_ID && type2 != LEX_ID)
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					if ( type1 == LEX_INT && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() != pop_lex2.get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() != pop_lex2.get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() != pop_lex2.get_int_value()  );
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_real_value() != pop_lex2.get_real_value()  );
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(  TID[pop_lex1.get_int_value()].get_str_value() != pop_lex2.get_string_value()  );
				}
				else if ( type1 != LEX_ID && type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if ( type1 == LEX_INT_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_int_value() != TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT_NUM && type2 == LEX_REAL )
						pop_lex2.put_int_value(pop_lex1.get_int_value() != TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL_NUM && type2 == LEX_INT )
						pop_lex2.put_int_value(pop_lex1.get_real_value() != TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL_NUM )
						pop_lex2.put_int_value(pop_lex1.get_real_value() != TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STR )
						pop_lex2.put_int_value(pop_lex1.get_string_value() != TID[pop_lex2.get_int_value()].get_str_value());
				}
				else
				{
					type1 = TID[pop_lex1.get_int_value()].get_type();
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type1 == LEX_INT && type2 == LEX_INT)
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() != TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_INT && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_int_value() != TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_REAL && type2 == LEX_INT )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() != TID[pop_lex2.get_int_value()].get_int_value());
					else if ( type1 == LEX_REAL && type2 == LEX_REAL )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_real_value() != TID[pop_lex2.get_int_value()].get_real_value());
					else if ( type1 == LEX_STRING && type2 == LEX_STRING )
						pop_lex2.put_int_value(TID[pop_lex1.get_int_value()].get_str_value() != TID[pop_lex2.get_int_value()].get_str_value());
				}
				args.push(pop_lex2);
				break;





			case LEX_ASSIGN:

				pop_lex2 = args.pop();
				pop_lex1 = args.pop();
				type2 = pop_lex2.get_type();
				type1 = pop_lex1.get_type();
				type_of_lex IDtype;
				if (type2 == LEX_INT_NUM)
				{
					if (TID[pop_lex1.get_int_value()].get_type() == LEX_INT)
					{
						TID[pop_lex1.get_int_value()].put_int_value(pop_lex2.get_int_value());
						pop_lex2.put_type(LEX_INT_NUM);
					}
					else if (TID[pop_lex1.get_int_value()].get_type() == LEX_REAL)
					{
						TID[pop_lex1.get_int_value()].put_real_value(pop_lex2.get_int_value());
						pop_lex2.put_type(LEX_REAL_NUM);
					}
				}
				else if (type2 == LEX_REAL_NUM)
				{
					TID[pop_lex1.get_int_value()].put_real_value(pop_lex2.get_real_value());
					pop_lex2.put_type(LEX_INT_NUM);
				}
				else if (type2 == LEX_STR)
				{
					TID[pop_lex1.get_int_value()].put_str_value(pop_lex2.get_string_value());
					pop_lex2.put_type(LEX_STR);
				}
				else if ( type2 == LEX_ID )
				{
					IDtype = TID[pop_lex2.get_int_value()].get_type();
					if (IDtype == LEX_INT)
					{
						TID[pop_lex1.get_int_value()].put_int_value(TID[pop_lex2.get_int_value()].get_int_value());
						pop_lex2.put_type(LEX_INT_NUM);
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_int_value());
					}
					else if (IDtype == LEX_REAL)
					{
						TID[pop_lex1.get_int_value()].put_real_value(TID[pop_lex2.get_int_value()].get_real_value());
						pop_lex2.put_type(LEX_REAL_NUM);
						pop_lex2.put_int_value(TID[pop_lex2.get_int_value()].get_real_value());
					}
					else if (IDtype == LEX_STRING)
					{
						TID[pop_lex1.get_int_value()].put_str_value(TID[pop_lex2.get_int_value()].get_str_value());
						pop_lex2.put_type(LEX_STR);
						pop_lex2.put_string_value(TID[pop_lex2.get_int_value()].get_str_value());
					}
				}
				TID[pop_lex1.get_int_value()].put_assign();
				args.push(pop_lex2);
				break;





			case POLIZ_UN_MINUS:

				pop_lex2 = args.pop();
				type2 = pop_lex2.get_type();
				if ( type2 == LEX_INT || type2 == LEX_INT_NUM )
					pop_lex2.put_int_value((-1) * pop_lex2.get_int_value());
				else if ( type2 == LEX_REAL || type2 == LEX_REAL_NUM )
					pop_lex2.put_real_value((-1) * pop_lex2.get_real_value());
				else if ( type2 == LEX_ID )
				{
					type2 = TID[pop_lex2.get_int_value()].get_type();
					if (type2 == LEX_INT) TID[pop_lex2.get_int_value()].put_int_value(   (-1) * TID[pop_lex2.get_int_value()].get_int_value()   );
					else if (type2 == LEX_REAL) TID[pop_lex2.get_int_value()].put_real_value(   (-1) * TID[pop_lex2.get_int_value()].get_real_value()   );
				}
				args.push(pop_lex2);
				break;
			case POLIZ_UN_PLUS:
			//	args.pop();
			case LEX_NULL:
			case LEX_SEMICOLON:
			case LEX_INT:
			case LEX_FOR:
			case LEX_WHILE:
			case LEX_LPAREN:
			case LEX_RPAREN:
			case LEX_EOF:
			case LEX_LBRACE:
			case LEX_RBRACE:
			case LEX_COMMA:
			case LEX_COLON:
			case LEX_FALSE:
			case LEX_TRUE:
			case LEX_DO:
			case LEX_PROGRAM:
			case LEX_ELSE:
			case LEX_IF:
			case LEX_REAL:
			case LEX_STRING:
			case LEX_GOTO:
				break;
		}
		++index;
	}
}
