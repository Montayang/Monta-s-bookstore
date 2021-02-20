#include "command.h"
#include <map>
extern list user_id_list;
extern list ISBN_list;
extern list name_list;
extern list author_list;
extern list keyword_list;
extern stack<pair<string,int>> users_online;
vector<string> con_keyword(const string& keyword1) {
    //put the keywords into a vector
    vector<string> keyword;
    string s;
    for (auto i : keyword1) {
        if (i=='|') {
            for (const auto& it : keyword) {
                if (strcmp(it.c_str(),s.c_str()) == 0) throw 1;
            }
            keyword.push_back(s);
            s="";
        }
        else s+=i;
    }
    for (const auto& it : keyword) {
        if (strcmp(it.c_str(),s.c_str()) == 0) throw 1;
    }
    keyword.push_back(s);
    return keyword;
}

void select(const string& ISBN1) {
    if (users_online.empty() || the_user(users_online.top().first).privilege<3) throw 1;
    int offset1;
    if (ISBN_list.findnode(ISBN1).empty()) {
        book book1;
        strcpy(book1.ISBN,ISBN1.c_str());
        ofstream fout;
        fout.open("bookdata.dat",ios::ate | ios::in | ios::out | ios::binary);
        offset1=fout.tellp();
        fout.write(reinterpret_cast<char*>(&book1),sizeof(book));
        fout.close();
        node node1(ISBN1,offset1);
        ISBN_list.addnode(node1);
    }
    user user1=the_user(users_online.top().first);
    users_online.pop();
    users_online.push(make_pair(user1.user_id,ISBN_list.findnode(ISBN1)[0].offset));
}

void modify(int offset1,const string& ISBN1, const string& name1, const string& author1, const string& keyword1, double price1) {
    if (the_user(users_online.top().first).privilege<3) throw 1;
    if (!ISBN1.empty() && !ISBN_list.findnode(ISBN1).empty()) throw 1;
    //record data of the old book
    book book0;
    ifstream fin;
    fin.open("bookdata.dat",ios::in | ios::out | ios::binary);
    fin.seekg(offset1);
    fin.read(reinterpret_cast<char*>(&book0),sizeof(book));
    fin.close();
    //construct the new book
    string ISBN2=ISBN1.empty()?book0.ISBN:ISBN1;
    string name2=name1.empty()?book0.name:name1;
    string author2=author1.empty()?book0.author:author1;
    string keyword2=keyword1.empty()?book0.keyword:keyword1;
    double price2=price1==42142?book0.price:price1;
    book book1(ISBN2,name2,author2,keyword2,price2,book0.quantity);
    //renew the blocklists
    if (!ISBN1.empty()) {
        node node1(book0.ISBN,offset1),node2(ISBN1,offset1);
        ISBN_list.dltnode(node1);
        ISBN_list.addnode(node2);
    }
    if (!name1.empty()) {
        node node1(book0.name,offset1),node2(name1,offset1);
        if (!name_list.findnode(book0.name).empty()) name_list.dltnode(node1);
        name_list.addnode(node2);
    }
    if (!author1.empty()) {
        node node1(book0.author,offset1),node2(author1,offset1);
        if (!author_list.findnode(book0.author).empty()) author_list.dltnode(node1);
        author_list.addnode(node2);
    }
    if (!keyword1.empty()) {
        for (const auto& it : con_keyword(book0.keyword)) {
            node node1(it,offset1);
            if (!keyword_list.findnode(it).empty()) keyword_list.dltnode(node1);
        }
        for (const auto& it : con_keyword(keyword1)) {
            node node2(it,offset1);
            keyword_list.addnode(node2);
        }
    }
    //renew bookdata.dat
    fstream fout;
    fout.open("bookdata.dat",ios::in | ios::out | ios::binary);
    fout.seekp(offset1);
    fout.write(reinterpret_cast<char*>(&book1),sizeof(book));
    fout.close();
}

void import(int offset1, int quantity1, double cost_price) {
    if (users_online.empty() || the_user(users_online.top().first).privilege<3) throw 1;
    book book1;
    ifstream fin;
    fin.open("bookdata.dat", ios::in | ios::out | ios::binary);
    fin.seekg(offset1);
    fin.read(reinterpret_cast<char*>(&book1),sizeof(book));
    fin.close();
    // record the transaction
    transaction_record transaction_record1(-cost_price,book1,quantity1,the_user(users_online.top().first));
    fstream fout;
    fout.open("transaction.dat", ios::ate | ios::in | ios::out | ios::binary);
    fout.write(reinterpret_cast<char*>(&transaction_record1),sizeof(transaction_record));
    fout.close();
    //renew bookdata.dat
    book1.quantity+=quantity1;
    fout.open("bookdata.dat", ios::in | ios::out | ios::binary);
    fout.seekp(offset1);
    fout.write(reinterpret_cast<char*>(&book1),sizeof(book));
    fout.close();
}

