#include <iostream>
#include "../include/Lex.h"
#include "../include/table_ident.h"
#include "../include/ident.h"
#include "../include/scanner.h"

using namespace std;

Lex null_lex; // helper object of the class Lex; понадобится нам для проверки условий при реализации процедур рс-метода


template < class T, int max_size > 
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

template <class T, int max_size >
void Stack <T, max_size >::push(T i) {
	if ( !is_full() )
	{
		s[top] = i;
		++top;
	}	
	else throw "Stack_is_full";
}

template <class T, int max_size >
T Stack <T, max_size >::pop() {
	if (!is_empty())
	{
		--top;
		return s[top];
	}
	else throw "Stack_is_empty";
}


//                                                   ТАБЛИЦА МЕТОК

struct tabl_m {
	struct metka_without_goto
	{
		string name;
		int place; // номер в полизе, который помечает эта метка
	};
	struct metka_with_goto 
	{
		string name;
		int place; // номер в полизе, по которому располагается эта метка
	};
	
	metka_without_goto * metki;
	metka_with_goto * metki_near_goto;
	int size;
	int top, goto_list_top;

	tabl_m (int max_size) 
	{
		metki = new metka_without_goto[size = max_size];
		metki_near_goto = new metka_with_goto[size]; 
		top = 0;
		goto_list_top = 0;
	}
	//~tabl_m() { delete []metki; delete []metki_near_goto; }
	//string& operator[] (int k) { return names[k]; }
	void put_NO_goto (const string buf, int num);
	void put_in_goto_list (const string buf, int num);
	void check ();
};

void tabl_m::put_NO_goto (const string buf, int num) 
{
	for (int j = 0; j < top; j++)
		if (buf == metki[j].name) throw "Ошибка! Одна и та же метка помечает разные точки в тексте программы.";
	metki[top].name = buf;
	metki[top].place = num;
	++top;	
};

void tabl_m::put_in_goto_list (const string buf, int num)
{
	for (int j = 0; j < goto_list_top; j++)
		if (metki_near_goto[j].name == buf) return;
	metki_near_goto[goto_list_top].name = buf;
	metki_near_goto[goto_list_top].place = num;
	++goto_list_top;
};

void tabl_m::check ()
{
	for (int i = 0; i < goto_list_top; ++i)
	{
		int j;
		for (j = 0; j < top; ++j)
		{
			if (metki[j].name == metki_near_goto[i].name) break;
		}
		if (j == top) throw "Ошибка! Встретилась неописанная метка.";
	}
}

tabl_m TM (100); 

//                                                   ТАБЛИЦА МЕТОК

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 2 этап   

 
class Poliz
{
	Lex * p;
	int size;
	int free;
public:
	Poliz ( int max_size )
	{
		p = new Lex [size = max_size];
	    free = 0;
	}
	~Poliz() { delete []p; };
	void put_lex(Lex l) { p[free] = l; ++free; };
	void put_lex(Lex l, int place) { p[place] = l; };
	void blank() { ++free; };
	int get_free() { return free; };
	Lex& operator[] ( int index )
	{
		if (index > size) throw "POLIZ:out of array";	
		else
		if ( index > free )
		throw "POLIZ:indefinite element of array";
		else return p[index];
	}
	void print()
	{
		for ( int i = 0; i < free; ++i )
		cout << i << ")  " << p[i];
	}
	void write_metki (tabl_m TM)
	{
		for (int i = 0; i < TM.goto_list_top; ++i)
		{
			for (int j = 0; j < TM.top; ++j)
				if ((TM.metki)[j].name == (TM.metki_near_goto)[i].name) {
					
					Lex label (POLIZ_LABEL, TM.metki[j].place);
					p[TM.metki_near_goto[i].place] = label; 
					
					break;
				}
		}
	} 
};
 
 
class Parser
{
	Lex curr_lex; 
	type_of_lex c_type, str_int_or_real;
	
