#ifndef BOOKSTORE_COMMAND_H
#define BOOKSTORE_COMMAND_H
#include "Bookstore.h"

void select(const string& ISBN1);
void modify(int offset1,const string& ISBN1, const string& name1, const string& author1, const string& keyword1, double price1);
void import(int offset1,int quantity1,double cost_price);
void show();
void show(const string& parameter,string type);
void show_finance(int time);
void buy(const string& ISBN1,int quantity1);

#endif //BOOKSTORE_COMMAND_H