void show() {
    map<string,book> possible_books;
    ifstream fin;
    fin.open("bookdata.dat",ios::ate | ios::in | ios::out | ios::binary);
    int End=fin.tellg();
    fin.seekg(0);
    while (fin.tellg()!=End) {
        book book1;
        fin.read(reinterpret_cast<char*>(&book1),sizeof(book));
        possible_books.insert(make_pair(book1.ISBN,book1));
    }
    fin.close();
    if (possible_books.empty()) { cout<<endl; return; }
    for (const auto& it : possible_books) {
        book book1=it.second;
        cout<<book1.ISBN<<"\t"<<book1.name<<"\t"<<book1.author<<"\t"<<book1.keyword<<"\t";
        printf("%.2f",book1.price);
        cout<<"\t"<<book1.quantity<<endl;
    }
}

void show(const string& parameter, string type) {
    if (users_online.empty()) throw 1;
    vector<node> vec1;
    switch (type[0]) {
        case 'I' : vec1=ISBN_list.findnode(parameter); break;
        case 'n' : vec1=name_list.findnode(parameter); break;
        case 'a' : vec1=author_list.findnode(parameter); break;
        case 'k' : vec1=keyword_list.findnode(parameter); break;
    }
    if (vec1.empty()) { cout<<endl; return; }
    map<string,book> possible_books;
    for (const auto& it : vec1) {
        book book1;
        ifstream fin;
        fin.open("bookdata.dat", ios::in | ios::out | ios::binary);
        fin.seekg(it.offset);
        fin.read(reinterpret_cast<char*>(&book1),sizeof(book));
        fin.close();
        possible_books.insert(make_pair(book1.ISBN,book1));
    }
    if (type[0] == 'k' && !possible_books.empty()) {
        vector<string> vec2;
        for (auto book1 : possible_books) {
            bool flag=false;
            for (const auto& it : con_keyword(book1.second.keyword)) {
                if (strcmp(it.c_str(),parameter.c_str()) == 0) {
                    flag=true;
                    break;
                }
            }
            if (!flag) vec2.push_back(book1.first);
        }
        if (!vec2.empty()) {
            for (const auto& it : vec2) possible_books.erase(it);
        }
    }
    if (possible_books.empty()) { cout<<endl; return;}
    for (const auto& it : possible_books) {
        if (it.first.empty()) continue;
        book book1=it.second;
        cout<<book1.ISBN<<"\t"<<book1.name<<"\t"<<book1.author<<"\t"<<book1.keyword<<"\t";
        printf("%.2f",book1.price);
        cout<<"\t"<<book1.quantity<<endl;
    }
}

void show_finance(int time) {
    if (users_online.empty() || the_user(users_online.top().first).privilege != 7) throw 1;
    ifstream fin;
    fin.open("transaction.dat",ios::ate | ios::in | ios::out | ios::binary);
    //calculate the total number of transaction
    int End=fin.tellg();
    fin.seekg(0);
    int Begin=fin.tellg();
    int num_transaction=(End-Begin)/sizeof(transaction_record);
    if (time>num_transaction) throw 1;
    if (time==-1) time = num_transaction;
    //calculate the income and the outcome
    fin.seekg((num_transaction-time) * sizeof(transaction_record));
    double income=0,outcome=0;
    while (fin.tellg()!=End) {
        transaction_record transaction_record1;
        fin.read(reinterpret_cast<char*>(&transaction_record1),sizeof(transaction_record));
        if (transaction_record1.money>0) income+=transaction_record1.money;
        if (transaction_record1.money<0) outcome+=-transaction_record1.money;
    }
    fin.close();
    cout<<"+ ";
    printf("%.2f",income);
    cout<<" - ";
    printf("%.2f",outcome);
    cout<<endl;
}

void buy(const string& ISBN1, int quantity1) {
    if (users_online.empty()) throw 1;
    book book1=the_book(ISBN1);
    int offset1=ISBN_list.findnode(ISBN1)[0].offset;
    if (quantity1 > book1.quantity) throw 1;
    // record the transaction
    double cost_price=quantity1*book1.price;
    transaction_record transaction_record1(cost_price,book1,-quantity1,the_user(users_online.top().first));
    ofstream fout;
    fout.open("transaction.dat",ios::ate | ios::in | ios::out | ios::binary);
    fout.write(reinterpret_cast<char*>(&transaction_record1),sizeof(transaction_record));
    fout.close();
    //renew bookdata.dat
    book1.quantity-=quantity1;
    fout.open("bookdata.dat", ios::in | ios::out | ios::binary);
    fout.seekp(offset1);
    fout.write(reinterpret_cast<char*>(&book1),sizeof(book));
    fout.close();
    //output the cost
    printf("%.2f",book1.price*quantity1);
    cout<<endl;
}