	int int_value;     // значения полей 
	double real_value; // только что считан-
	string str_value;  // ной лексемы.
	
	int num_in_TID; // вспомогательная переменная для ИНИЦИАЛИЗАЦИИ переменных в описаниях; так же она будет использоваться в рс-методе
	
	int ID_FLAG = 0; // вспомогательная переменная для применимости метода рекурсивного спуска
	
	Lex buf; // вспомогательный объект класса Lex, понадобится нам при реализации процедур рс-метода (будет буфером, в котором хранится считанный на предыдущем шаге идентификатор)
	Lex identifier; // аналогично
	
	int IE_pl0, IE_pl1, IE_pl2, IE_pl3, IE_pl4, IE_rem_for_pl0, IE_rem_for_pl1, IE_rem_for_pl2, IE_rem_for_pl3, IE_rem_for_pl4,
		FOR_pl0, FOR_pl1, FOR_pl2, FOR_pl3, FOR_pl4, FOR_rem_for_pl0, FOR_rem_for_pl1, FOR_rem_for_pl2, FOR_rem_for_pl3, FOR_rem_for_pl4,
		WH_pl0, WH_pl1, WH_pl2, WH_pl3, WH_pl4, WH_rem_for_pl0, WH_rem_for_pl1, WH_rem_for_pl2, WH_rem_for_pl3, WH_rem_for_pl4;

	scanner scan;
	
	Stack < int, 100 > st_int;
	Stack < type_of_lex, 100 > st_lex;
	
	// процедуры РС-метода:
	void PROGRAM(); void DESCRIPTIONS(); void ONE_DESCRIPTION(); void OPERATORS(); void ONE_OPERATOR(); void VARIABLE(); void CONST(); void EXPRESSION();
	void EXPR_OR(); void EXPR_AND(); void EXPR_PM(); void EXPR_MS(); void EXPR_RELAT_OP(); void EXPR_UN(); void EXPR();
		
	// семантичиеские действия:
	void dec (type_of_lex type); void check_id (); void check_op (); 
	void check_not (); void eq_type (); 
	
	void gl () { // получить очередную лексему
		curr_lex = scan.get_lex();
		c_type = curr_lex.get_type();
		int_value = curr_lex.get_int_value();
		real_value = curr_lex.get_real_value();
		str_value = curr_lex.get_string_value();
	}
public:
	Poliz prog;
	Parser (const char *program) : scan (program), prog (1000){}
	void analyze(); // анализатор с действиями
		void f (int a, int b)
		{
			ONE_OPERATOR();
			prog.put_lex(Lex(POLIZ_LABEL, a));
			prog.put_lex(POLIZ_GO);
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), b);
		}
};
 
void Parser::analyze () {
	gl ();
	PROGRAM ();
	TM.check();
	prog.write_metki(TM);
	//prog.print();
	//cout << endl << "Программа корректна :) :) :) !" << endl;
}

void Parser::PROGRAM ()
{
	if (c_type == LEX_PROGRAM) gl();
	else throw "Ошибка! Программа на модельном языке должна начинаться со слова program.";
	if (c_type == LEX_LBRACE) gl();
	else throw "Ошибка! После слова program должна идти открывающая фигурная скобка.";
	DESCRIPTIONS();
	OPERATORS();
	if (c_type == LEX_RBRACE) gl();
	else throw "Ошибка! Некорректная программа.";
	if (c_type != LEX_EOF) throw "Ошибка! После тела программы не должно ничего идти.";
}

void Parser::DESCRIPTIONS ()
{
	while (c_type == LEX_INT || c_type == LEX_REAL || c_type == LEX_STRING)
	{
		ONE_DESCRIPTION();
		if (c_type == LEX_SEMICOLON) gl();
		else throw "Ошибка! После описания должна идти точка с запятой.";
	}
}

