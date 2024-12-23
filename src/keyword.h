#ifndef KEYWORD_H
#define KEYWORD_H
#include <iostream>
#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int block_size3 = 128;

struct Pair3 {
    char key[61];
    char value[21];
    int key_len;
    int value_len;
};

struct HeadNode2 {
    int id;
    int prev_head;
    int nex_head;
    int size;
    int cur_index;
};

extern Pair3* bloc3;
extern HeadNode2 link3[100001];

const int sizeofP3 = sizeof(Pair3);
const int sizeofH3 = 4 * sizeof(int);

class NodeHead_for_keyword {
    fstream file;
    std::string file_name;
public:

    int new_id = -1;
    int cur_size = 0;
    int head = -1;

    NodeHead_for_keyword() = default;

    NodeHead_for_keyword(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeHead_for_keyword() {
        file.close();
    }

    HeadNode2 visitHead(int index) {
        HeadNode2 ret;
        file.seekp(index * sizeofH3);
        file.read(reinterpret_cast<char*>(&ret), sizeofH3);
        ret.cur_index = ret.id * block_size3;
        return ret;
    }

    static int getBlockSize(int index) {
        return link3[index].size;
    }

    void writeHead(int index, HeadNode2 ret) {
        file.seekp(index * sizeofH3);
        file.write(reinterpret_cast<char*>(&ret), sizeofH3);
    }

    int addHead(int index) {
        new_id++;
        HeadNode2 head_node{};
        if (cur_size == 0) {
            head = new_id;
            head_node.id = new_id;
            head_node.prev_head = head_node.nex_head = -1;
            head_node.size = 0;
            head_node.cur_index = new_id * block_size3;
        }
        else {
            head_node.id = new_id;
            head_node.cur_index = new_id * block_size3;
            head_node.size = 0;
            head_node.prev_head = link3[index].id;
            head_node.nex_head = link3[index].nex_head;
            link3[index].nex_head = new_id;
            if (head_node.nex_head != -1) {
                link3[head_node.nex_head].prev_head = new_id;
            }
        }
        link3[new_id] = head_node;
        cur_size++;
        return new_id;
    }

    void deleteHead(int index) {
        int prev_head = link3[index].prev_head, nex_head = link3[index].nex_head;
        if (prev_head != -1) {
            link3[prev_head].nex_head = nex_head;
        }
        else {
            head = nex_head;
        }
        if (nex_head != -1) {
            link3[nex_head].prev_head = prev_head;
        }
        cur_size--;
    }
};

class NodeBody_for_keyword {
    fstream file;
    std::string file_name;
public:

    NodeBody_for_keyword() = default;

    explicit NodeBody_for_keyword(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeBody_for_keyword() {
        file.close();
    }

    void visitNode(int index) {
        delete[] bloc3;
        bloc3 = new Pair3[block_size3 + 1];
        file.seekp(index * sizeofP3);
        file.read(reinterpret_cast<char*>(bloc3), link3[index / block_size3].size * sizeofP3);
    }

    void writeNode(int index) {
        file.seekp(index * sizeofP3);
        file.write(reinterpret_cast<char*>(bloc3), link3[index / block_size3].size * sizeofP3);
    }
};

class keyword_operation {
    NodeHead_for_keyword Head;
    NodeBody_for_keyword Body;
public:
    keyword_operation(): Head("NodeHead_for_keyword"), Body("NodeBody_for_keyword") {
        initialise();
    }

    ~keyword_operation() {
        flush();
    }

