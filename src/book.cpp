#include "book.h"

Book_Operation::Book_Operation():
    Head("NodeHead_for_books"),
    Body("NodeBody_for_books") {
    select = false;
    initialise();
}

Book_Operation::~Book_Operation() {
    flush();
}

void Book_Operation::initialise() {
    fstream file_;
    file_.open("Host_for_Books", std::ios::in|std::ios::out);
    if (!file_.is_open()) {
        file_.open("Host_for_Books", std::ios::out);
        file_.close();
        file_.open("Host_for_Books", std::ios::in|std::ios::out|std::ios::binary);
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
            link_[p] = Head.visitHead(p);
            p = link_[p].nex_head;
        }
    }
    file_.close();
    bloc_ = new Book[block_size_ + 1];
}

void Book_Operation::flush() {
    fstream file_;
    file_.open("Host_for_Books", std::ios::in|std::ios::out|std::ios::binary);
    file_.seekp(0);
    file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
    file_.close();
    int p = Head.head;
    while (p != -1) {
        Head.writeHead(p, link_[p]);
        p = link_[p].nex_head;
    }
    file_.close();

    delete[] bloc_;
}

void Book_Operation::addNode(int index, Book data) {
    int cursor = index / block_size_;
    int cur_size = Head.getBlockSize(cursor);
    int insert_place;
    Book ret;

    int l = 0, r = cur_size - 1, mid;
    while (l <= r) {
        mid = (l + r) / 2;
        ret = bloc_[mid];
        int val = string_cmp(data.ISBN, ret.ISBN, data.ISBN_len, ret.ISBN_len);
        if (val == 0) {
            return;
        }
        if (val == -1) {
            r = mid - 1;
        }
        else {
            l = mid + 1;
        }
    }
    insert_place = l;

    for (int i = cur_size - 1; i >= insert_place; i--) {
        bloc_[i + 1] = bloc_[i];
    }
    bloc_[insert_place] = data;
    cur_size++;
    link_[cursor].size = cur_size;
}

void Book_Operation::deleteNode(int index) {
    int cur_head = index / block_size_ * block_size_;
    int cursor = cur_head / block_size_;
    for (int i = index + 1; i < cur_head + link_[cursor].size; i++) {
        bloc_[i - 1 - cur_head] = bloc_[i - cur_head];
    }
    link_[cursor].size--;
}

void Book_Operation::Select(const std::string &ISBN) {
    Book ret = getBook(ISBN);
    if (ret.Quantity == -1) {
        Insert(ISBN);
        ret = getBook(ISBN);
    }
    current_Book = ret;
    select = true;
}

double Book_Operation::Buy(const std::string &ISBN, int Quantity) {
    if (!select) {
        return 0;
    }
    Book data;
    std::strcpy(data.ISBN, ISBN.c_str());
    data.ISBN_len = static_cast<int>(ISBN.size());
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(data.ISBN, bloc_[0].ISBN, data.ISBN_len, bloc_[0].ISBN_len) != -1 &&
            string_cmp(data.ISBN, bloc_[size - 1].ISBN, data.ISBN_len, bloc_[size - 1].ISBN_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(data.ISBN, bloc_[mid].ISBN, data.ISBN_len, bloc_[mid].ISBN_len);
                if (val == 0) {
                    if (bloc_[mid].Quantity < Quantity) {
                        return 0;
                    }
                    bloc_[mid].Quantity -= Quantity;
                    printf("%.2f\n", bloc_[mid].price * Quantity);
                    Body.writeNode(p * block_size_);
                    return bloc_[mid].price * Quantity;
                }
                if (val == -1) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }
            break;
            }
        p = link_[p].nex_head;
    }
    return 0;
}