void Parser::ONE_DESCRIPTION ()
{
	str_int_or_real = c_type;
	gl(); // текущая лексема равна либо int, либо real, либо string - считываем её
	st_int.reset();
	VARIABLE();
	while (c_type == LEX_COMMA) 
	{
		gl();
		VARIABLE();
	}
	dec(str_int_or_real);
}

void Parser::VARIABLE ()
{
	if (c_type == LEX_ID) 
	{
		st_int.push (int_value);
		num_in_TID = int_value;
		gl();
	}
	else throw "Ошибка! В описании ожидался идентификатор.";
	if (c_type != LEX_ASSIGN) return;
	else 
	{ 
		gl(); 
		CONST(); 
	} 
}

void Parser::CONST () 
{
	int sign = 1;
	if (c_type == LEX_STR) 
	{ 
		if (str_int_or_real != LEX_STRING) throw "Ошибка! Попытка инициализировать переменную типа int или real строковой константой.";
		TID[num_in_TID].put_str_value(str_value);
		TID[num_in_TID].put_assign();
		gl(); 
		return; 
	}
	else if ((c_type == LEX_PLUS) || (c_type == LEX_MINUS)) 
	{
		if (c_type == LEX_MINUS) sign = -1; 
		gl(); 
	} 
	if (c_type == LEX_INT_NUM)
	{
		if (str_int_or_real == LEX_STRING) throw "Ошибка! Попытка инициализировать переменную типа string числовой константой.";
		else if (str_int_or_real == LEX_INT) TID[num_in_TID].put_int_value(sign * int_value);
		else if (str_int_or_real == LEX_REAL) TID[num_in_TID].put_real_value(sign * int_value);
		TID[num_in_TID].put_assign(); 
		gl();
	}
	else if (c_type == LEX_REAL_NUM) 
	{
		if (str_int_or_real != LEX_REAL) throw "Ошибка! Попытка инициализировать вещественной константой переменную не типа real.";
		TID[num_in_TID].put_real_value(sign * real_value);
		TID[num_in_TID].put_assign();
		gl();
	}
	else throw "Ошибка! Встретилось некорректное описание.";
}

void Parser::OPERATORS ()
{
	while ((c_type == LEX_IF) || (c_type == LEX_WHILE) || (c_type == LEX_READ) || (c_type == LEX_WRITE) || (c_type == LEX_LBRACE) || (c_type == LEX_ID) || (c_type == LEX_REAL) ||
	(c_type == LEX_NOT) || (c_type == LEX_PLUS) || (c_type == LEX_MINUS) || (c_type == LEX_FOR) || (c_type == LEX_GOTO) || (c_type == LEX_LPAREN) || (c_type == LEX_INT_NUM) || (c_type == LEX_STR))
	{ 
		ONE_OPERATOR();  
	}
}

