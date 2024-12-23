#ifndef NAME_H
#define NAME_H
#include <iostream>
#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

const int block_size1 = 128;

struct Pair {
    char key[61];
    char value[21];
    int key_len;
    int value_len;
};

struct HeadNode1 {
    int id;
    int prev_head;
    int nex_head;
    int size;
    int cur_index;
};

extern Pair* bloc1;
extern HeadNode1 link1[100001];

const int sizeofP0 = sizeof(Pair);
const int sizeofH0 = 4 * sizeof(int);

class NodeHead_for_name {
    fstream file;
    std::string file_name;
public:

    int new_id = -1;
    int cur_size = 0;
    int head = -1;

    NodeHead_for_name() = default;

    NodeHead_for_name(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeHead_for_name() {
        file.close();
    }

    HeadNode1 visitHead(int index) {
        HeadNode1 ret;
        file.seekp(index * sizeofH0);
        file.read(reinterpret_cast<char*>(&ret), sizeofH0);
        ret.cur_index = ret.id * block_size1;
        return ret;
    }

    static int getBlockSize(int index) {
        return link1[index].size;
    }

    void writeHead(int index, HeadNode1 ret) {
        file.seekp(index * sizeofH0);
        file.write(reinterpret_cast<char*>(&ret), sizeofH0);
    }

    int addHead(int index) {
        new_id++;
        HeadNode1 head_node{};
        if (cur_size == 0) {
            head = new_id;
            head_node.id = new_id;
            head_node.prev_head = head_node.nex_head = -1;
            head_node.size = 0;
            head_node.cur_index = new_id * block_size1;
        }
        else {
            head_node.id = new_id;
            head_node.cur_index = new_id * block_size1;
            head_node.size = 0;
            head_node.prev_head = link1[index].id;
            head_node.nex_head = link1[index].nex_head;
            link1[index].nex_head = new_id;
            if (head_node.nex_head != -1) {
                link1[head_node.nex_head].prev_head = new_id;
            }
        }
        link1[new_id] = head_node;
        cur_size++;
        return new_id;
    }

    void deleteHead(int index) {
        int prev_head = link1[index].prev_head, nex_head = link1[index].nex_head;
        if (prev_head != -1) {
            link1[prev_head].nex_head = nex_head;
        }
        else {
            head = nex_head;
        }
        if (nex_head != -1) {
            link1[nex_head].prev_head = prev_head;
        }
        cur_size--;
    }
};

class NodeBody_for_name {
    fstream file;
    std::string file_name;
public:

    NodeBody_for_name() = default;

    explicit NodeBody_for_name(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out|std::ios::binary);
        }
    }

    ~NodeBody_for_name() {
        file.close();
    }

    void visitNode(int index) {
        delete[] bloc1;
        bloc1 = new Pair[block_size1 + 1];
        file.seekp(index * sizeofP0);
        file.read(reinterpret_cast<char*>(bloc1), link1[index / block_size1].size * sizeofP0);
    }

    void writeNode(int index) {
        file.seekp(index * sizeofP0);
        file.write(reinterpret_cast<char*>(bloc1), link1[index / block_size1].size * sizeofP0);
    }
};

class name_operation {
    NodeHead_for_name Head;
    NodeBody_for_name Body;
public:
    name_operation(): Head("NodeHead_for_name"), Body("NodeBody_for_name") {
        initialise();
    }

    ~name_operation() {
        flush();
    }

