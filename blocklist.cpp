#include "blocklist.h"

node::node(const string &key1, const int &offset1) {
    strcpy(key,key1.c_str());
    offset = offset1;
}

node::node(const node &node1) {
    strcpy(key,node1.key);
    offset = node1.offset;
}
node::~node(){}
list::list(const string &s) {
    file = s;
}

void list::addnode(node &node1) {
    ifstream fin;
    ofstream fout;
    block blk1;
    fin.open(file,ios::ate | ios::in | ios::out | ios::binary);
    fout.open(file, ios::in | ios::out | ios::binary);
    if (!fin || !fout) throw 1;
    //when the number of block is 0
    if (fin.tellg() == 0) {
        fout.seekp(0);
        fout.write(reinterpret_cast<char *>(&blk1), sizeof(block));
    }
    //find the last possible block
    int cur = 0, next = nextblk(cur);
    while (next != -1) {
        fin.seekg(next);
        fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
        if (strcmp(node1.key,blk1.node_array[0].key)<0) break;
        cur = next;
        next = nextblk(cur);
    }
    //insert the node into the block
    fin.seekg(cur);
    fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
    int it = 0;
    while (it != blk1.len) {
        if (strcmp(blk1.node_array[it].key , node1.key)>0) break;
        it++;
    }
    for (int i = blk1.len - 1; i >= it; i--) {
        blk1.node_array[i + 1] = blk1.node_array[i];
    }
    blk1.node_array[it] = node1;
    blk1.len++;
    //to renew the block and split
    fout.seekp(cur);
    fout.write(reinterpret_cast<char *>(&blk1), sizeof(block));
    fin.close();
    fout.close();
    if (blk1.len > 295) split(cur);
}

void list::dltnode(node &node1) {
    ifstream fin;
    ofstream fout;
    block blk1;
    fin.open(file,ios::ate | ios::in | ios::out | ios::binary);
    fout.open(file, ios::in | ios::out | ios::binary);
    if (!fin || !fout) throw 1;
    //find the first possible block
    int cur = 0, next = nextblk(cur);
    while (next != -1) {
        fin.seekg(next);
        fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
        if (strcmp(node1.key,blk1.node_array[0].key) <= 0) break;
        cur = next;
        next = nextblk(cur);
    }
    //delete the node from the block
    int it = 0;
    fin.seekg(cur);
    fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
    while (strcmp(blk1.node_array[0].key,node1.key) <= 0) {
        fin.seekg(cur);
        fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
        while (it != blk1.len) {
            if (blk1.node_array[it].offset == node1.offset) break;
            it++;
        }
        if (it != blk1.len) break;
        cur = nextblk(cur);
        it = 0;
    }
    for (int i = it; i <= blk1.len - 2; i++) {
        blk1.node_array[i] = blk1.node_array[i + 1];
    }
    blk1.len--;
    //to renew the block
    fout.seekp(cur);
    fout.write(reinterpret_cast<char *>(&blk1), sizeof(block));
    fin.close();
    fout.close();
}

vector<node> list::findnode(const string &key) {
    ifstream fin;
    fin.open(file,ios::ate | ios::in | ios::out | ios::binary);
    if (!fin) throw 1;
    block blk1;
    vector<node> possible_nodes;
    //when the number of block is 0
    if(fin.tellg() == 0){
        fin.close();
        return possible_nodes;
    }
    //find the possible blocks
    int first = 0,last = 0;
    while (nextblk(last) != -1) {
        fin.seekg(nextblk(last));
        fin.read(reinterpret_cast<char*>(&blk1),sizeof(block));
        if (strcmp(key.c_str(),blk1.node_array[0].key) < 0) break;
        fin.seekg(last);
        fin.read(reinterpret_cast<char*>(&blk1),sizeof(block));
        if (strcmp(key.c_str(), blk1.node_array[0].key) > 0) first = last;
        last = nextblk(last);
    }
    //find the nodes from the possible blocks and insert them into the vector
    while (true) {
        fin.seekg(first);
        fin.read(reinterpret_cast<char*>(&blk1),sizeof(block));
        for(int i = 0 ; i < blk1.len ; i++)
        {
            if(strcmp(key.c_str(),blk1.node_array[i].key) == 0){
                possible_nodes.push_back(blk1.node_array[i]);
            }
        }
        if (first == last) break;
        first = blk1.nxt;
    }
    fin.close();
    return possible_nodes;
}

void list::split(int offset) {
    ifstream fin;
    ofstream fout;
    fin.open(file, ios::in | ios::out | ios::binary);
    fout.open(file, ios::in | ios::out | ios::binary);
    if (!fin || !fout) throw 1;
    //construct the two blocks
    block blk1,blk2;
    fin.seekg(offset);
    fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
    blk1.len-=100;
    blk2.len=100;
    for (int i=0;i<=99;i++) {
        blk2.node_array[i]=blk1.node_array[blk1.len+i];
    }
    fin.seekg(0, ios::end);
    int offset2=fin.tellg();
    //link the new list
    blk2.nxt=blk1.nxt;
    blk1.nxt=offset2;
    //renew the list
    fout.seekp(offset);
    fout.write(reinterpret_cast<char*>(&blk1),sizeof(block));
    fout.seekp(offset2);
    fout.write(reinterpret_cast<char*>(&blk2),sizeof(block));
    fin.close();
    fout.close();
}

int list::nextblk(int offset) {
    ifstream fin;
    fin.open(file, ios::in | ios::out | ios::binary);
    block blk1;
    fin.seekg(offset);
    fin.read(reinterpret_cast<char *>(&blk1), sizeof(block));
    return blk1.nxt;
};
