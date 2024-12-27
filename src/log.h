#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include "book.h"

using std::string;
using std::fstream;

struct User{
    char UserID[32];
    char Password[32];
    char UserName[32];
    int UserID_len;
    int Password_len;
    int UserName_len;
    int privilege;
    bool login;
};

struct HeadNode {
    int id;
    int prev_head;
    int nex_head;
    int size;
    int cur_index;
};

extern User* bloc;
extern HeadNode link[100001];

const int block_size = 300;
const int sizeofP = sizeof(User);
const int sizeofH = 4 * sizeof(int);

class NodeHead_for_Users {
    fstream file;
    std::string file_name;
public:

    int new_id = -1;
    int cur_size = 0;
    int head = -1;

    NodeHead_for_Users() = default;

    explicit NodeHead_for_Users(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeHead_for_Users() {
        file.close();
    }

    HeadNode visitHead(int index) {
        HeadNode ret{};
        file.seekp(index * sizeofH);
        file.read(reinterpret_cast<char*>(&ret), sizeofH);
        ret.cur_index = ret.id * block_size;
        return ret;
    }

    void writeHead(int index, HeadNode ret) {
        file.seekp(index * sizeofH);
        file.write(reinterpret_cast<char*>(&ret), sizeofH);
    }

    int addHead(int index) {
        new_id++;
        HeadNode head_node{};
        if (cur_size == 0) {
            head = new_id;
            head_node.id = new_id;
            head_node.prev_head = head_node.nex_head = -1;
            head_node.size = 0;
            head_node.cur_index = new_id * block_size;
        }
        else {
            head_node.id = new_id;
            head_node.cur_index = new_id * block_size;
            head_node.size = 0;
            head_node.prev_head = link[index].id;
            head_node.nex_head = link[index].nex_head;
            link[index].nex_head = new_id;
            if (head_node.nex_head != -1) {
                link[head_node.nex_head].prev_head = new_id;
            }
        }
        link[new_id] = head_node;
        cur_size++;
        return new_id;
    }

    void deleteHead(int index) {
        int prev_head = link[index].prev_head, nex_head = link[index].nex_head;
        if (prev_head != -1) {
            link[prev_head].nex_head = nex_head;
        }
        else {
            head = nex_head;
        }
        if (nex_head != -1) {
            link[nex_head].prev_head = prev_head;
        }
        cur_size--;
    }
};

class NodeBody_for_Users {
    fstream file;
    std::string file_name;
public:

    NodeBody_for_Users() = default;

    NodeBody_for_Users(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeBody_for_Users() {
        file.close();
    }

    void visitNode(int index) {
        file.seekp(index * sizeofP);
        file.read(reinterpret_cast<char*>(bloc), link[index / block_size].size * sizeofP);
    }

    void writeNode(int index) {
        file.seekp(index * sizeofP);
        file.write(reinterpret_cast<char*>(bloc), link[index / block_size].size * sizeofP);
    }
};


class User_Operation {
    NodeHead_for_Users Head;
    NodeBody_for_Users Body;
    std::vector <User> login_stack;
public:

    User current_User{};

    User_Operation();

    ~User_Operation();

    void initialise();

    void flush();

    static void addNode(int, User);

    static void deleteNode(int);

    void Insert(
        const std::string &,
        const std::string &,
        const std::string &,
        int );

    void Delete(const std::string &);

    User getUser(const std::string &);

    void changePassword(const std::string &, const std::string &, const std::string &);

    void Login(const string &, const string &);

    void Logout();

    void Quit();

    static bool checkValidity(const std::string &);

    static int string_cmp(char*, char*, int, int);
};
#endif //LOG_H