bool Book_Operation::Import(int Quantity) {
    if (!select) {
        std::cout << "Invalid" << '\n';
        return false;
    }
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(current_Book.ISBN, bloc_[0].ISBN, current_Book.ISBN_len, bloc_[0].ISBN_len) != -1 &&
            string_cmp(current_Book.ISBN, bloc_[size - 1].ISBN, current_Book.ISBN_len, bloc_[size - 1].ISBN_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(current_Book.ISBN, bloc_[mid].ISBN, current_Book.ISBN_len, bloc_[mid].ISBN_len);
                if (val == 0) {
                    bloc_[mid].Quantity += Quantity;
                    Body.writeNode(p * block_size_);
                    return true;
                }
                if (val == -1) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }
            break;
        }
        p = link_[p].nex_head;
    }
    return false;
}

void Book_Operation::Modify(int type, const std::string &info) {
    if (!select) {
        std::cout << "Invalid" << '\n';
        return;
    }
    if (type == 1) {
        if (getBook(info).Quantity != -1) {
            std::cout << "Invalid" << '\n';
            return;
        }
        Delete();
        std::strcpy(current_Book.ISBN, info.c_str());
        Insert_(current_Book);
        return;
    }
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(current_Book.ISBN, bloc_[0].ISBN, current_Book.ISBN_len, bloc_[0].ISBN_len) != -1 &&
            string_cmp(current_Book.ISBN, bloc_[size - 1].ISBN, current_Book.ISBN_len, bloc_[size - 1].ISBN_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(current_Book.ISBN, bloc_[mid].ISBN, current_Book.ISBN_len, bloc_[mid].ISBN_len);
                if (val == 0) {
                    if (type == 2) {
                        std::strcpy(bloc_[mid].BookName, info.c_str());
                        bloc_[mid].BookName_len = static_cast<int>(info.size());
                    }
                    if (type == 3) {
                        std::strcpy(bloc_[mid].Author, info.c_str());
                        bloc_[mid].Author_len = static_cast<int>(info.size());
                    }
                    if (type == 4) {
                        //todo: check the validity of info
                        std::strcpy(bloc_[mid].Keyword, info.c_str());
                        bloc_[mid].Keyword_len = static_cast<int>(info.size());
                    }
                    if (type == 5) {
                        bloc_[mid].price = stringToDouble(info);
                    }
                    Body.writeNode(p * block_size_);
                    return;
                }
                if (val == -1) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }
            break;
        }
        p = link_[p].nex_head;
    }
}

Book Book_Operation::getBook(const std::string &ISBN) {
    Book data;
    std::strcpy(data.ISBN, ISBN.c_str());
    data.ISBN_len = static_cast<int>(ISBN.size());
    data.Quantity = -1;
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(data.ISBN, bloc_[0].ISBN, data.ISBN_len, bloc_[0].ISBN_len) != -1 &&
            string_cmp(data.ISBN, bloc_[size - 1].ISBN, data.ISBN_len, bloc_[size - 1].ISBN_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(data.ISBN, bloc_[mid].ISBN, data.ISBN_len, bloc_[mid].ISBN_len);
                if (val == 0) {
                    return bloc_[mid];
                }
                if (val == -1) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }
            break;
            }
        p = link_[p].nex_head;
    }
    return data;
}

void Book_Operation::Show(int type, const std::string &info) {
    bool put = false;
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        for (int i = 0; i < size; i++) {
            bool flag = false;
            if (type == -1) {
                flag = true;
            }
            if (type == 1) {
                std::string info_(bloc_[i].ISBN, bloc_[i].ISBN_len);
                if (info_ == info) {
                    flag = true;
                }
            }
            if (type == 2) {
                std::string info_(bloc_[i].BookName, bloc_[i].BookName_len);
                if (info_ == info) {
                    flag = true;
                }
            }
            if (type == 3) {
                std::string info_(bloc_[i].Author, bloc_[i].Author_len);
                if (info_ == info) {
                    flag = true;
                }
            }
            if (type == 4) {
                std::string info_(bloc_[i].Keyword, bloc_[i].Keyword_len);
                std::vector<std::string> KeyToken = Key_Split(info_);
                for (int j = 0; j < KeyToken.size(); j++) {
                    if (info == KeyToken[j]) {
                        flag = true;
                        break;
                    }
                }
            }
            if (flag) {
                put = true;
                std::string ISBN(bloc_[i].ISBN, bloc_[i].ISBN_len);
                std::string BookName(bloc_[i].BookName, bloc_[i].BookName_len);
                std::string Author(bloc_[i].Author, bloc_[i].Author_len);
                std::string Keyword(bloc_[i].Keyword, bloc_[i].Keyword_len);
                std::cout << ISBN << '\t'
                          << BookName << '\t'
                          << Author << '\t'
                          << Keyword << '\t';
                printf("%.2f\t", bloc_[i].price);
                std::cout << bloc_[i].Quantity << '\n';
            }
        }
        p = link_[p].nex_head;
    }
    if (!put) {
        std::cout << '\n';
    }
}

