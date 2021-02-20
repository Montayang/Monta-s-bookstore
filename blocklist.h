#include <iostream>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>

#ifndef BOOKSTORE_BLOCKLIST_H
#define BOOKSTORE_BLOCKLIST_H
using namespace std;

class node {
public:
    char key[100];
    int offset;

    explicit node(const string &key1 = "", const int &offset1 = -1);

    node(const node &node1);

    ~node();
};

class block {
public:
    node node_array[300];
    int nxt = -1, len = 0;

    block() = default;
};

class list {
public:

    explicit list(const string &s);

    void addnode(node &node1);

    void dltnode(node &node1);

    vector<node> findnode(const string &key);

    int nextblk(int offset);

private:
    string file;

    void split(int offset);
};

#endif //BOOKSTORE_BLOCKLIST_H
