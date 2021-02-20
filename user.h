#ifndef BOOKSTORE_USER_H
#define BOOKSTORE_USER_H
#include <iostream>
#include <cstring>
#include <stack>
using namespace std;

class user {
public:
    char user_id[31] = "";
    char passwd[50] = "";
    char name[31] = "";
    int privilege = 1;
    user()=default;
    user(const string& user_id1,const string& passwd1,const string& name1);
};

void su(const string& user_id1,const string& passwd1);
void logout();
void useradd(const string& user_id1,const string& passwd1,int privilege1,const string& name1);
void Register(const string& user_id1,const string& passwd1,const string& name1);
void Delete(const string& user_id1);
void change_passwd(const string& user_id1,const string& old_passwd,const string& new_passwd);
user the_user(const string& user_id1);

#endif //BOOKSTORE_USER_H
