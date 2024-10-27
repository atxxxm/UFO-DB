#ifndef UFO_DATA_BASE_H
#define UFO_DATA_BASE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <initializer_list>
#include <sstream>
#include <fstream> 


class UFORecords {
public:
    std::unordered_map<std::string, std::string> fields;

    void add(const std::string& fieldName, const std::string& value) {
        fields[fieldName] = value;
    }

    std::string getField(const std::string& fieldName) const {
        auto it = fields.find(fieldName);
        return (it != fields.end()) ? it->second : "";
    }
};

class UFOTable {
public:
    std::string name;
    std::vector<std::string> columns;
    std::vector<UFORecords> records;
    int nextID = 1;

    UFOTable() : name(""), nextID(1) {}

    UFOTable(const std::string& tableName, const std::vector<std::string>& cols) : name(tableName), columns(cols) {}

    void insertRecord(const UFORecords& record) {
        UFORecords newRecord = record;
        newRecord.add("id", std::to_string(nextID++));
        records.push_back(newRecord);
    }

    void selectAll() const {
        printHeader();
        for (const auto& record : records) {
            printRecord(record);
        }
    }

    std::vector<UFORecords> selectWhere(const std::string& fieldName, const std::string& value) const {
        std::vector<UFORecords> results;
        std::copy_if(records.begin(), records.end(), std::back_inserter(results),
            [&](const UFORecords& record) { return record.getField(fieldName) == value; });
        return results;
    }

    void updateRecord(int id, const std::unordered_map<std::string, std::string>& updates) {
        auto it = std::find_if(records.begin(), records.end(),
            [&](const UFORecords& record) { return record.getField("id") == std::to_string(id); });

        if (it != records.end()) {
            for (const auto& update : updates) {
                it->fields[update.first] = update.second;
            }
        }
        else {
            throw std::runtime_error("Запись с id " + std::to_string(id) + " не найдена.");
        }
    }


    bool deleteRecord(int id) {
        auto it = std::remove_if(records.begin(), records.end(),
            [&](const UFORecords& record) { return record.getField("id") == std::to_string(id); });
        if (it != records.end()) {
            records.erase(it, records.end());
            return true;
        }
        return false;
    }

private:

    void printHeader() const {
        std::cout << "| " << std::left << std::setw(10) << "id";
        for (const auto& col : columns) {
            std::cout << " | " << std::left << std::setw(10) << col;
        }
        std::cout << " |\n";
        std::cout << std::string(13 + columns.size() * 13, '-') << "\n";
    }

    void printRecord(const UFORecords& record) const {
        std::cout << "| " << std::left << std::setw(10) << record.getField("id");
        for (const auto& col : columns) {
            std::cout << " | " << std::left << std::setw(10) << record.getField(col);
        }
        std::cout << " |\n";
    }
};


class UFO_DB {
public:
    std::unordered_map<std::string, UFOTable> tables;
    std::string currentTable;

    void createTable(const std::string& name, const std::vector<std::string>& columns) {
        if (tables.count(name)) {
            throw std::runtime_error("Таблица с именем " + name + " уже существует.");
        }
        tables[name] = UFOTable(name, columns);
    }

    void setCurrentTable(const std::string& tableName) {
        if (!tables.count(tableName)) {
            throw std::runtime_error("Таблица с именем " + tableName + " не существует.");
        }
        currentTable = tableName;
    }

    void insert(const std::string& tableName, const UFORecords& record) {
        if (!tables.count(tableName)) {
            throw std::runtime_error("Таблица с именем " + tableName + " не существует.");
        }
        tables[tableName].insertRecord(record);
    }


    void select(const std::string& tableName) const {
        if (!tables.count(tableName)) {
            throw std::runtime_error("Таблица с именем " + tableName + " не существует.");
        }
        tables.at(tableName).selectAll();
    }


    void addData(const std::string& tableName, const std::vector<std::string>& data) {
        if (!tables.count(tableName)) {
            throw std::runtime_error("Таблица с именем " + tableName + " не существует.");

        }
        if (data.size() != tables.at(tableName).columns.size()) {
            throw std::runtime_error("Неверное количество данных для таблицы " + tableName);
        }

        UFORecords record;
        for (size_t i = 0; i < tables.at(tableName).columns.size(); ++i) {
            record.add(tables.at(tableName).columns[i], data[i]);
        }
        tables.at(tableName).insertRecord(record);

    }


    void update(const std::string& tableName, int id, const std::unordered_map<std::string, std::string>& updates) {
        if (!tables.count(tableName)) {
            throw std::runtime_error("Таблица с именем " + tableName + " не существует.");
        }
        tables.at(tableName).updateRecord(id, updates);
    }


    bool deleteRecordFromTable(const std::string& tableName, int id) {
        if (!tables.count(tableName)) {
            return false; // Или бросить исключение
        }
        return tables.at(tableName).deleteRecord(id);
    }


    bool saveToFile(const std::string& filename) const {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для записи."); // Исключения вместо std::cerr
        }

        outfile << tables.size() << '\n';
        for (const auto& pair : tables) {
            const UFOTable& table = pair.second;
            outfile << table.name << '\n';
            outfile << table.columns.size() << '\n';
            for (const auto& col : table.columns) {
                outfile << col << '\n';
            }
            outfile << table.nextID << '\n'; // nextID, а не ID
            outfile << table.records.size() << '\n';
            for (const auto& record : table.records) {
                for (const auto& col : table.columns) {
                    outfile << record.getField(col) << "|";
                }
                outfile << record.getField("id") << '\n';
            }
        }
        return true; // Возвращаем true в случае успеха
    }


    bool loadFromFile(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для чтения.");
        }

        tables.clear();

        int numTables;
        infile >> numTables;
        infile.ignore();


        for (int i = 0; i < numTables; ++i) {
            std::string tableName;
            std::getline(infile, tableName);

            int numColumns;
            infile >> numColumns;
            infile.ignore();

            std::vector<std::string> columns(numColumns);
            for (int j = 0; j < numColumns; ++j) {
                std::getline(infile, columns[j]);
            }

            createTable(tableName, columns);


            int nextID;
            infile >> nextID;
            infile.ignore();
            tables[tableName].nextID = nextID;



            int numRecords;
            infile >> numRecords;
            infile.ignore();

            for (int k = 0; k < numRecords; ++k) {
                UFORecords record;
                std::string line;
                std::getline(infile, line);
                std::stringstream ss(line);
                std::string value;

                for (size_t c = 0; c < columns.size(); ++c) {
                    std::getline(ss, value, '|');
                    record.add(columns[c], value);

                }
                std::getline(ss, value, '|');
                record.add("id", value);
                insert(tableName, record);
            }
        }
        return true;
    }



};
#endif // !UFO_DATA_BASE_H
