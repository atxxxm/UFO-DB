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

class UFORecords 
{
public:
    std::unordered_map<std::string, std::string> fields;

    void add(const std::initializer_list<std::pair<std::string, std::string>>& pairs) 
    {
        for (const auto& pair : pairs) {
            fields[pair.first] = pair.second;
        }
    }

    std::string getField(const std::string& fieldName) const 
    {
        auto it = fields.find(fieldName);
        return it != fields.end() ? it->second : "";
    }

};

class UFOTable 
{
public:
    std::string name;
    std::vector<std::string> columns;
    std::vector<UFORecords> records;
    int ID = 1; 

    UFOTable() : name(""), columns() {}

    UFOTable(const std::string& name, const std::vector<std::string>& columns)
        : name(name), columns(columns) {}

    void insertRecord(UFORecords record) 
    {
        record.add({ {"id", std::to_string(ID)} });
        records.push_back(record);
        ID++;
    }

    void selectAll() const 
    {
        // Заголовок
        std::cout << "|" << std::left << std::setw(10) << "id";
        for (const auto& column : columns) {
            std::cout << "|" << std::left << std::setw(10) << column;
        }
        std::cout << "|\n";

        // Разделитель
        std::cout << std::string(10 + columns.size() * 11, '-') << "\n"; 

        // Записи
        for (const auto& record : records) 
        {
            std::cout << "|" << std::left << std::setw(10) << record.getField("id"); 
            for (const auto& column : columns) {
                std::cout << "|" << std::left << std::setw(10) << record.getField(column); 
            }
            std::cout << "|\n";
        }
    }

    void updateRecord(int id, const std::unordered_map<std::string, std::string>& updates) {
        for (auto& record : records) {
            if (record.getField("id") == std::to_string(id)) {
                for (const auto& pair : updates) {
                    record.fields[pair.first] = pair.second;
                }
                return;
            }
        }
        std::cerr << "Ошибка: запись с id " << id << " не найдена." << std::endl;
    }
};

class UFO_DB
{
public:
    std::unordered_map<std::string, UFOTable> tables;
    std::string currentTable;

    void createTable(const std::string& name, const std::vector<std::string>& columns) 
    {
        tables[name] = UFOTable(name, columns);
    }

    void setCurrentTable(const std::string& tableName) 
    {
        currentTable = tableName;
    }

    void insert(const std::string& TableName, const UFORecords& record)
    {
        tables[TableName].insertRecord(record);
    }

    void select(const std::string& tableName) 
    {
        tables[tableName].selectAll();
    }

    void addData(const std::string& format, const std::string& data) 
    {
        if (currentTable.empty()) 
        {
            std::cerr << "Ошибка: Текущая таблица не найдена." << std::endl;
            return;
        }

        std::vector<std::string> formatParts;
        std::stringstream formatStream(format);
        std::string part;
        while (std::getline(formatStream, part, '$')) {
            if (!part.empty()) {
                formatParts.push_back(part);
            }
        }


        std::vector<std::string> dataParts;
        std::stringstream dataStream(data);
        std::string dataPart;
        while (std::getline(dataStream, dataPart, ',')) 
        { 
            dataParts.push_back(dataPart);

        }

        if (formatParts.size() != dataParts.size()) 
        {
            std::cerr << "Ошибка: количество параметров формата и данных не совпадает." << std::endl;
            return;
        }

        UFORecords record;
        for (size_t i = 0; i < formatParts.size(); ++i) {
            std::string fieldName = formatParts[i];
            std::string fieldValue = dataParts[i];

            fieldName = fieldName.substr(fieldName.find_first_not_of(" ")); 
            fieldName = fieldName.substr(0, fieldName.find_last_not_of(" ") + 1);  
            fieldValue = fieldValue.substr(fieldValue.find_first_not_of(" "));

            record.add({ {fieldName, fieldValue} });

        }


        insert(currentTable, record);
    }

    void update(const std::string& tableName, int id, const std::unordered_map<std::string, std::string>& updates) {
        if (tables.find(tableName) == tables.end()) {
            std::cerr << "Ошибка: таблица " << tableName << " не найдена." << std::endl;
            return;
        }
        tables[tableName].updateRecord(id, updates);
    }

    bool saveToFile(const std::string& filename) const {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Ошибка: Не удалось открыть файл для записи." << std::endl;
            return false;
        }

        outfile << tables.size() << '\n';

        for (const auto& pair : tables) {
            const UFOTable& table = pair.second;
            outfile << table.name << '\n';
            outfile << table.columns.size() << '\n';

            for (const auto& col : table.columns) {
                outfile << col << '\n';
            }

            outfile << table.ID << '\n'; 
            outfile << table.records.size() << '\n';

            for (const auto& record : table.records) {
                for (const auto& col : table.columns) {
                    outfile << record.getField(col) << "|";
                }
                outfile << record.getField("id") << '\n'; 
            }
        }

        outfile.close(); 
        return true;
    }

    bool loadFromFile(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            std::cerr << "Ошибка: Не удалось открыть файл для чтения." << std::endl;
            return false;
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

            std::vector<std::string> columns;
            for (int j = 0; j < numColumns; ++j) {
                std::string column;
                std::getline(infile, column);
                columns.push_back(column);
            }

            createTable(tableName, columns);
            int id_value;
            infile >> id_value;
            infile.ignore();
            tables[tableName].ID = id_value;


            int numRecords;
            infile >> numRecords;
            infile.ignore();

            for (int k = 0; k < numRecords; ++k) {
                UFORecords record;
                for (size_t c = 0; c < columns.size(); ++c) {
                    std::string value;
                    std::getline(infile, value);

                    record.add({ {columns[c], value } });

                }
                std::string value;
                std::getline(infile, value);

                record.add({ {"id", value } });
                insert(tableName, record);
            }
        }

        infile.close();
        return true;
    }

    

};

#endif // !UFO_DATA_BASE_H
