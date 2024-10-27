#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <stdexcept>

#include "UFOBase.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int getIntInput() {
    int value;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка: Введите целое число: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string getStringInput() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void printTables(const UFO_DB& db) {
    if (db.tables.empty()) {
        std::cout << "В базе данных нет таблиц.\n";
        return;
    }
    std::cout << "Таблицы в базе данных:\n";
    for (const auto& pair : db.tables) {
        std::cout << "- " << pair.first << "\n";
    }
}

void createTable(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    tableName = getStringInput();

    int numColumns;
    std::cout << "Введите количество столбцов: ";
    numColumns = getIntInput();

    std::vector<std::string> columns(numColumns);
    for (int i = 0; i < numColumns; ++i) {
        std::cout << "Введите имя столбца " << i + 1 << ": ";
        columns[i] = getStringInput();
    }

    try {
        db.createTable(tableName, columns);
        std::cout << "Таблица \"" << tableName << "\" успешно создана.\n";
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void selectTable(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    tableName = getStringInput();
    try {
        db.setCurrentTable(tableName);
        std::cout << "Выбрана таблица \"" << tableName << "\".\n";
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void addRecord(UFO_DB& db) {
    if (db.currentTable.empty()) {
        std::cerr << "Ошибка: Текущая таблица не выбрана.\n";
        return;
    }

    std::cout << "Введите данные, разделенные запятыми (например, Иван,30): ";
    std::string dataLine = getStringInput();

    std::stringstream ss(dataLine);
    std::string item;
    std::vector<std::string> data;
    while (std::getline(ss, item, ',')) {
        data.push_back(item);
    }

    try {
        db.addData(db.currentTable, data);
        std::cout << "Запись успешно добавлена.\n";
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void printRecords(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    tableName = getStringInput();

    try {
        db.select(tableName);
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void updateRecord(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    tableName = getStringInput();

    int id;
    std::cout << "Введите id записи для обновления: ";
    id = getIntInput();

    int numUpdates;
    std::cout << "Введите количество полей для обновления: ";
    numUpdates = getIntInput();

    std::unordered_map<std::string, std::string> updates;
    for (int i = 0; i < numUpdates; ++i) {
        std::string fieldName, fieldValue;
        std::cout << "Введите имя поля: ";
        fieldName = getStringInput();
        std::cout << "Введите значение: ";
        fieldValue = getStringInput();
        updates[fieldName] = fieldValue;
    }

    try {
        db.update(tableName, id, updates);
        std::cout << "Запись успешно обновлена.\n";
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void deleteRecord(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    tableName = getStringInput();

    int id;
    std::cout << "Введите ID записи для удаления: ";
    id = getIntInput();

    try {
        if (db.deleteRecordFromTable(tableName, id)) {
            std::cout << "Запись с ID " << id << " успешно удалена из таблицы " << tableName << std::endl;
        }
        else {
            std::cerr << "Ошибка: Запись с ID " << id << " не найдена в таблице " << tableName << std::endl;
        }
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка: " << error.what() << std::endl;
    }
}

void saveToFile(UFO_DB& db) {
    std::string filename;
    std::cout << "Введите имя файла для сохранения: ";
    filename = getStringInput(); 

    try {
        if (db.saveToFile(filename)) {
            std::cout << "Данные сохранены в файл " << filename << std::endl;
        }
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка при сохранении: " << error.what() << std::endl;
    }
}


void loadFromFile(UFO_DB& db) {
    std::string filename;
    std::cout << "Введите имя файла для загрузки: ";
    filename = getStringInput(); 

    try {
        if (db.loadFromFile(filename)) {
            std::cout << "Данные загружены из файла " << filename << std::endl;
        }
    }
    catch (const std::runtime_error& error) {
        std::cerr << "Ошибка при загрузке: " << error.what() << std::endl;
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    UFO_DB db;

    while (true) {
        clearScreen();
        std::cout << "Выберите действие:\n";
        std::cout << "1. Создать таблицу\n";
        std::cout << "2. Выбрать таблицу\n";
        std::cout << "3. Добавить запись\n";
        std::cout << "4. Вывести все записи\n";
        std::cout << "5. Обновить запись\n";
        std::cout << "6. Удалить запись\n"; 
        std::cout << "7. Сохранить в файл\n";
        std::cout << "8. Загрузить из файла\n";
        std::cout << "9. Вывести список таблиц\n";
        std::cout << "0. Выход\n";

        int choice = getIntInput();

        switch (choice) {
        case 1: createTable(db); break;
        case 2: selectTable(db); break;
        case 3: addRecord(db); break;
        case 4: printRecords(db); break;
        case 5: updateRecord(db); break;
        case 6: deleteRecord(db); break; 
        case 7: saveToFile(db); break;
        case 8: loadFromFile(db); break;
        case 9: printTables(db); break;
        case 0: return 0;
        default: std::cerr << "Ошибка: Неверный выбор.\n";
        }
        std::cin.get();
    }

    return 0;
}
