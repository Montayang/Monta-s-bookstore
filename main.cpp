#include "command.h"
#include <sstream>
using namespace std;
extern list user_id_list;
extern stack<pair<string,int>> users_online;
void initialize();

void execute(const string& cmd);

int main() {
    initialize();
    string cmd;
    while (getline(cin, cmd)) {
        try {
            execute(cmd);
        } catch (...) {
            cout << "Invalid\n";
        }
    }
    return 0;
}

void initialize() {
    fstream fx;
    fx.open("userdata.dat",ios::in | ios::binary);
    if (!fx) {
        //the file stored the data of the instances
        fx.open("userdata.dat", ios::out | ios::binary);
        user root("root", "sjtu", "root");
        root.privilege = 7;
        fx.seekg(0);
        fx.write(reinterpret_cast<char *>(&root), sizeof(user));
        fx.close();
        fx.open("user_id_list.dat", ios::out | ios::binary);
        fx.close();
        node node1("root", 0);
        user_id_list.addnode(node1);
        fx.open("bookdata.dat", ios::out | ios::binary);
        fx.close();
        fx.open("transaction.dat", ios::out | ios::binary);
        fx.close();
        //the file stored the data of the blocklists
        fx.open("ISBN_list.dat", ios::out | ios::binary);
        fx.close();
        fx.open("name_list.dat", ios::out | ios::binary);
        fx.close();
        fx.open("author_list.dat", ios::out | ios::binary);
        fx.close();
        fx.open("keyword_list.dat", ios::out | ios::binary);
    }
    fx.close();
}

string search_token(string sentence, int position) {
    string s;
    int count = 0;
    for (int i = 0; i <= sentence.length(); i++) {
        if (sentence[i] == ' ') {
            if (count == position) break;
            count++;
        } else if (count == position - 1) {
            s += sentence[i];
        }
    }
    if (strcmp(s.c_str()," ")<=0) s="";
    return s;
};

string get_token(const string& sentence) {
    //get the word which is in the ""
    string token;
    int flag=0;
    for (auto i : sentence) {
        if (i=='\"') {
            if (flag == 1) break;
            else flag=1-flag;
        }
        if (flag==1 && i!='\"') token+=i;
    }
    return token;
}

void execute(const string& cmd) {
    string first_token = search_token(cmd, 1);
    //execute the command which is in the system of users
    if (strcmp(first_token.c_str(),"su") == 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 4).empty()) throw 1;
        //login the user with a lower privilege
        if (search_token(cmd, 3).empty()) {
            if (users_online.empty()) throw 1;
            if (the_user(users_online.top().first).privilege <= the_user(search_token(cmd, 2)).privilege) throw 1;
            su(search_token(cmd, 2), the_user(search_token(cmd, 2)).passwd);
            return;
        }
        //simply login
        su(search_token(cmd, 2), search_token(cmd, 3));
        return;
    } else if (strcmp(first_token.c_str(),"logout") == 0) {
        if (!search_token(cmd, 2).empty()) throw 1;
        logout();
        return;
    } else if (strcmp(first_token.c_str(),"useradd") == 0) {
        if (search_token(cmd, 5).empty() || !search_token(cmd, 6).empty()) throw 1;
        useradd(search_token(cmd, 2), search_token(cmd, 3), search_token(cmd, 4)[0] - '0', search_token(cmd, 5));
        return;
    } else if (strcmp(first_token.c_str(),"register") == 0) {
        if (search_token(cmd, 4).empty() || !search_token(cmd, 5).empty()) throw 1;
        Register(search_token(cmd, 2), search_token(cmd, 3), search_token(cmd, 4));
        return;
    } else if (strcmp(first_token.c_str(),"delete") == 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 3).empty()) throw 1;
        Delete(search_token(cmd, 2));
        return;
    } else if (strcmp(first_token.c_str(),"passwd") == 0) {
        if (search_token(cmd, 3).empty() || !search_token(cmd, 5).empty()) throw 1;
        //change the passwd of root
        if (search_token(cmd, 4).empty()) {
            if (users_online.empty()) throw 1;
            if (the_user(users_online.top().first).privilege != 7) throw 1;
            change_passwd(search_token(cmd, 2), the_user(search_token(cmd, 2)).passwd, search_token(cmd, 3));
            return;
        }
        //simply change the passwd
        change_passwd(search_token(cmd, 2), search_token(cmd, 3), search_token(cmd, 4));
        return;
    }
    //execute the command which is in the system of books
    if (strcmp(first_token.c_str(),"select") == 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 3).empty()) throw 1;
        select(search_token(cmd, 2));
        return;
    } else if (strcmp(first_token.c_str(),"modify") == 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 7).empty()) throw 1;
        if (users_online.empty() || users_online.top().second == -1) throw 1;
        string token[6], parameter[6]={"42142","42142","42142","42142","42142","012345642142"};
        for (int i = 1; i <= 5; i++) {
            token[i] = search_token(cmd, i + 1).empty() ? "42142" : search_token(cmd, i + 1);
            switch (token[i][1]) {
                case 'I': parameter[1] = '\"'+token[i].substr(6)+'\"'; break;
                case 'n': parameter[2] = token[i]; break;
                case 'a': parameter[3] = token[i]; break;
                case 'k': parameter[4] = token[i]; break;
                case 'p': parameter[5] = token[i]; break;
            }
        }
        stringstream f(parameter[5].substr(7));
        double x;
        f>>x;
        modify(users_online.top().second,get_token(parameter[1]),get_token(parameter[2]),get_token(parameter[3]),get_token(parameter[4]),x);
        return;
    } else if (strcmp(first_token.c_str(),"import") == 0) {
        if (users_online.empty() || users_online.top().second == -1) throw 1;
        if (search_token(cmd, 3).empty() || !search_token(cmd, 4).empty()) throw 1;
        if (search_token(cmd,2)[0]<'0' || search_token(cmd,2)[0]>'9') throw 1;
        stringstream f1(search_token(cmd,2)),f2(search_token(cmd,3));
        int x1; double x2;
        f1>>x1; f2>>x2;
        import(users_online.top().second , x1 , x2);
        return;
    } else if (strcmp(first_token.c_str(),"show") == 0 && search_token(cmd,2).empty()) {
        show();
        return;
    } else if (strcmp(first_token.c_str(),"show") == 0 && strcmp(search_token(cmd,2).c_str() , "finance") != 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 3).empty()) throw 1;
        switch (search_token(cmd,2)[1]) {
            case 'I': show(search_token(cmd,2).substr(6),"ISBN"); break;
            case 'n': show(get_token(cmd),"name"); break;
            case 'a': show(get_token(cmd),"author"); break;
            case 'k': show(get_token(cmd),"keyword"); break;
            default: throw 1;
        }
        return;
    } else if (strcmp(first_token.c_str(),"show") == 0 && strcmp(search_token(cmd,2).c_str() , "finance") == 0) {
        if (search_token(cmd, 2).empty() || !search_token(cmd, 4).empty()) throw 1;
        stringstream f(search_token(cmd,3));
        int x;
        f>>x;
        if (search_token(cmd,3).empty()) show_finance(-1);
        else show_finance(x);
        return;
    } else if (strcmp(first_token.c_str(),"buy") == 0) {
        if (search_token(cmd, 3).empty() || !search_token(cmd, 4).empty()) throw 1;
        stringstream f(search_token(cmd,3));
        int x;
        f>>x;
        buy(search_token(cmd,2), x);
        return;
    } else if (strcmp(first_token.c_str(),"exit") == 0 || strcmp(first_token.c_str(),"quit") == 0) {
        exit(0);
    } else throw 1;
}