void Book_Operation::Insert(const std::string &ISBN) {
    Book data;
    std::strcpy(data.ISBN, ISBN.c_str());
    data.ISBN_len = static_cast<int>(ISBN.size());
    int cur_size = Head.cur_size;
    if (cur_size == 0) {
        int id = Head.addHead(0);
        Body.visitNode(id * block_size_);
        addNode(id * block_size_, data);
        Body.writeNode(id * block_size_);
        return;
    }
    int p = Head.head, q = -1;
    int insert_type = 2;
    int nex_head, size, qsize;
    while (p != -1) {
        nex_head = link_[p].nex_head;
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(bloc_[0].ISBN, data.ISBN, bloc_[0].ISBN_len, data.ISBN_len) == -1 &&
            string_cmp (bloc_[size - 1].ISBN, data.ISBN, bloc_[size - 1].ISBN_len, data.ISBN_len) == 1) {
            insert_type = 1;
            break;
            }
        if (string_cmp(bloc_[0].ISBN, data.ISBN, bloc_[0].ISBN_len, data.ISBN_len) != -1) {
            break;
        }
        q = p;
        p = nex_head;
        qsize = size;
    }
    if (insert_type == 1) {
        if (size == block_size_) {
            addNode(p * block_size_, data);

            int split_place = p;
            p = Head.addHead(split_place);
            link_[p].size = block_size_ - block_size_ / 2;
            link_[split_place].size = block_size_ / 2 + 1;

            Body.writeNode(split_place * block_size_);
            for (int i = block_size_ / 2 + 1; i < block_size_ + 1; i++) {
                bloc_[i - block_size_ / 2 - 1] = bloc_[i];
            }
            Body.writeNode(p * block_size_);
        }
        else {
            addNode(p * block_size_, data);
            Body.writeNode(p * block_size_);
        }
    }
    if (insert_type == 2) {
        if (p != -1 && size < block_size_) {
            addNode(p * block_size_, data);
            Body.writeNode(p * block_size_);
        }
        else if (q != -1 && qsize < block_size_) {
            Body.visitNode(q * block_size_);
            addNode(q * block_size_, data);
            Body.writeNode(q * block_size_);
        }
        else {
            if (q == -1) {
                q = p;
            }
            Body.visitNode(q * block_size_);
            addNode(q * block_size_, data);

            int split_place = q;
            q = Head.addHead(split_place);
            link_[split_place].size = block_size_ / 2 + 1;
            link_[q].size = block_size_ - block_size_ / 2;
            Body.writeNode(split_place * block_size_);
            for (int i = block_size_ / 2 + 1; i < block_size_ + 1; i++) {
                bloc_[i - block_size_ / 2 - 1] = bloc_[i];
            }
            Body.writeNode(q * block_size_);
        }
    }
}

