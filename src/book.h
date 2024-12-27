#ifndef BOOK_H
#define BOOK_H
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include<algorithm>
#include <cstring>
#include "name.h"
#include "author.h"
#include "keyword.h"

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

struct Book {
    char ISBN[22]{};
    char BookName[62]{};
    char Author[62]{};
    char Keyword[62]{};
    int ISBN_len = 0;
    int BookName_len = 0;
    int Author_len = 0;
    int Keyword_len{};
    long long Quantity = 0;
    double price = 0;
    bool is_new = false;
};

struct HeadNode_ {
    int id;
    int prev_head;
    int nex_head;
    int size;
    int cur_index;
};

extern Book* bloc_;
extern HeadNode_ link_[100001];

const int block_size_ = 300;
const int sizeofP_ = sizeof(Book);
const int sizeofH_ = 4 * sizeof(int);

class NodeHead_for_Books {
    fstream file;
    std::string file_name;
public:

    int new_id = -1;
    int cur_size = 0;
    int head = -1;

    NodeHead_for_Books() = default;

    explicit NodeHead_for_Books(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeHead_for_Books() {
        file.close();
    }

    HeadNode_ visitHead(int index) {
        HeadNode_ ret{};
        file.seekp(index * sizeofH_);
        file.read(reinterpret_cast<char*>(&ret), sizeofH_);
        ret.cur_index = ret.id * block_size_;
        return ret;
    }

    static int getBlockSize(int index) {
        return link_[index].size;
    }

    void writeHead(int index, HeadNode_ ret) {
        file.seekp(index * sizeofH_);
        file.write(reinterpret_cast<char*>(&ret), sizeofH_);
    }

    int addHead(int index) {
        new_id++;
        HeadNode_ head_node{};
        if (cur_size == 0) {
            head = new_id;
            head_node.id = new_id;
            head_node.prev_head = head_node.nex_head = -1;
            head_node.size = 0;
            head_node.cur_index = new_id * block_size_;
        }
        else {
            head_node.id = new_id;
            head_node.cur_index = new_id * block_size_;
            head_node.size = 0;
            head_node.prev_head = link_[index].id;
            head_node.nex_head = link_[index].nex_head;
            link_[index].nex_head = new_id;
            if (head_node.nex_head != -1) {
                link_[head_node.nex_head].prev_head = new_id;
            }
        }
        link_[new_id] = head_node;
        cur_size++;
        return new_id;
    }

    void deleteHead(int index) {
        int prev_head = link_[index].prev_head, nex_head = link_[index].nex_head;
        if (prev_head != -1) {
            link_[prev_head].nex_head = nex_head;
        }
        else {
            head = nex_head;
        }
        if (nex_head != -1) {
            link_[nex_head].prev_head = prev_head;
        }
        cur_size--;
    }
};

class NodeBody_for_Books {
    fstream file;
    std::string file_name;
public:

    NodeBody_for_Books() = default;

    explicit NodeBody_for_Books(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeBody_for_Books() {
        file.close();
    }

    void visitNode(int index) {
        file.seekp(index * sizeofP_);
        file.read(reinterpret_cast<char*>(bloc_), link_[index / block_size_].size * sizeofP_);
    }

    void writeNode(int index) {
        file.seekp(index * sizeofP_);
        file.write(reinterpret_cast<char*>(bloc_), link_[index / block_size_].size * sizeofP_);
    }
};

class Book_Operation {
    NodeHead_for_Books Head;
    NodeBody_for_Books Body;
    name_operation name_op;
    author_operation author_op;
    keyword_operation keyword_op;
public:

    std::vector<Book> book_stack;
    Book current_Book;

    Book_Operation();

    ~Book_Operation();

    void initialise();

    void flush();

    static void addNode(int, Book);

    static void deleteNode(int);

    void Select(const std::string &);

    void Modify(const std::string &);

    Book getBook(const std::string &);

    void Insert(const std::string &);

    void Insert_(Book);

    void Delete();

    bool Import(int);

    void Show(int, const std::string &);

    double Buy(const std::string &, int);

    static std::vector<std::string> Key_Split(const std::string &);

    static bool checkValidity(const std::string &);

    static int string_cmp(char*, char*, int, int);

    static double stringToDouble(const std::string &);
};

extern Book_Operation Book_op;

#endif //BOOK_H