void Parser::ONE_OPERATOR ()
{
	// IF-ELSE
	if (c_type == LEX_IF) 
	{
		gl();
		if (c_type == LEX_LPAREN) gl(); 
		else throw "Ошибка! После служебного слова if ожидалась открывающая круглая скобка.";
		EXPRESSION();
		
		type_of_lex expression_type = st_lex.pop(); 
		if (expression_type != LEX_INT && expression_type != LEX_INT_NUM) throw "Ошибка! В скобочках после оператора if может быть только целочисленное выражение.";
		
		if (c_type == LEX_RPAREN) gl();
		else throw "Ошибка! Вы забыли поставить закрывающую круглую скобку в операторе if.";
		IE_pl2 = prog.get_free(); prog.blank(); prog.put_lex(Lex (POLIZ_FGO));
		ONE_OPERATOR();
		IE_pl3 = prog.get_free(); prog.blank(); prog.put_lex(Lex (POLIZ_GO)); prog.put_lex (Lex (POLIZ_LABEL, prog.get_free() ), IE_pl2);
		if (c_type == LEX_ELSE) gl();
		else throw "Ошибка! В операторе if отсутствует else-часть.";
		ONE_OPERATOR();
		prog.put_lex ( Lex (POLIZ_LABEL, prog.get_free()), IE_pl3);
	}
	// WHILE
	else if (c_type == LEX_WHILE) 
	{
		gl();
		if (c_type == LEX_LPAREN) gl(); 
		else throw "Ошибка! После служебного слова while ожидалась открывающая круглая скобка.";
		
		WH_pl0 = prog.get_free();
		
		EXPRESSION();
		
		WH_rem_for_pl1 = prog.get_free();
		prog.blank();
		prog.put_lex(POLIZ_FGO);
		
		if (c_type == LEX_RPAREN) gl();
		else throw "Ошибка! Ожидалась закрывабщая скобка в описании цикла while.";
		
		ONE_OPERATOR();
		
		prog.put_lex(Lex (POLIZ_LABEL, WH_pl0));
		prog.put_lex(POLIZ_GO);
		
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), WH_rem_for_pl1);
	}	 
	// READ
	else if (c_type == LEX_READ) 
	{
		gl();
		if (c_type == LEX_LPAREN) gl();
		else throw "Ошибка! После служебного слова read ожидалась открывающая круглая скобка.";
		if (c_type == LEX_ID) 
		{
			check_id();
			prog.put_lex(Lex (POLIZ_ADDRESS, int_value));
			prog.put_lex(Lex (LEX_READ));
			gl();
		} 
		else throw "Ошибка! В качестве параметра оператора read может выступать только идентификатор.";
		if (c_type == LEX_RPAREN) gl();
		else throw "Ошибка! После служебного слова read ожидался идентификатор, заключенный в круглые скобки.";
		if (c_type == LEX_SEMICOLON) { prog.put_lex(curr_lex); gl(); } 
		else throw "Ошибка! Вы забыли поставить точку с запятой после оператора read.";
	}
	// WRITE
	else if (c_type == LEX_WRITE) 
	{
		gl();
		if (c_type == LEX_LPAREN) gl(); 
		else throw "Ошибка! После служебного слова write ожидалась открывающая круглая скобка.";
		EXPRESSION();
		while (c_type == LEX_COMMA)
		{
			gl();
			EXPRESSION();
		}
		if (c_type == LEX_RPAREN) gl(); 
		else throw curr_lex;
		if (c_type == LEX_SEMICOLON) gl(); 
		else throw "Ошибка! Вы забыли поставить точку с запятой после оператора write.";
		prog.put_lex(LEX_WRITE);
	}	 	 
	// СОСТАВНОЙ ОПЕРАТОР
	else if (c_type == LEX_LBRACE) 
	{
		gl();
		OPERATORS();
		if (c_type == LEX_RBRACE) gl();
		else throw curr_lex;
	}
	// FOR
	else if (c_type == LEX_FOR) 
	{	
		gl();
		if (c_type == LEX_LPAREN) gl(); 
		else throw curr_lex;
		if (c_type != LEX_SEMICOLON) EXPRESSION();
		
		if (c_type == LEX_SEMICOLON) { prog.put_lex(LEX_SEMICOLON); gl(); }
		else throw curr_lex;
		
		FOR_rem_for_pl3 = prog.get_free();
		
		if (c_type != LEX_SEMICOLON)
		{
			EXPRESSION();
			type_of_lex expression_type = st_lex.pop();
		    if (expression_type != LEX_INT && expression_type != LEX_INT_NUM) throw "Ошибка! В качестве условия завершения цикла может использоваться только целочисленное выражение.";
	    }
	    
		if (c_type == LEX_SEMICOLON) { prog.put_lex(LEX_SEMICOLON); gl(); }
		else throw "Ошибка! Ожидалась запятая в описании цикла for.";
	    
	    FOR_pl1 = prog.get_free(); 
	    prog.blank();
	    prog.put_lex(Lex (POLIZ_FGO));
	    FOR_pl2 = prog.get_free();
	    prog.blank();
	    prog.put_lex(Lex (POLIZ_GO));
		
		FOR_rem_for_pl4 = prog.get_free();
		
		if (c_type != LEX_RPAREN) EXPRESSION();
		if (c_type == LEX_RPAREN) gl();
		else throw "Ошибка! Ожидалась закрывающая скобка в описании цикла for.";
		
		prog.put_lex(Lex(POLIZ_LABEL, FOR_rem_for_pl3));
		prog.put_lex(POLIZ_GO);
		
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), FOR_pl2);
		
		//
		f(FOR_rem_for_pl4, FOR_pl1);
		//
		
		//

		//
		
		/*ONE_OPERATOR();
		
		 prog.put_lex(Lex(POLIZ_LABEL, FOR_rem_for_pl4));
		prog.put_lex(POLIZ_GO);
		
		prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), FOR_pl1); */
		
	}		
	// GOTO
	else if (c_type == LEX_GOTO) 
	{
		gl();
		if (c_type == LEX_ID) { TM.put_in_goto_list(TID[int_value].get_name(), prog.get_free()); gl(); } 
		else throw "Ошибка! После goto должен идти идентификатор.";
		if (c_type == LEX_SEMICOLON) gl();
		else throw "Ошибка! Вы забыли поставить точку с запятой после оператора GOTO.";
		prog.blank();
		prog.put_lex(Lex(POLIZ_GO));
	}
	// LEX_ID
	else if (c_type == LEX_ID) 
	{
		ID_FLAG = 1;
		
		identifier = curr_lex;
		num_in_TID = int_value;
		
		gl();
		if (c_type == LEX_COLON)
		{
			gl();	
			TM.put_NO_goto(TID[num_in_TID].get_name(), prog.get_free()); // кладем ИМЯ (значение типа string) метки в таблицу меток, которые стоят слева от двоеточия
			ID_FLAG = 0;
			ONE_OPERATOR ();
		}
		else
		{	
			int help = int_value;    //  эти действия мы выполняем для того, чтобы проверить,
			int_value = num_in_TID;  //  был ли описан идентификатор (который уже не
			check_id();	             //  является текущей лексемой, а был
			int_value = help;        //  текущей лексемой на предыдущем шаге)
			
			
			EXPRESSION();
			if (c_type == LEX_SEMICOLON) { prog.put_lex(c_type); gl(); }
			else if (c_type != LEX_ASSIGN) throw "Ошибка! Некорректная программа.";
			else { gl(); EXPRESSION(); prog.put_lex(Lex(LEX_ASSIGN)); }
		} 
	}
	// OPERATOR_EXPRESSION (1st is not LEX_ID)
	else if (c_type == LEX_INT_NUM || c_type == LEX_REAL_NUM || c_type == LEX_STR || c_type == LEX_LPAREN || c_type == LEX_NOT || c_type == LEX_PLUS || c_type == LEX_MINUS)
	{
		EXPRESSION();
		if (c_type == LEX_SEMICOLON) { prog.put_lex(curr_lex); gl(); }
		else throw "Ошибка! Некорректная программа.";
	} 
} 