void Book_Operation::Insert_(Book data) {
    int cur_size = Head.cur_size;
    if (cur_size == 0) {
        int id = Head.addHead(0);
        Body.visitNode(id * block_size_);
        addNode(id * block_size_, data);
        Body.writeNode(id * block_size_);
        return;
    }
    int p = Head.head, q = -1;
    int insert_type = 2;
    int nex_head, size, qsize;
    while (p != -1) {
        nex_head = link_[p].nex_head;
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(bloc_[0].ISBN, data.ISBN, bloc_[0].ISBN_len, data.ISBN_len) == -1 &&
            string_cmp (bloc_[size - 1].ISBN, data.ISBN, bloc_[size - 1].ISBN_len, data.ISBN_len) == 1) {
            insert_type = 1;
            break;
            }
        if (string_cmp(bloc_[0].ISBN, data.ISBN, bloc_[0].ISBN_len, data.ISBN_len) != -1) {
            break;
        }
        q = p;
        p = nex_head;
        qsize = size;
    }
    if (insert_type == 1) {
        if (size == block_size_) {
            addNode(p * block_size_, data);

            int split_place = p;
            p = Head.addHead(split_place);
            link_[p].size = block_size_ - block_size_ / 2;
            link_[split_place].size = block_size_ / 2 + 1;

            Body.writeNode(split_place * block_size_);
            for (int i = block_size_ / 2 + 1; i < block_size_ + 1; i++) {
                bloc_[i - block_size_ / 2 - 1] = bloc_[i];
            }
            Body.writeNode(p * block_size_);
        }
        else {
            addNode(p * block_size_, data);
            Body.writeNode(p * block_size_);
        }
    }
    if (insert_type == 2) {
        if (p != -1 && size < block_size_) {
            addNode(p * block_size_, data);
            Body.writeNode(p * block_size_);
        }
        else if (q != -1 && qsize < block_size_) {
            Body.visitNode(q * block_size_);
            addNode(q * block_size_, data);
            Body.writeNode(q * block_size_);
        }
        else {
            if (q == -1) {
                q = p;
            }
            Body.visitNode(q * block_size_);
            addNode(q * block_size_, data);

            int split_place = q;
            q = Head.addHead(split_place);
            link_[split_place].size = block_size_ / 2 + 1;
            link_[q].size = block_size_ - block_size_ / 2;
            Body.writeNode(split_place * block_size_);
            for (int i = block_size_ / 2 + 1; i < block_size_ + 1; i++) {
                bloc_[i - block_size_ / 2 - 1] = bloc_[i];
            }
            Body.writeNode(q * block_size_);
        }
    }
}

void Book_Operation::Delete() {
    int p = Head.head;
    int size;
    while (p != -1) {
        size = link_[p].size;
        Body.visitNode(p * block_size_);
        if (string_cmp(current_Book.ISBN, bloc_[0].ISBN, current_Book.ISBN_len, bloc_[0].ISBN_len) != -1 &&
            string_cmp(current_Book.ISBN, bloc_[size - 1].ISBN, current_Book.ISBN_len, bloc_[size - 1].ISBN_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(current_Book.ISBN, bloc_[mid].ISBN, current_Book.ISBN_len, bloc_[mid].ISBN_len);
                if (val == 0) {
                    deleteNode(p * block_size_ + mid);
                    Body.writeNode(p * block_size_);
                    return;
                }
                if (val == -1) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }
            break;
        }
        p = link_[p].nex_head;
    }
}

std::vector<std::string> Book_Operation::Key_Split(const std::string &str) {
    std::vector<std::string> ret;
    int len = static_cast<int>(str.size());
    int p = 0;
    std::string cur;
    while (p < len) {
        if (str[p] == '|') {
            ret.push_back(cur);
            cur = "";
        }
        else {
            cur += str[p];
        }
        p++;
    }
    ret.push_back(cur);
    return ret;
}

int Book_Operation::string_cmp(char* s1, char* s2, int len_1, int len_2) {
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

double Book_Operation::stringToDouble(const std::string &str) {
    double ret = 0;
    int len = static_cast<int>(str.size());
    int p = 0;
    while (str[p] != '.' && p < len) {
        ret = ret * 10 + str[p] - '0';
        p++;
    }
    p++;
    if (len == p + 1) {
        ret = ret + (str[p] - '0') * 0.1;
    }
    if (len == p + 2) {
        ret = ret + (str[p] - '0') * 0.1 + (str[p + 1] - '0') * 0.01;
    }
    return ret;
}