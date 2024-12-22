#ifndef BOOK_H
#define BOOK_H
#include <iostream>
#include <fstream>
#include <cstring>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;



struct Book {
    char ISBN[21];
    char BookName[31];
    char Author[31];
    char Keyword[61];
    int ISBN_len = 0;
    int BookName_len = 0;
    int Author_len = 0;
    int Keyword_len;
    int Quantity = 0;
    double price = 0;
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

const int block_size_ = 128;
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

    NodeHead_for_Books(const std::string &file_name) {
        this -> file_name = file_name;
        //file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        //if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        //}
    }

    ~NodeHead_for_Books() {
        file.close();
    }

    HeadNode_ visitHead(int index) {
        HeadNode_ ret;
        file.seekp(index * sizeofH_);
        file.read(reinterpret_cast<char*>(&ret), sizeofH_);
        ret.cur_index = ret.id * block_size_;
        return ret;
    }

    int getBlockSize(int index) {
        return link_[index].size;
    }

    void writeHead(int index, HeadNode_ ret) {
        file.seekp(index * sizeofH_);
        file.write(reinterpret_cast<char*>(&ret), sizeofH_);
    }

    int addHead(int index) {
        new_id++;
        HeadNode_ head_node;
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

    NodeBody_for_Books(const std::string &file_name) {
        this -> file_name = file_name;
        //file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        //if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        //}
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
    Book current_Book;
    bool select = false;
public:
    Book_Operation();

    ~Book_Operation();

    void initialise();

    void flush();

    void addNode(int, Book);

    static void deleteNode(int);

    void Select(const std::string &);

    void Modify(int, const std::string &);

    Book getBook(const std::string &);

    void Insert(const std::string &);

    void Import(int);

    void Show(int, const std::string &);

    double Buy(const std::string &, int);

    static int string_cmp(char*, char*, int, int);

    static double stringToDouble(const std::string &);
};

#endif //BOOK_H