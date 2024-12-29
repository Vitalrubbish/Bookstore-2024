#ifndef DIARY_H
#define DIARY_H

#include <fstream>

std::vector <std::string> Split(const std::string &);
int stringToInt(const std::string &);

struct Operation {
    char UserID[31];
    char op[301];
    int UserID_len;
    int op_len;
};

extern User_Operation User_op;
extern Book_Operation Book_op;

class Diary {
    fstream file;
    std::string file_name;
    Operation *operation;
public:
    Diary() = default;

    Diary(const std::string &file_name) {
        this -> file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out);
        }
        operation = nullptr;
    }

    ~Diary() {
        file.close();
    }

    void addLog(const std::string &UserID, const std::string &op) {
        Operation cur{};
        std::strcpy(cur.UserID, UserID.c_str());
        std::strcpy(cur.op, op.c_str());
        cur.UserID_len = static_cast<int>(UserID.size());
        cur.op_len = static_cast<int>(op.size());
        file.seekp(0, file.end);
        file.write(reinterpret_cast<char*>(&cur), sizeof(Operation));
    }

    void printLog() {
        file.seekp(0, file.end);
        int end = file.tellp();
        int sz = static_cast<int>(end / sizeof(Operation));
        operation = new Operation[sz];
        file.seekp(0);
        file.read(reinterpret_cast<char*>(operation), sz * sizeof(Operation));
        for (int i = 0; i < sz; i++) {
            std::string UserID(operation[i].UserID, operation[i].UserID_len);
            std::string op(operation[i].op, operation[i].op_len);
            std::cout << UserID << '\t' << op << '\n';
        }
        delete[] operation;
    }

    void reportEmployee() {
        file.seekp(0, file.end);
        int end = file.tellp();
        int sz = static_cast<int>(end / sizeof(Operation));
        operation = new Operation[sz];
        file.seekp(0);
        file.read(reinterpret_cast<char*>(operation), sz * sizeof(Operation));
        for (int i = 0; i < sz; i++) {
            std::string UserID(operation[i].UserID, operation[i].UserID_len);
            std::string op(operation[i].op, operation[i].op_len);
            if (User_op.getUser(UserID).privilege == 3) {
                std::cout << UserID << '\t' << op << '\n';
            }
        }
        delete[] operation;
    }

    void reportFinance() {
        file.seekp(0, file.end);
        int end = file.tellp();
        int sz = static_cast<int>(end / sizeof(Operation));
        operation = new Operation[sz];
        file.seekp(0);
        file.read(reinterpret_cast<char*>(operation), sz * sizeof(Operation));
        for (int i = 0; i < sz; i++) {
            std::string UserID(operation[i].UserID, operation[i].UserID_len);
            std::string op(operation[i].op, operation[i].op_len);
            std::vector <std::string> token = Split(op);
            if (token[0] == "buy") {
                double earn = Book_op.getBook(token[1]).price * stringToInt(token[2]);
                std::cout << token[0] << "\t" << token[1] << '\t' << token[2] << '\t' << earn << '\n';
            }
            if (token[0] == "import") {
                std::cout << token[0] << "\t" << token[3] << '\t' << token[1] << '\t' << token[2] << '\n';
            }
        }
        delete[] operation;
    }
};

#endif //DIARY_H
