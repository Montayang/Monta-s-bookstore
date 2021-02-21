#ifndef BOOKSTORE_BOOKSDTORE_H
#define BOOKSTORE_BOOKSDTORE_H

#include <ostream>
#include "blocklist.h"
#include "user.h"

class book {
public:
    char name[60] = "";
    char ISBN[20] = "";
    char author[60] = "";
    char keyword[60] = "";
    double price = 0;
    int quantity = 0;

    book()=default;

    book(const string &ISBN1, const string &name1, const string &author1, const string &keyword1,
         const double &price1,int quantity1);
};

class transaction_record {
public:
    double money=0;
    book operate_book;
    int book_quantity=0;
    user operate_user;
    transaction_record()=default;
    transaction_record(double money1, const book& book1, int book_quantity1, const user& user1);
};

class operation {
public:
    char operate[100] = "";
    char user_name[31] = "";
    char object[31] = "";
    operation() = default;
    operation(const string& operate1, const string& user_name1, const string& object1);

    friend ostream &operator<<(ostream &os, const operation &operation);
};
book the_book(const string& ISBN1);
#endif //BOOKSTORE_BOOKSDTORE_H
