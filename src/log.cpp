#include "log.h"

User_Operation::User_Operation():
    Head("NodeHead_for_users"),
    Body("NodeBody_for_users") {
    initialise();
    login_stack.clear();
    current_User.privilege = 0;
    login_stack.push_back(current_User);
}

User_Operation::~User_Operation(){
    Quit();
    flush();
}

void User_Operation::initialise() {
    fstream file_;
    bloc = new User[block_size + 3];
    file_.open("Host_for_users", std::ios::in|std::ios::out);
    if (!file_.is_open()) {
        file_.open("Host_for_users", std::ios::out);
        file_.close();
        file_.open("Host_for_users", std::ios::in|std::ios::out|std::ios::binary);
        Head.new_id = -1;
        Head.cur_size = 0;
        Head.head = -1;
        file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
        Insert("root", "sjtu", "supervisor", 7);
    }
    else {
        file_.read(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
        file_.read(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
        file_.read(reinterpret_cast<char*>(&Head.head), sizeof(int));
        int p = Head.head;
        while (p != -1) {
            link[p] = Head.visitHead(p);
            p = link[p].nex_head;
        }
    }
    file_.close();
}

void User_Operation::flush() {
    fstream file_;
    file_.open("Host_for_users", std::ios::in|std::ios::out|std::ios::binary);
    file_.seekp(0);
    file_.write(reinterpret_cast<char*>(&Head.new_id), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.cur_size), sizeof(int));
    file_.write(reinterpret_cast<char*>(&Head.head), sizeof(int));
    file_.close();
    int p = Head.head;
    while (p != -1) {
        Head.writeHead(p, link[p]);
        p = link[p].nex_head;
    }

    delete[] bloc;
}

void User_Operation::addNode(int index, User data) {
    int cursor = index / block_size;
    int cur_size = link[cursor].size;
    int insert_place;
    User ret;

    int l = 0, r = cur_size - 1, mid;
    while (l <= r) {
        mid = (l + r) / 2;
        ret = bloc[mid];
        int val = string_cmp(data.UserID, ret.UserID, data.UserID_len, ret.UserID_len);
        if (val == 0) {
            std::cout << "Invalid" << '\n';
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
        bloc[i + 1] = bloc[i];
    }
    bloc[insert_place] = data;
    cur_size++;
    link[cursor].size = cur_size;
}

void User_Operation::deleteNode(int index) {
    int cur_head = index / block_size * block_size;
    int cursor = cur_head / block_size;
    for (int i = index + 1; i < cur_head + link[cursor].size; i++) {
        bloc[i - 1 - cur_head] = bloc[i - cur_head];
    }
    link[cursor].size--;
}

void User_Operation::Login(const std::string &UserID, const std::string &Password_input) {
    User data{};
    if (!checkValidity(UserID) || !checkValidity(Password_input)) {
        std::cout << "Invalid\n";
        return;
    }
    std::strcpy(data.UserID, UserID.c_str());
    data.UserID_len = static_cast<int>(UserID.size());

    int p = Head.head;
    while (p != -1) {
        int size = link[p].size;
        if (size <= 0) continue;
        Body.visitNode(p * block_size);
        if (string_cmp(data.UserID, bloc[0].UserID, data.UserID_len, bloc[0].UserID_len) != -1 &&
            string_cmp(data.UserID, bloc[size - 1].UserID, data.UserID_len, bloc[size - 1].UserID_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(data.UserID, bloc[mid].UserID, data.UserID_len, bloc[mid].UserID_len);
                if (val == 0) {
                    std::string Password(bloc[mid].Password, bloc[mid].Password_len);
                    if (Password == Password_input || Password_input.empty() && current_User.privilege > bloc[mid].privilege) {
                        bloc[mid].login = true;
                        current_User = bloc[mid];
                        if (!Book_op.book_stack.empty()) {
                            Book_op.book_stack[Book_op.book_stack.size() - 1] = Book_op.current_Book;
                        }
                        login_stack.push_back(bloc[mid]);
                        Book init{};
                        init.Quantity = -1;
                        Book_op.book_stack.push_back(init);
                        Body.writeNode(p * block_size);
                        Book_op.current_Book = init;
                    }
                    else {
                        std::cout << "Invalid" << '\n';
                    }
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
        p = link[p].nex_head;
    }
    std::cout << "Invalid\n";
}

void User_Operation::Logout() {
    if (login_stack.size() > 1) {
        User data = current_User;
        int p = Head.head;
        while (p != -1) {
            int size = link[p].size;
            if (size <= 0) continue;
            Body.visitNode(p * block_size);
            if (string_cmp(data.UserID, bloc[0].UserID, data.UserID_len, bloc[0].UserID_len) != -1 &&
                string_cmp(data.UserID, bloc[size - 1].UserID, data.UserID_len, bloc[size - 1].UserID_len) != 1) {
                int l = 0, r = size - 1, mid;
                while (l <= r) {
                    mid = (l + r) / 2;
                    int val = string_cmp(data.UserID, bloc[mid].UserID, data.UserID_len, bloc[mid].UserID_len);
                    if (val == 0) {
                        bloc[mid].login = false;
                        login_stack.pop_back();
                        Book_op.book_stack.pop_back();
                        current_User = login_stack[login_stack.size() - 1];
                        if (Book_op.book_stack.size() > 0) {
                            Book_op.current_Book = Book_op.book_stack[Book_op.book_stack.size() - 1];
                        }
                        Body.writeNode(p * block_size);
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
            p = link[p].nex_head;
        }
    }
    else {
        std::cout << "Invalid" << '\n';
    }
}

void User_Operation::Quit() {
    while (login_stack.size() > 1) {
        Logout();
    }
}

void User_Operation::Insert(const std::string &UserID,
                            const std::string &Password,
                            const std::string &UserName,
                            int privilege) {
    if (UserName.size() > 30) {
        std::cout << "Invalid" << '\n';
        return;
    }
    if (!checkValidity(UserID) || !checkValidity(Password)) {
        std::cout << "Invalid" << '\n';
        return;
    }
    User data{};
    std::strcpy(data.UserID, UserID.c_str());
    std::strcpy(data.Password, Password.c_str());
    std::strcpy(data.UserName, UserName.c_str());
    data.UserID_len = static_cast<int>(UserID.size());
    data.Password_len = static_cast<int>(Password.size());
    data.UserName_len = static_cast<int>(UserName.size());
    data.privilege = privilege;
    data.login = false;

    int cur_size = Head.cur_size;
    if (cur_size == 0) {
        int id = Head.addHead(0);
        Body.visitNode(id * block_size);
        addNode(id * block_size, data);
        Body.writeNode(id * block_size);
        return;
    }

    int p = Head.head, q = -1;
    int insert_type = 2;
    int nex_head, size, qsize;
    while (p != -1) {
        nex_head = link[p].nex_head;
        size = link[p].size;
        if (size <= 0) continue;
        Body.visitNode(p * block_size);
        if (string_cmp(bloc[0].UserID, data.UserID, bloc[0].UserID_len, data.UserID_len) == -1 &&
            string_cmp (bloc[size - 1].UserID, data.UserID, bloc[size - 1].UserID_len, data.UserID_len) == 1) {
            insert_type = 1;
            break;
        }
        if (string_cmp(bloc[0].UserID, data.UserID, bloc[0].UserID_len, data.UserID_len) != -1) {
            break;
        }
        q = p;
        p = nex_head;
        qsize = size;
    }

    if (insert_type == 1) {
        if (size == block_size) {
            addNode(p * block_size, data);
            if (link[p].size == block_size) {
                return;
            }
            int split_place = p;
            p = Head.addHead(split_place);
            link[p].size = block_size - block_size / 2;
            link[split_place].size = block_size / 2 + 1;

            Body.writeNode(split_place * block_size);
            for (int i = block_size / 2 + 1; i < block_size + 1; i++) {
                bloc[i - block_size / 2 - 1] = bloc[i];
            }
            Body.writeNode(p * block_size);
        }
        else {
            addNode(p * block_size, data);
            Body.writeNode(p * block_size);
        }
    }
    if (insert_type == 2) {
        if (p != -1 && size < block_size) {
            addNode(p * block_size, data);
            Body.writeNode(p * block_size);
        }
        else if (q != -1 && qsize < block_size) {
            Body.visitNode(q * block_size);
            addNode(q * block_size, data);
            Body.writeNode(q * block_size);
        }
        else {
            if (q == -1) {
                q = p;
            }
            Body.visitNode(q * block_size);
            addNode(q * block_size, data);
            if (link[q].size == block_size) {
                return;
            }
            int split_place = q;
            q = Head.addHead(split_place);
            link[split_place].size = block_size / 2 + 1;
            link[q].size = block_size - block_size / 2;
            Body.writeNode(split_place * block_size);
            for (int i = block_size / 2 + 1; i < block_size + 1; i++) {
                bloc[i - block_size / 2 - 1] = bloc[i];
            }
            Body.writeNode(q * block_size);
        }
    }
}

void User_Operation::Delete(const std::string &UserID) {
    User data{};
    if (!checkValidity(UserID)) {
        std::cout << "Invalid\n";
        return;
    }
    std::strcpy(data.UserID, UserID.c_str());
    data.UserID_len = static_cast<int>(UserID.size());

    bool flag = false;
    int p = Head.head, q = -1;
    int size = 0, qsize = 0;
    while (p != -1) {
        size = link[p].size;
        if (size <= 0) continue;
        Body.visitNode(p * block_size);
        if (string_cmp(data.UserID, bloc[0].UserID, data.UserID_len, bloc[0].UserID_len) != -1 &&
            string_cmp(data.UserID, bloc[size - 1].UserID, data.UserID_len, bloc[size - 1].UserID_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(data.UserID, bloc[mid].UserID, data.UserID_len, bloc[mid].UserID_len);
                if (val == 0) {
                    flag = true;
                    if (bloc[mid].login) {
                        std::cout << "Invalid" << '\n';
                        return;
                    }
                    deleteNode(link[p].cur_index + mid);
                    break;
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
        q = p;
        p = link[p].nex_head;
        qsize = size;
    }

    if (flag) {
        Body.writeNode(p * block_size);
        if (link[p].size == 0) {
            Head.deleteHead(p);
        }
        else if (p != -1 && q != -1 && size + qsize - 1 < block_size) {
            User* tp = new User[size];
            for (int i = 0; i < size - 1; i++) {
                tp[i] = bloc[i];
            }
            Head.deleteHead(p);
            Body.visitNode(q * block_size);
            for (int i = qsize; i < qsize + size - 1; i++) {
                bloc[i] = tp[i - qsize];
            }
            link[q].size = qsize + size - 1;
            Body.writeNode(q * block_size);
            delete[] tp;
        }
    }
    else {
        std::cout << "Invalid" << '\n';
    }
}

void User_Operation::changePassword(const std::string &UserID,
                                    const std::string &current_Password,
                                    const std::string &new_Password) {
    if (!checkValidity(UserID) || !checkValidity(new_Password) || !checkValidity(current_Password)) {
        std::cout << "Invalid\n";
        return;
    }
    User data{};
    std::strcpy(data.UserID, UserID.c_str());
    data.UserID_len = static_cast<int>(UserID.size());

    int p = Head.head;
    while (p != -1) {
        int size = link[p].size;
        if (size <= 0) continue;
        Body.visitNode(p * block_size);
        if (string_cmp(data.UserID, bloc[0].UserID, data.UserID_len, bloc[0].UserID_len) != -1 &&
            string_cmp(data.UserID, bloc[size - 1].UserID, data.UserID_len, bloc[size - 1].UserID_len) != 1) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = (l + r) / 2;
                int val = string_cmp(data.UserID, bloc[mid].UserID, data.UserID_len, bloc[mid].UserID_len);
                if (val == 0) {
                    std::string Password(bloc[mid].Password, bloc[mid].Password_len);
                    if (Password == current_Password || current_Password.empty() && current_User.privilege == 7) {
                        bloc[mid].Password_len = static_cast<int>(new_Password.size());
                        std::strcpy(bloc[mid].Password, new_Password.c_str());
                        Body.writeNode(p * block_size);
                    }
                    else {
                        std::cout << "Invalid" << '\n';
                    }
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
        p = link[p].nex_head;
    }
    std::cout << "Invalid" << '\n';
}

bool User_Operation::checkValidity(const std::string &str) {
    int len = static_cast<int>(str.size());
    if (str.size() > 30) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        if (!(str[i] >= '0' && str[i] <= '9' || str[i] >= 'a' && str[i] <= 'z' ||
            str[i] >= 'A' && str[i] <= 'Z' || str[i] == '_')) {
            return false;
        }
    }
    return true;
}

int User_Operation::string_cmp(char* s1, char* s2, int len_1, int len_2) {
    std::string x(s1, len_1);
    std::string y(s2, len_2);
    if (x > y) {
        return 1;
    }
    if (x < y) {
        return -1;
    }
    return 0;
}