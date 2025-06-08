//
// Created by ivan on 6/8/25.
//

#include"string"

#ifndef POLIZ_H
#define POLIZ_H

using namespace std;

//                                                   ТАБЛИЦА МЕТОК

struct tabl_m {
	struct label_without_goto
	{
		string name;
		int place; // номер в полизе, который помечает эта метка
	};
	struct label_with_goto
	{
		string name;
		int place; // номер в полизе, по которому располагается эта метка
	};

	label_without_goto * labels;
	label_with_goto * labels_near_goto;
	int size;
	int top, goto_list_top;

	explicit tabl_m (const int max_size)
	{
		labels = new label_without_goto[size = max_size];
		labels_near_goto = new label_with_goto[size];
		top = 0;
		goto_list_top = 0;
	}
	//~tabl_m() { delete []metki; delete []metki_near_goto; }
	//string& operator[] (int k) { return names[k]; }
	void put_NO_goto (const string buf, int num);
	void put_in_goto_list (const string buf, int num);
	void check () const;
};

inline void tabl_m::put_NO_goto (const string buf, int num)
{
	for (int j = 0; j < top; j++)
		if (buf == labels[j].name) throw "Ошибка! Одна и та же метка помечает разные точки в тексте программы.";
	labels[top].name = buf;
	labels[top].place = num;
	++top;
};

inline void tabl_m::put_in_goto_list (const string buf, int num)
{
	for (int j = 0; j < goto_list_top; j++)
		if (labels_near_goto[j].name == buf) return;
	labels_near_goto[goto_list_top].name = buf;
	labels_near_goto[goto_list_top].place = num;
	++goto_list_top;
};

inline void tabl_m::check () const {
	for (int i = 0; i < goto_list_top; ++i)
	{
		int j;
		for (j = 0; j < top; ++j)
		{
			if (labels[j].name == labels_near_goto[i].name) break;
		}
		if (j == top) throw "Ошибка! Встретилась неописанная метка.";
	}
}

inline tabl_m TM (100);

//                                                   ТАБЛИЦА МЕТОК

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 2 этап


class Poliz
{
	Lex * p;
	int size;
	int free;
public:
	explicit Poliz(const int max_size )
	{
		p = new Lex [size = max_size];
	    free = 0;
	}
	~Poliz() { delete []p; };
	void put_lex(Lex l) { p[free] = l; ++free; };
	void put_lex(Lex l, int place) const { p[place] = l; };
	void blank() { ++free; };
	int get_free() const { return free; };
	Lex& operator[] ( int index ) const {
		if (index > size) throw "POLIZ:out of array";
		if ( index > free )
			throw "POLIZ:indefinite element of array";
		return p[index];
	}
	void print()
	{
		for ( int i = 0; i < free; ++i )
		cout << i << ")  " << p[i];
	}
	void write_labels(const tabl_m &TM) const {
		for (int i = 0; i < TM.goto_list_top; ++i)
		{
			for (int j = 0; j < TM.top; ++j)
				if ((TM.labels)[j].name == (TM.labels_near_goto)[i].name) {

					Lex label (POLIZ_LABEL, TM.labels[j].place);
					p[TM.labels_near_goto[i].place] = label;

					break;
				}
		}
	}
};

#endif //POLIZ_H
