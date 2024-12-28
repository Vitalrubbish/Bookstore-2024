#ifndef FINANCE_H
#define FINANCE_H
#include <fstream>

struct Record {
    int type;
    double amount;
};

class Finance {
    std::fstream file;
    std::string file_name;
    Record* record;
public:
    Finance() = default;

    Finance(const std::string &file_name) {
        this->file_name = file_name;
        file.open(file_name, std::ios::in|std::ios::out);
        if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in | std::ios::out);
        }
        record = nullptr;
    }

    ~Finance() {
        file.close();
    }

     void readFinance(int count) {
        if (count == 0) {
            std::cout << "\n";
            return;
        }
        file.seekp(0, file.end);
        int end = file.tellp();
        if (count > 0 && end < count * sizeof(Record)) {
            std::cout << "Invalid" << '\n';
            return;
        }
        if (count != -1) {
            end -= static_cast<int>(count * sizeof(Record));
            file.seekp(end);
        } else {
            count = end / sizeof(Record);
            file.seekp(0);
        }
        double finance_in = 0, finance_out = 0;
        if (count == 0) {
            printf("+ %.2lf - %.2lf\n",finance_in, finance_out);
            return;
        }
        if (count > 0) {
            record = new Record[count + 3];
            file.read(reinterpret_cast<char*> (record), count * sizeof(Record));
            for (int i = 0; i < count; i++) {
                if (record[i].type == 1) {
                    finance_in += record[i].amount;
                }
                if (record[i].type == 2) {
                    finance_out += record[i].amount;
                }
            }
            printf("+ %.2lf - %.2lf\n",finance_in, finance_out);
            delete[] record;
        }
    }

    void writeFinance(int type, double finance) {
        Record new_record{};
        new_record.type = type;
        new_record.amount = finance;
        file.seekp(0,file.end);
        file.write(reinterpret_cast<char*> (&new_record), sizeof(Record));
    }

};
#endif //FINANCE_H