    void initialise() {
        fstream file_;
        file_.open("Host_for_keyword", std::ios::in|std::ios::out);
        if (!file_.is_open()) {
            file_.open("Host_for_keyword", std::ios::out);
            file_.close();
            file_.open("Host_for_keyword", std::ios::in|std::ios::out|std::ios::binary);
            Head.new_id = -1;
            Head.cur_size = 0;
            Head.head = -1;
            file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
            file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
            file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
        }
        else {
            file_.read(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
            file_.read(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
            file_.read(reinterpret_cast<char*>(&Head.head), sizeof(int));
            int p = Head.head;
            while (p != -1) {
                link3[p] = Head.visitHead(p);
                p = link3[p].nex_head;
            }
        }
        file_.close();
        bloc3 = new Pair3[block_size3 + 1];
    }

    void flush() {
        fstream file_;
        file_.open("Host_for_keyword", std::ios::in|std::ios::out|std::ios::binary);
        file_.seekp(0);
        file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
        file_.close();
        int p = Head.head;
        while (p != -1) {
            Head.writeHead(p, link3[p]);
            p = link3[p].nex_head;
        }
        file_.close();

        delete[] bloc3;
    }

    static void addNode(int index, Pair3 data) {
        int cursor = index / block_size3;
        int cur_size = NodeHead_for_keyword::getBlockSize(cursor);
        int insert_place;
        Pair3 ret;

        int l = 0, r = cur_size - 1, mid;
        while (l <= r) {
            mid = (l + r) / 2;
            ret = bloc3[mid];
            if (pair_cmp(data, ret) == 0) {
                return;
            }
            if (pair_cmp(data,ret) == -1) {
                r = mid - 1;
            }
            else {
                l = mid + 1;
            }
        }
        insert_place = l;

        for (int i = cur_size - 1; i >= insert_place; i--) {
            bloc3[i + 1] = bloc3[i];
        }
        bloc3[insert_place] = data;
        cur_size++;
        link3[cursor].size = cur_size;
    }

    static void deleteNode(int index) {
        int cur_head = index / block_size3 * block_size3;
        int cursor = cur_head / block_size3;
        for (int i = index + 1; i < cur_head + link3[cursor].size; i++) {
            bloc3[i - 1 - cur_head] = bloc3[i - cur_head];
        }
        link3[cursor].size--;
    }

    void Insert(const std::string &name, const std::string &ISBN) {
        Pair3 data;
        std::strcpy(data.key, name.c_str());
        data.key_len = static_cast<int>(name.size());
        std::strcpy(data.value, ISBN.c_str());
        data.value_len = static_cast<int>(ISBN.size());

        int cur_size = Head.cur_size;
        if (cur_size == 0) {
            int id = Head.addHead(0);
            Body.visitNode(id * block_size3);
            addNode(id * block_size3, data);
            Body.writeNode(id * block_size3);
            return;
        }

        int p = Head.head, q = -1;
        int insert_type = 2;
        int nex_head, size, qsize;
        while (p != -1) {
            nex_head = link3[p].nex_head;
            size = link3[p].size;
            Body.visitNode(p * block_size3);
            if (size > 0) {
                if (pair_cmp(bloc3[0], data) == -1 && pair_cmp(data, bloc3[size - 1]) == -1) {
                    insert_type = 1;
                    break;
                }
                if (pair_cmp(bloc3[0], data) >= 0) {
                    break;
                }
            }
            q = p;
            p = nex_head;
            qsize = size;
        }

        if (insert_type == 1) {
            if (size == block_size3) {
                addNode(p * block_size3, data);

                int split_place = p;
                p = Head.addHead(split_place);
                link3[p].size = block_size3 - block_size3 / 2;
                link3[split_place].size = block_size3 / 2 + 1;

                Body.writeNode(split_place * block_size3);
                for (int i = block_size3 / 2 + 1; i < block_size3 + 1; i++) {
                    bloc3[i - block_size3 / 2 - 1] = bloc3[i];
                }
                Body.writeNode(p * block_size3);
            }
            else {
                addNode(p * block_size3, data);
                Body.writeNode(p * block_size3);
            }

        }
        if (insert_type == 2) {
            if (p != -1 && size < block_size3) {
                addNode(p * block_size3, data);
                Body.writeNode(p * block_size3);
            }
            else if (q != -1 && qsize < block_size3) {
                Body.visitNode(q * block_size3);
                addNode(q * block_size3, data);
                Body.writeNode(q * block_size3);
            }
            else {
                if (q == -1) {
                    q = p;
                }
                Body.visitNode(q * block_size3);
                addNode(q * block_size3, data);

                int split_place = q;
                q = Head.addHead(split_place);
                link3[split_place].size = block_size3 / 2 + 1;
                link3[q].size = block_size3 - block_size3 / 2;
                Body.writeNode(split_place * block_size3);
                for (int i = block_size3 / 2 + 1; i < block_size3 + 1; i++) {
                    bloc3[i - block_size3 / 2 - 1] = bloc3[i];
                }
                Body.writeNode(q * block_size3);
            }
        }
    }

    void Delete(const std::string &name, const std::string &ISBN) {
        Pair3 data;
        std::strcpy(data.key, name.c_str());
        data.key_len = static_cast<int>(name.size());
        std::strcpy(data.value, ISBN.c_str());
        data.value_len = static_cast<int>(ISBN.size());

        bool flag = false;
        int p = Head.head, q = -1;
        int size = 0, qsize = 0;
        while (p != -1) {
            size = link3[p].size;
            Body.visitNode(p * block_size3);
            if (size > 0) {
                if (pair_cmp(data, bloc3[0]) >= 0 && pair_cmp(bloc3[size - 1], data) >= 0) {
                    int l = 0, r = size - 1, mid;
                    while (l <= r) {
                        mid = (l + r) / 2;
                        if (pair_cmp(data, bloc3[mid]) == 0) {
                            flag = true;
                            deleteNode(link3[p].cur_index + mid);
                            break;
                        }
                        if (pair_cmp(data, bloc3[mid]) == -1) {
                            r = mid - 1;
                        }
                        else {
                            l = mid + 1;
                        }
                    }
                    break;
                }
            }
            q = p;
            p = link3[p].nex_head;
            qsize = size;
        }

        if (flag) {
            Body.writeNode(p * block_size3);
            if (link3[p].size == 0) {
                Head.deleteHead(p);
            }
            else if (p != -1 && q != -1 && size + qsize - 1 < block_size3) {
                Pair3* tp = new Pair3[size];
                for (int i = 0; i < size - 1; i++) {
                    tp[i] = bloc3[i];
                }
                Head.deleteHead(p);
                Body.visitNode(q * block_size3);
                for (int i = qsize; i < qsize + size - 1; i++) {
                    bloc3[i] = tp[i - qsize];
                }
                link3[q].size = qsize + size - 1;
                Body.writeNode(q * block_size3);
                delete[] tp;
            }
        }
    }

    std::vector<std::string> Find(const std::string &name) {
        std::vector<std::string> ret;
        int p = Head.head;
        char index[61];
        std::strcpy(index, name.c_str());
        int len = static_cast<int>(name.size());
        while (p != -1) {
            Body.visitNode(p * block_size3);
            Pair3 head_ = bloc3[0], tail_ = bloc3[link3[p].size - 1];
            if (string_cmp(tail_.key, index, tail_.key_len, len) == 0 &&
                string_cmp(head_.key, index, head_.key_len, len) == 0) {
                for (int i = 0; i < link3[p].size; i++) {
                    std::string tmp(bloc3[i].value, bloc3[i].value_len);
                    ret.push_back(tmp);
                }
            }
            else if (string_cmp(tail_.key, index, tail_.key_len, len) >= 0 &&
                string_cmp(head_.key, index, head_.key_len, len) <= 0) {
                int start_l = 0, end_l = 0, start_r = link3[p].size - 1, end_r = link3[p].size - 1, mid;
                while (start_l <= start_r) {
                    mid = (start_l + start_r) / 2;
                    Pair3 tmp = bloc3[mid];
                    if (string_cmp(tmp.key, index, tmp.key_len, len) >= 0) {
                        start_r = mid - 1;
                    }
                    else {
                        start_l = mid + 1;
                    }
                }
                while (end_l <= end_r) {
                    mid = (end_l + end_r) / 2;
                    Pair3 tmp = bloc3[mid];
                    if (string_cmp(tmp.key, index, tmp.key_len, len) == 1) {
                        end_r = mid - 1;
                    }
                    else {
                        end_l = mid + 1;
                    }
                }
                for (int i = start_l; i <= end_r; i++) {
                    std::string tmp(bloc3[i].value, bloc3[i].value_len);
                    ret.push_back(tmp);
                }
                if (end_r != link3[p].size - 1) {
                    break;
                }
            }
            p = link3[p].nex_head;
        }
        return ret;
    }

    static int string_cmp(char* s1, char* s2, int len_1, int len_2) {
        int common_len = std::min(len_1, len_2);
        for (int i = 0; i < common_len; i++) {
            if (s1[i] > s2[i]) {
                return 1;
            }
            if (s1[i] < s2[i]) {
                return -1;
            }
        }
        if (len_1 > len_2) {
            return 1;
        }
        if (len_1 < len_2) {
            return -1;
        }
        return 0;
    }

    static int pair_cmp(Pair3 x, Pair3 y) {
        if (string_cmp(x.key, y.key, x.key_len, y.key_len) == 1) {
            return 1;
        }
        if (string_cmp(x.key, y.key, x.key_len, y.key_len) == -1) {
            return -1;
        }
        if (string_cmp(x.value, y.value, x.value_len, y.value_len) == 1) {
            return 1;
        }
        if (string_cmp(x.value, y.value, x.value_len, y.value_len) == -1) {
            return -1;
        }
        return 0;
    }
};
#endif //KEYWORD_H
