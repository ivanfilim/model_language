//
// Created by ivan on 6/4/25.
//

#include "../include/scanner.h"
#include <string>

#include "../include/Lex.h"
#include "../include/type_of_lex.h"

using namespace std;

bool isalpha (const char c) {
	return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

bool isdigit (const char c) {
	return (c >= '0') && (c <= '9');
}

type_of_lex scanner::words [] = { LEX_NULL, LEX_AND, LEX_DO, LEX_ELSE,
	LEX_IF, LEX_FALSE, LEX_INT, LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_REAL, LEX_TRUE,
	LEX_WHILE, LEX_WRITE, LEX_FOR, LEX_GOTO, LEX_STRING
};

type_of_lex scanner::dlms [] = { LEX_NULL, LEX_SEMICOLON, LEX_LBRACE, LEX_RBRACE, LEX_COMMA, LEX_COLON, LEX_ASSIGN,
	LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH,
	LEX_LEQ, LEX_NEQ, LEX_GEQ
};

string scanner:: TW [ ] = { "nullword", "and","do","else",
	"if","false","int","not","or","program","read", "real", "true", "while","write", "for", "goto", "string", "end_of_array"
};

string scanner:: TD [ ] = { "nulldelim", ";", "{", "}", ",", ":", "=",
	 "(", ")","==","<", ">", "+", "-", "*", "/", "<=", "!=", ">=", "end_of_array"
};

Lex scanner::get_lex () {
	enum state { H, IDENT, INT_NUM, INT_NUMdot, REAL_NUM, STR, ALE, NEQ, MAYBE_COM, COM, END_COM };
	state CS = H;
	string buf;
	int int_part, j;
	double real_num, building_fraction;
	do {
		gc ();
		switch (CS) {
			case H:
				if ( c == ' ' || c == '\n' || c == '\r' || c == '\t') {}
				else if (c == '"') { buf.push_back(c); CS = STR; }
				else if (isalpha(c)) { buf.push_back(c); CS = IDENT; }
				else if (isdigit(c)) { int_part = c - '0'; CS = INT_NUM; }
				else if (c == '=' || c == '<' || c == '>') { buf.push_back(c); CS = ALE; }
				else if (c == '!') { buf.push_back(c); CS = NEQ; }
				else if (c == '/' ) { buf.push_back(c); CS = MAYBE_COM; }
				else {
					buf.push_back(c);
					if ( (j = look(buf, TD)) ) return Lex (dlms[j], j);
					if (c == EOF) return Lex (LEX_EOF);
					throw "Incorrect character!";
				}
				break;
			case IDENT:
				if ( (isalpha(c)) || (isdigit(c)) ) buf.push_back(c);
				else {
					ungetc(c, fp);
					if ( (j = look(buf, TW)) ) return Lex ((type_of_lex) j, j);
					else {
						j = TID.put(buf);
						return Lex (LEX_ID, j);
					}
				}
				break;
			case INT_NUM:
				if (isdigit (c)) {
					int_part = int_part * 10 + (c - '0');
				}
				else if (isalpha(c)) throw "Ошибка: некорректный идентификатор.";
				else if (c == '.') CS = INT_NUMdot;
 				else {
					ungetc(c, fp);
					return Lex (LEX_INT_NUM, int_part);
				}
				break;
			case INT_NUMdot:
				if (isdigit (c)) {
					CS = REAL_NUM;
					building_fraction = 0.1;
					real_num = int_part;
					real_num = real_num + building_fraction * (c - '0');
				}
				else throw "Ошибка: некорректная запись вещественного числа.";
				break;
			case REAL_NUM:
				if (isdigit (c)) {
					building_fraction = building_fraction / 10;
					real_num = real_num + building_fraction * (c - '0');
				}
				else if (c == '.') throw "Лишняя точка!";
 				else {
					ungetc(c, fp);
					return Lex (LEX_REAL_NUM, 0, real_num );
				}
				break;
			case STR:
				if (c == '"') { buf.push_back(c); return Lex (LEX_STR, 0, 0, buf); }
				else if (c == EOF) throw "Ошибка: не хватает закрывающих кавычек.";
				else buf.push_back(c);
				break;
			case ALE:
				if ( c == '=' ) buf.push_back(c);
				else ungetc(c, fp);
				j = look (buf, TD);
				return Lex ((type_of_lex) dlms[j], j );
			case NEQ:
				if (c == '=') {
					buf.push_back(c);
					j = look ( buf, TD );
					return Lex ( LEX_NEQ, j );
				}
				else throw "Ошибка при использовании символа '!'.";
				break;
			case MAYBE_COM:
				if (c != '*') {
					ungetc(c, fp);
					j = look(buf, TD);
					return Lex ((type_of_lex) dlms[j], j);
				}
				else { CS = COM; buf.clear(); }
				break;
			case COM:
				if (c == '*') { CS = END_COM; break; }
			case END_COM:
				if (c == '/') CS = H;
		}
	}
	while (true);
}