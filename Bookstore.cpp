#include "Bookstore.h"
#include <map>

list user_id_list("user_id_list.dat");
list ISBN_list("ISBN_list.dat");
list name_list("name_list.dat");
list author_list("author_list.dat");
list keyword_list("keyword_list.dat");

book::book(const string& ISBN1,const string& name1,const string& author1,const string& keyword1,const double& price1,int quantity1) : price(price1),quantity(quantity1) {
    strcpy(ISBN,ISBN1.c_str());
    strcpy(name,name1.c_str());
    strcpy(author,author1.c_str());
    strcpy(keyword,keyword1.c_str());
}

book the_book(const string& ISBN1) {
    //find the certain book through its ISBN
    if (ISBN_list.findnode(ISBN1).empty()) throw 1;
    book book1;
    int offset1 = ISBN_list.findnode(ISBN1)[0].offset;
    ifstream fin;
    fin.open("bookdata.dat", ios::in | ios::out | ios::binary);
    fin.seekg(offset1);
    fin.read(reinterpret_cast<char *>(&book1), sizeof(book));
    fin.close();
    return book1;
}

transaction_record::transaction_record(double money1, const book &book1, int book_quantity1, const user &user1) : money(money1),operate_book(book1),book_quantity(book_quantity1),operate_user(user1){}

operation::operation(const string &operate1, const string &user_name1, const string &object1) {
    strcpy(operate,operate1.c_str());
    strcpy(user_name,user_name1.c_str());
    strcpy(object,object1.c_str());
}

ostream &operator<<(ostream &os, const operation &operation) {
    os << operation.user_name <<"\t"<< operation.operate <<"\t"<< operation.object <<endl;
    return os;
}