void Parser::EXPRESSION () 
{
	if (ID_FLAG == 0)
	{
		if (c_type == LEX_ID) 
		{
			check_id(); // проверили, описан ли данный идентификатор
			buf = curr_lex;
			gl();
			if (c_type == LEX_ASSIGN)
			{
				prog.put_lex(Lex (POLIZ_ADDRESS, buf.get_int_value())); // buf.get_int_value() - номер в таблице идентификаторов
				st_lex.push(c_type); 
				gl();
				buf = null_lex;
				EXPRESSION();
				check_op();
			}
			else if (c_type != LEX_EOF) EXPR_OR();
			else throw "Ошибка! Неправильная структура программы.";
		}
		else EXPR_OR();
	}
	else
	{
		buf = identifier;
		if (c_type == LEX_ASSIGN)
		{
			prog.put_lex(Lex(POLIZ_ADDRESS, num_in_TID)); // кладем считанный на предыдущем шаге идентификатор-ссылку в ПОЛИЗ-массив
			st_lex.push(c_type); 
			gl();
			identifier = null_lex;
			buf = null_lex;
			ID_FLAG = 0;
			EXPRESSION();
			check_op();
		}
		else if (c_type != LEX_EOF) EXPR_OR(); 
		else throw "Ошибка! Неправильная структура программы!";
	}
}

