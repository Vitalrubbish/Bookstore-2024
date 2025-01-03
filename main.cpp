#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cassert>
#include "log.h"
#include "book.h"
#include "diary.h"
#include "finance.h"
Finance Money ("finance");
Diary Log("diary");
User_Operation User_op;
Book_Operation Book_op;
bool valid;

User* bloc = nullptr;
Book* bloc_ = nullptr;

HeadNode link[100001];
HeadNode_ link_[100001];

std::vector<std::string> Split(const std::string &original) {
    std::vector<std::string> token;
    token.clear();
    int len = static_cast<int>(original.size());
    int p = 0;
    std::string cur;
    while (p < len) {
        if (!isgraph(original[p]) && original[p] != ' ') {
            std::cout << "Invalid\n";
            valid = false;
            token.clear();
            return token;
        }
        if (original[p] == ' ') {
            if (!cur.empty()) {
                token.push_back(cur);
                cur = "";
            }
            p++;
            continue;
        }
        cur += original[p];
        p++;
    }
    if (!cur.empty()) {
        token.push_back(cur);
    }
    return token;
}

std::vector<std::string> inner_Split(const std::string &original) {
    std::vector<std::string> token;
    token.clear();
    int len = static_cast<int>(original.size());
    if (original[0] != '-') {
        return token;
    }
    int p = 1, cur;
    while (original[p] != '=' && p < len) {
        p++;
    }
    if (p == len || p == len - 1) {
        return token;
    }
    std::string tp = original.substr(1, p - 1);
    token.push_back(tp);
    if (tp == "ISBN") {
        p++;
        cur = p;
        p = len;
        token.push_back(original.substr(cur, p - cur));
    }
    if (tp == "name") {
        p++;
        if (original[p] != '\"') {
            return token;
        }
        p++;
        cur = p;
        while (p < len && original[p] != '\"') {
            p++;
        }
        if (p + 1 != len) {
            return token;
        }
        token.push_back(original.substr(cur, p - cur));
    }
    if (tp == "author") {
        p++;
        if (original[p] != '\"') {
            return token;
        }
        p++;
        cur = p;
        while (p < len && original[p] != '\"') {
            p++;
        }
        if (p + 1 != len) {
            return token;
        }
        token.push_back(original.substr(cur, p - cur));
    }
    if (tp == "keyword") {
        p++;
        if (original[p] != '\"') {
            return token;
        }
        p++;
        cur = p;
        while (p < len && original[p] != '\"') {
            p++;
        }
        if (p + 1 != len) {
            return token;
        }
        token.push_back(original.substr(cur, p - cur));
    }
    if (tp == "price") {
        p++;
        cur = p;
        p = len;
        token.push_back(original.substr(cur, p - cur));
    }
    return token;
}

int stringToInt(const std::string &str) {
    int len = static_cast<int>(str.size());
    int p = 0;
    long long ans = 0;
    while (p < len) {
        if (str[p] < '0' || str[p] > '9') {
            return -1;
        }
        ans = ans * 10 + str[p] - '0';
        p++;
    }
    if (ans > 2147483647) {
        return -1;
    }
    return static_cast<int>(ans);
}

int getType(const std::string &str) {
    if (str == "ISBN") {
        return 1;
    }
    if (str == "name") {
        return 2;
    }
    if (str == "author") {
        return 3;
    }
    if (str == "keyword") {
        return 4;
    }
    if (str == "price") {
        return 5;
    }
    return 0;
}

