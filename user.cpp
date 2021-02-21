#include "user.h"
#include "Bookstore.h"
#include <stack>
#include <fstream>

extern list user_id_list;
extern list ISBN_list;
extern list name_list;
extern list author_list;
extern list keyword_list;

stack<pair<string, int>> users_online;//the second value of the pair is the offset of the selected book

user::user(const string &user_id1, const string &passwd1, const string &name1) {
    strcpy(user_id, user_id1.c_str());
    strcpy(passwd, passwd1.c_str());
    strcpy(name, name1.c_str());
}

void su(const string &user_id1, const string &passwd1) {
    user user1 = the_user(user_id1);
    if (strcmp(passwd1.c_str(), user1.passwd) == 0) {
        users_online.push(make_pair(user1.user_id, -1));
    } else throw 1;
}

void logout() {
    if (users_online.empty()) throw 1;
    users_online.pop();
    //pop the users under the logouting user which have been deleted from the stack
//    while (user_id_list.findnode(users_online.top().first).empty()) {
//        users_online.pop();
//    }
}

void useradd(const string &user_id1, const string &passwd1, int privilege1, const string &name1) {
    if (users_online.empty() || privilege1 >= the_user(users_online.top().first).privilege) throw 1;
    if (!user_id_list.findnode(user_id1).empty()) throw 1;
    //add the user's data into the userdata.dat
    user user1(user_id1, passwd1, name1);
    user1.privilege = privilege1;
    ofstream fout;
    fout.open("userdata.dat", ios::ate | ios::in | ios::out | ios::binary);
    int offset1 = fout.tellp();
    fout.write(reinterpret_cast<char *>(&user1), sizeof(user));
    //add the node into the user_id_list
    fout.close();
    node node1(user_id1, offset1);
    user_id_list.addnode(node1);
    //record the operation
    fout.open("operation.dat", ios::ate | ios::in | ios::out | ios::binary);
    operation operation1("add the user",the_user(users_online.top().first).name,name1);
    fout.write(reinterpret_cast<char *>(&operation1), sizeof(operation));
    fout.close();
}

void Register(const string &user_id1, const string &passwd1, const string &name1) {
    if (!user_id_list.findnode(user_id1).empty()) throw 1;
    //add the user's data into the userdata.dat
    user user1(user_id1, passwd1, name1);
    ofstream fout;
    fout.open("userdata.dat", ios::ate | ios::in | ios::out | ios::binary);
    int offset1 = fout.tellp();
    fout.write(reinterpret_cast<char *>(&user1), sizeof(user));
    //add the node into the user_id_list
    fout.close();
    node node1(user_id1, offset1);
    user_id_list.addnode(node1);
    //record the operation
    fout.open("operation.dat", ios::ate | ios::in | ios::out | ios::binary);
    operation operation1("register the user","the tourist",name1);
    fout.write(reinterpret_cast<char *>(&operation1), sizeof(operation));
    fout.close();
}

void Delete(const string &user_id1) {
    if (users_online.empty()) throw 1;
    if (user_id_list.findnode(user_id1).empty() || the_user(users_online.top().first).privilege != 7) throw 1;
    if (strcmp(user_id1.c_str(),"root") == 0) throw 1;
    user_id_list.dltnode(user_id_list.findnode(user_id1)[0]);
    //record the operation
    ofstream fout;
    fout.open("operation.dat", ios::ate | ios::in | ios::out | ios::binary);
    operation operation1("delete the user",the_user(users_online.top().first).name,the_user(user_id1).name);
    fout.write(reinterpret_cast<char *>(&operation1), sizeof(operation));
    fout.close();
}

void change_passwd(const string &user_id1, const string &old_passwd, const string &new_passwd) {
    if (user_id_list.findnode(user_id1).empty()) throw 1;
    user user1 = the_user(user_id1);
    if (strcmp(old_passwd.c_str(), user1.passwd) == 0) {
        strcpy(user1.passwd, new_passwd.c_str());
    } else throw 1;
    ofstream fout;
    int offset1 = user_id_list.findnode(user_id1)[0].offset;
    fout.open("userdata.dat", ios::in | ios::out | ios::binary);
    fout.seekp(offset1);
    fout.write(reinterpret_cast<char *>(&user1), sizeof(user));
    fout.close();
    //record the operation
    fout.open("operation.dat", ios::ate | ios::in | ios::out | ios::binary);
    operation operation1("change the user's password",the_user(users_online.top().first).name,user1.name);
    fout.write(reinterpret_cast<char *>(&operation1), sizeof(operation));
    fout.close();
}

user the_user(const string &user_id1) {
    //find the certain user through its user_id
    if (user_id_list.findnode(user_id1).empty()) throw 1;
    user user1;
    int offset1 = user_id_list.findnode(user_id1)[0].offset;
    ifstream fin;
    fin.open("userdata.dat", ios::in | ios::out | ios::binary);
    fin.seekg(offset1);
    fin.read(reinterpret_cast<char *>(&user1), sizeof(user));
    fin.close();
    return user1;
}