    void initialise() {
        fstream file_;
        file_.open("Host_for_name", std::ios::in|std::ios::out);
        if (!file_.is_open()) {
            file_.open("Host_for_name", std::ios::out);
            file_.close();
            file_.open("Host_for_name", std::ios::in|std::ios::out|std::ios::binary);
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
                link1[p] = Head.visitHead(p);
                p = link1[p].nex_head;
            }
        }
        file_.close();
        bloc1 = new Pair[block_size1 + 1];
    }

    void flush() {
        fstream file_;
        file_.open("Host_for_name", std::ios::in|std::ios::out|std::ios::binary);
        file_.seekp(0);
        file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
        file_.close();
        int p = Head.head;
        while (p != -1) {
            Head.writeHead(p, link1[p]);
            p = link1[p].nex_head;
        }
        file_.close();

        delete[] bloc1;
    }

    static void addNode(int index, Pair data) {
        int cursor = index / block_size1;
        int cur_size = NodeHead_for_name::getBlockSize(cursor);
        int insert_place;
        Pair ret;

        int l = 0, r = cur_size - 1, mid;
        while (l <= r) {
            mid = (l + r) / 2;
            ret = bloc1[mid];
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
            bloc1[i + 1] = bloc1[i];
        }
        bloc1[insert_place] = data;
        cur_size++;
        link1[cursor].size = cur_size;
    }

    static void deleteNode(int index) {
        int cur_head = index / block_size1 * block_size1;
        int cursor = cur_head / block_size1;
        for (int i = index + 1; i < cur_head + link1[cursor].size; i++) {
            bloc1[i - 1 - cur_head] = bloc1[i - cur_head];
        }
        link1[cursor].size--;
    }

    void Insert(const std::string &name, const std::string &ISBN) {
        Pair data;
        std::strcpy(data.key, name.c_str());
        data.key_len = static_cast<int>(name.size());
        std::strcpy(data.value, ISBN.c_str());
        data.value_len = static_cast<int>(ISBN.size());

        int cur_size = Head.cur_size;
        if (cur_size == 0) {
            int id = Head.addHead(0);
            Body.visitNode(id * block_size1);
            addNode(id * block_size1, data);
            Body.writeNode(id * block_size1);
            return;
        }

        int p = Head.head, q = -1;
        int insert_type = 2;
        int nex_head, size, qsize;
        while (p != -1) {
            nex_head = link1[p].nex_head;
            size = link1[p].size;
            Body.visitNode(p * block_size1);
            if (size > 0) {
                if (pair_cmp(bloc1[0], data) == -1 && pair_cmp(data, bloc1[size - 1]) == -1) {
                    insert_type = 1;
                    break;
                }
                if (pair_cmp(bloc1[0], data) >= 0) {
                    break;
                }
            }
            q = p;
            p = nex_head;
            qsize = size;
        }

        if (insert_type == 1) {
            if (size == block_size1) {
                addNode(p * block_size1, data);

                int split_place = p;
                p = Head.addHead(split_place);
                link1[p].size = block_size1 - block_size1 / 2;
                link1[split_place].size = block_size1 / 2 + 1;

                Body.writeNode(split_place * block_size1);
                for (int i = block_size1 / 2 + 1; i < block_size1 + 1; i++) {
                    bloc1[i - block_size1 / 2 - 1] = bloc1[i];
                }
                Body.writeNode(p * block_size1);
            }
            else {
                addNode(p * block_size1, data);
                Body.writeNode(p * block_size1);
            }

        }
        if (insert_type == 2) {
            if (p != -1 && size < block_size1) {
                addNode(p * block_size1, data);
                Body.writeNode(p * block_size1);
            }
            else if (q != -1 && qsize < block_size1) {
                Body.visitNode(q * block_size1);
                addNode(q * block_size1, data);
                Body.writeNode(q * block_size1);
            }
            else {
                if (q == -1) {
                    q = p;
                }
                Body.visitNode(q * block_size1);
                addNode(q * block_size1, data);

                int split_place = q;
                q = Head.addHead(split_place);
                link1[split_place].size = block_size1 / 2 + 1;
                link1[q].size = block_size1 - block_size1 / 2;
                Body.writeNode(split_place * block_size1);
                for (int i = block_size1 / 2 + 1; i < block_size1 + 1; i++) {
                    bloc1[i - block_size1 / 2 - 1] = bloc1[i];
                }
                Body.writeNode(q * block_size1);
            }
        }
    }

    void Delete(const std::string &name, const std::string &ISBN) {
        Pair data;
        std::strcpy(data.key, name.c_str());
        data.key_len = static_cast<int>(name.size());
        std::strcpy(data.value, ISBN.c_str());
        data.value_len = static_cast<int>(ISBN.size());

        bool flag = false;
        int p = Head.head, q = -1;
        int size = 0, qsize = 0;
        while (p != -1) {
            size = link1[p].size;
            Body.visitNode(p * block_size1);
            if (size > 0) {
                if (pair_cmp(data, bloc1[0]) >= 0 && pair_cmp(bloc1[size - 1], data) >= 0) {
                    int l = 0, r = size - 1, mid;
                    while (l <= r) {
                        mid = (l + r) / 2;
                        if (pair_cmp(data, bloc1[mid]) == 0) {
                            flag = true;
                            deleteNode(link1[p].cur_index + mid);
                            break;
                        }
                        if (pair_cmp(data, bloc1[mid]) == -1) {
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
            p = link1[p].nex_head;
            qsize = size;
        }

        if (flag) {
            Body.writeNode(p * block_size1);
            if (link1[p].size == 0) {
                Head.deleteHead(p);
            }
            else if (p != -1 && q != -1 && size + qsize - 1 < block_size1) {
                Pair* tp = new Pair[size];
                for (int i = 0; i < size - 1; i++) {
                    tp[i] = bloc1[i];
                }
                Head.deleteHead(p);
                Body.visitNode(q * block_size1);
                for (int i = qsize; i < qsize + size - 1; i++) {
                    bloc1[i] = tp[i - qsize];
                }
                link1[q].size = qsize + size - 1;
                Body.writeNode(q * block_size1);
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
            Body.visitNode(p * block_size1);
            Pair head_ = bloc1[0], tail_ = bloc1[link1[p].size - 1];
            if (string_cmp(tail_.key, index, tail_.key_len, len) == 0 &&
                string_cmp(head_.key, index, head_.key_len, len) == 0) {
                for (int i = 0; i < link1[p].size; i++) {
                    std::string tmp(bloc1[i].value, bloc1[i].value_len);
                    ret.push_back(tmp);
                }
            }
            else if (string_cmp(tail_.key, index, tail_.key_len, len) >= 0 &&
                string_cmp(head_.key, index, head_.key_len, len) <= 0) {
                int start_l = 0, end_l = 0, start_r = link1[p].size - 1, end_r = link1[p].size - 1, mid;
                while (start_l <= start_r) {
                    mid = (start_l + start_r) / 2;
                    Pair tmp = bloc1[mid];
                    if (string_cmp(tmp.key, index, tmp.key_len, len) >= 0) {
                        start_r = mid - 1;
                    }
                    else {
                        start_l = mid + 1;
                    }
                }
                while (end_l <= end_r) {
                    mid = (end_l + end_r) / 2;
                    Pair tmp = bloc1[mid];
                    if (string_cmp(tmp.key, index, tmp.key_len, len) == 1) {
                        end_r = mid - 1;
                    }
                    else {
                        end_l = mid + 1;
                    }
                }
                for (int i = start_l; i <= end_r; i++) {
                    std::string tmp(bloc1[i].value, bloc1[i].value_len);
                    ret.push_back(tmp);
                }
                if (end_r != link1[p].size - 1) {
                    break;
                }
            }
            p = link1[p].nex_head;
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

    static int pair_cmp(Pair x, Pair y) {
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
#endif //NAME_H
