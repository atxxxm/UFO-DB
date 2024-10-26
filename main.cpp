#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

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

    db.createTable(tableName, columns);
    std::cout << "Таблица \"" << tableName << "\" успешно создана.\n";
}

void selectTable(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    std::cin >> tableName;
    db.setCurrentTable(tableName);
    if (!db.currentTable.empty()) {
        std::cout << "Выбрана таблица \"" << tableName << "\".\n";
    }
}

void addRecord(UFO_DB& db) {
    if (db.currentTable.empty()) {
        std::cerr << "Ошибка: Текущая таблица не выбрана.\n";
        return;
    }
    std::string format, data;

    std::cout << "Введите формат данных (например, $Имя$Возраст): ";
    format = getStringInput();

    std::cout << "Введите данные (например, Иван,30): ";
    data = getStringInput();

    db.addData(format, data);
}


void printRecords(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    std::cin >> tableName;

    db.select(tableName);
}

void updateRecord(UFO_DB& db) {
    std::string tableName;
    std::cout << "Введите имя таблицы: ";
    std::cin >> tableName;

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

    db.update(tableName, id, updates);
}


void saveToFile(UFO_DB& db) {
    std::string filename;
    std::cout << "Введите имя файла для сохранения: ";
    std::cin >> filename;
    if (db.saveToFile(filename)) {
        std::cout << "Данные сохранены в файл " << filename << std::endl;
    }
}


void loadFromFile(UFO_DB& db) {
    std::string filename;
    std::cout << "Введите имя файла для загрузки: ";
    std::cin >> filename;
    if (db.loadFromFile(filename)) {
        std::cout << "Данные загружены из файла " << filename << std::endl;
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
        std::cout << "6. Сохранить в файл\n";
        std::cout << "7. Загрузить из файла\n";
        std::cout << "8. Вывести список таблиц\n";
        std::cout << "9. Выход\n";


        int choice = getIntInput();

        switch (choice) {
        case 1:
            createTable(db);
            break;
        case 2:
            selectTable(db);
            break;
        case 3:
            addRecord(db);
            break;
        case 4:
            printRecords(db);
            break;
        case 5:
            updateRecord(db);
            break;
        case 6:
            saveToFile(db);
            break;
        case 7:
            loadFromFile(db);
            break;
        case 8:
            printTables(db);
            break;
        case 9:
            return 0;
        default:
            std::cerr << "Ошибка: Неверный выбор." << std::endl;
        }
        system("pause");
    }

    return 0;
}