void Parser::EXPR_OR () 
{
	EXPR_AND(); 
	while (c_type == LEX_OR)
	{
		st_lex.push(c_type);
		gl();
		buf = null_lex;	
		EXPR_AND();
		check_op();
	}
}

void Parser::EXPR_AND () 
{
	EXPR_RELAT_OP();
	while (c_type == LEX_AND)
	{
		st_lex.push(c_type);
		gl();
		buf = null_lex;
		EXPR_RELAT_OP();
		check_op();
	}
}

void Parser::EXPR_RELAT_OP () 
{
	EXPR_PM();
	while (c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_GEQ || c_type == LEX_LEQ || c_type == LEX_EQ || c_type == LEX_NEQ) 
	{
		st_lex.push(c_type);
		gl();
		buf = null_lex;
		EXPR_PM();
		check_op();
	}
}

void Parser::EXPR_PM () 
{
	EXPR_MS();
	while (c_type == LEX_PLUS || c_type == LEX_MINUS)
	{
		st_lex.push(c_type);
		gl();
		buf = null_lex;
		EXPR_MS();
		check_op();	
	}
}

void Parser::EXPR_MS () 
{
	EXPR_UN();
	while (c_type == LEX_TIMES || c_type == LEX_SLASH) 
	{
		st_lex.push(c_type);
		gl();
		buf = null_lex;
		EXPR_UN();
		check_op();
	}
}

void Parser::EXPR_UN () 
{	
	int rem_type = c_type, un_flag = 0;
	if (buf.get_type() == LEX_NULL) 
	{ 
		if (c_type == LEX_NOT || c_type == LEX_PLUS || c_type == LEX_MINUS) { gl(); un_flag = 1; } 
	}
	EXPR();
	if (rem_type == LEX_NOT) check_not();
	else if (rem_type == LEX_PLUS && un_flag == 1) prog.put_lex(Lex (POLIZ_UN_PLUS));
	else if (rem_type == LEX_MINUS && un_flag == 1) prog.put_lex(Lex (POLIZ_UN_MINUS));
}

void Parser::EXPR() 
{
	if (buf.get_type() != LEX_NULL) 
	{
		prog.put_lex(buf);
		buf = null_lex;
	}
	else if (c_type == LEX_ID || c_type == LEX_INT_NUM || c_type == LEX_REAL_NUM || c_type == LEX_STR) 
	{
		if (c_type == LEX_ID) check_id();
		else st_lex.push(c_type);
		prog.put_lex(curr_lex);
		gl();
	}
	else if (c_type == LEX_LPAREN)
	{
		gl();
		EXPRESSION();
		if (c_type == LEX_RPAREN) gl();
		else throw curr_lex;
	}
	else return;
}

