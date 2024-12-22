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
        this -> file_name = file_name;
        //file.open(file_name, std::ios::in|std::ios::out);
        //if (!file.is_open()) {
            file.open(file_name, std::ios::out);
            file.close();
            file.open(file_name, std::ios::in|std::ios::out);
        //}
    }

    ~Finance() {
        file.close();
    }

     void readFinance(int count) {
        Record finance;
        file.seekp(0,std::fstream::end);
        int end = file.tellp();
        if (end < count * sizeof(Record)) {
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
        record = new Record[count];
        file.read(reinterpret_cast<char*> (record), count * sizeof(Record));
        double finance_in = 0, finance_out = 0;
        for (int i = 0; i < count; i++) {
            if (record[i].type == 1) {
                finance_in += record[i].amount;
            }
            if (record[i].type == 2) {
                finance_out += record[i].amount;
            }
        }
        printf("+ %.2f - %.2f\n",finance_in, finance_out);
        delete[] record;
    }

    void writeFinance(int type, double finance) {
        Record new_record;
        new_record.type = type;
        new_record.amount = finance;
        file.seekp(0,std::fstream::end);
        file.write(reinterpret_cast<char*> (&new_record), sizeof(Record));
    }

};
#endif //FINANCE_H