int main() {
    std::string op;

    while (getline(std::cin, op)) {
        valid = true;
        std::string ID(User_op.current_User.UserID, User_op.current_User.UserID_len);
        if (User_op.current_User.privilege == 0) {
            ID = "visitor";
        }
        std::vector<std::string> token = Split(op);
        if (token.empty()) {
            continue;
        }
        //todo: judge the permission to do current operation.
        if (token[0] == "quit" || token[0] == "exit") {
            if (token.size() != 1) {
                valid = false;
                std::cout << "Invalid\n";
                continue;
            }
            break;
        }
        if (token[0] == "su") {
            if (token.size() == 2) {
                User_op.Login(token[1], "");
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            if (token.size() == 3) {
                User_op.Login(token[1], token[2]);
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            std::cout << "Invalid" << '\n';
            valid = false;
        }
        else if (token[0] == "logout") {
            if (User_op.current_User.privilege < 1) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (token.size() != 1) {
                std::cout << "Invalid\n";
                valid = false;
                continue;
            }
            User_op.Logout();
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "register") {
            if (token.size() != 4) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            User_op.Insert(token[1], token[2], token[3], 1);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "passwd") {
            if (User_op.current_User.privilege < 1) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (token.size() == 3) {
                if (User_op.current_User.privilege < 7) {
                    std::cout << "Invalid" << '\n';
                    valid = false;
                    continue;
                }
                User_op.changePassword(token[1],"", token[2]);
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            if (token.size() == 4) {
                User_op.changePassword(token[1], token[2], token[3]);
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            std::cout << "Invalid" << '\n';
            valid = false;
        }
        else if (token[0] == "useradd") {
            if (User_op.current_User.privilege < 3 || token.size() != 5) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            int privilege;
            if (token[3] == "1") {
                privilege = 1;
            }
            else if (token[3] == "3") {
                privilege = 3;
            }
            else {
                std::cout << "Invalid\n";
                valid = false;
                continue;
            }
            if (User_op.current_User.privilege <= privilege) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            User_op.Insert(token[1], token[2], token[4], privilege);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "delete") {
            if (User_op.current_User.privilege < 7 || token.size() != 2) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            User_op.Delete(token[1]);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token.size() > 1 && token[0] == "show" && token[1] == "finance") {
            if (User_op.current_User.privilege < 7) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (token.size() == 2) {
                Money.readFinance(-1);
                continue;
            }
            if (token.size() == 3) {
                if (token[2].size() > 10 || stringToInt(token[2]) == -1) {
                    std::cout << "Invalid" << '\n';
                    valid = false;
                    continue;
                }
                Money.readFinance(stringToInt(token[2]));
                continue;
            }
            std::cout << "Invalid" << '\n';
            valid = false;
        }
        else if (token[0] == "show") {
            if (User_op.current_User.privilege < 1) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (token.size() == 1) {
                Book_op.Show(-1, "");
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            if (token.size() == 2) {
                std::vector<std::string> inner_token = inner_Split(token[1]);
                if (inner_token.size() < 2 || getType(inner_token[0]) == 0 || getType(inner_token[0]) == 5) {
                    std::cout << "Invalid" << '\n';
                    valid = false;
                    continue;
                }
                if (inner_token[1].empty()) {
                    std::cout << "Invalid" << '\n';
                    valid = false;
                    continue;
                }
                Book_op.Show(getType(inner_token[0]), inner_token[1]);
                if (valid) {
                    Log.addLog(ID, op);
                }
                continue;
            }
            std::cout << "Invalid" << '\n';
            valid = false;
        }
        else if (token[0] == "buy") {
            if (token.size() != 3) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (User_op.current_User.privilege < 1) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (stringToInt(token[2]) <= 0 || token[2].size() > 10) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            double cost = Book_op.Buy(token[1], stringToInt(token[2]));
            if (cost < 0) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            Money.writeFinance(1, cost);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "select") {
            if (token.size() != 2) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (User_op.current_User.privilege < 3) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            Book_op.Select(token[1]);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "modify") {
            if (User_op.current_User.privilege < 3) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            Book_op.Modify(op);
            if (valid) {
                Log.addLog(ID, op);
            }
        }
        else if (token[0] == "import") {
            if (User_op.current_User.privilege < 3) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (token.size() != 3) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            if (stringToInt(token[1]) <= 0 || token[1].size() > 10) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            double cost = Book_Operation::stringToDouble(token[2]);
            if (cost <= 0) {
                std::cout << "Invalid" << '\n';
                valid = false;
                continue;
            }
            bool check = Book_op.Import(stringToInt(token[1]));
            if (check) {
                Money.writeFinance(2, cost);
                if (valid) {
                    std::string ISBN(Book_op.current_Book.ISBN, Book_op.current_Book.ISBN_len);
                    op += " ";
                    op += ISBN;
                    Log.addLog(ID, op);
                }
            }
        }
        else if (token.size() == 2 && token[0] == "report" && token[1] == "finance" ){
            if (User_op.current_User.privilege < 7) {
                std::cout << "Invalid\n" ;
                valid = false;
                continue;
            }
            std::cout << "-------- Book Transaction Details --------" << '\n';
            Log.reportFinance();
        }
        else if (token.size() == 2 && token[0] == "report" && token[1] == "employee") {
            if (User_op.current_User.privilege < 7) {
                std::cout << "Invalid\n" ;
                valid = false;
                continue;
            }
            std::cout << "-------- Employee Operation Details --------" << '\n';
            Log.reportEmployee();
        }
        else if (token.size() == 1 && token[0] == "log") {
            std::cout << "------------ System Log ------------" << '\n';
            Log.printLog();

            std::cout << "-------- Book Transaction Details --------" << '\n';
            Log.reportFinance();
        }
        else {
            std::cout << "Invalid" << '\n';
            valid = false;
        }
    }
}


//todo 无 buy, import, show finance, delete