void Parser::dec (type_of_lex type)
{
	int i;
	while ( !st_int.is_empty() )
	{
		i = st_int.pop();
		if ( TID[i].get_declare() ) throw "Ошибка! В программе встретилась дважды описанная переменная.";
		else
		{
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
}

void Parser::check_id ()
{
	if (TID[int_value].get_declare()) st_lex.push(TID[int_value].get_type());
	else throw "Ошибка! В программе встретился не описанный идентификатор.";
}

void Parser::check_not ()
{
	type_of_lex t = st_lex.pop();
	if (t != LEX_INT && t != LEX_INT_NUM)
	throw "Ошибка! Операндом унарной операции not может быть только целочисленное выражение.";
	else
	{
		st_lex.push (LEX_INT_NUM);
		prog.put_lex (Lex (LEX_NOT) );
	}
}

void Parser::check_op ()
{
	type_of_lex t1, t2, op, result_type;
	t2 = st_lex.pop(); 
	op = st_lex.pop(); 
	t1 = st_lex.pop(); 
	if ((t1 == LEX_INT && t2 == LEX_INT_NUM) || (t1 == LEX_INT_NUM && t2 == LEX_INT) || (t1 == LEX_INT && t2 == LEX_INT) || (t1 == LEX_INT_NUM && t2 == LEX_INT_NUM)) result_type = LEX_INT;
	else if (((t1 == LEX_INT || t1 == LEX_INT_NUM) && (t2 == LEX_REAL || t2 == LEX_REAL_NUM)) || ((t1 == LEX_REAL || t1 == LEX_REAL_NUM) && (t2 == LEX_INT || t2 == LEX_INT_NUM))
	||  ((t1 == LEX_REAL || t1 == LEX_REAL_NUM) && (t2 == LEX_REAL || t2 == LEX_REAL_NUM))) result_type = LEX_REAL;
	else if ((t1 == LEX_STRING && t2 == LEX_STRING) || (t1 == LEX_STR && t2 == LEX_STR) || (t1 == LEX_STR && t2 == LEX_STRING) || (t1 == LEX_STRING && t2 == LEX_STR)) result_type = LEX_STRING;
	else throw "Ошибка! Несогласованы типы операндов в выражении."; 
	if (result_type == LEX_STRING && op != LEX_PLUS && op != LEX_GTR && op != LEX_GEQ && op != LEX_LSS && op != LEX_LEQ && op != LEX_EQ && op != LEX_NEQ) 
	throw "Ошибка! Некорректная операция для операндов типа string.	";
	else if (t1 == LEX_INT && t2 == LEX_REAL && op == LEX_ASSIGN) throw "Ошибка! Попытка присвоить переменной типа int вещественное значние.";
	else if ((t1 == LEX_INT_NUM || t1 == LEX_REAL_NUM || t1 == LEX_STR) && (op == LEX_ASSIGN)) throw "Ошибка! Слева от оператора приваивания может находиться только идентификатор.";
	if (op == LEX_EQ || op == LEX_NEQ || op == LEX_GEQ || op == LEX_GTR || op == LEX_LSS || op == LEX_LEQ) result_type = LEX_INT; 
	if ((op == LEX_OR || op == LEX_AND) && ((t1 != LEX_INT && t1 != LEX_INT_NUM) || (t2 != LEX_INT && t2 != LEX_INT_NUM))) throw "Ошибка! Операнды бинарных операций or и and могут быть только целочисленными.";
	st_lex.push(result_type);
	prog.put_lex (Lex (op) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 2 etap

class Executer
{
	Lex pc_el;
public:
	void execute ( Poliz & prog );
};


void Executer::execute ( Poliz & prog )
{
	int int_num;
	double real_num;
	string str;
	type_of_lex type2, type1;
	Stack < Lex, 100 > args;
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
				else throw "Ошибка! Неинициализированная переменная.";
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

class Interpreter
{
	Parser parser;
	Executer executor;
public:
	explicit Interpreter (const char* program): parser (program) {}

	void interpretation ();
};

void Interpreter::interpretation ()
{
	parser.analyze ();
	executor.execute ( parser.prog );
}



int main(const int argc, char * argv[])
{
	if (argc != 2)
	{
		cout << "Error: expected 2 cmd arguments, but received " << argc << " instead."  << endl;
		return 1;
	}
	try
	{
		Interpreter I ( argv[1] );
		I.interpretation();
		return 0;
	}
	catch(char c)
	{
		cout << "unexpected symbol " << c << endl;
		return 1;
	}
	catch (Lex l)
	{
		cout << "unexpected lexeme";
		cout << l;
		return 1;
	}
	catch (const char *source)
	{
		cout << source << endl;
		return 1;
	}
}
