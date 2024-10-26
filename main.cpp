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
        std::cout << "������: ������� ����� �����: ";
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
        std::cout << "� ���� ������ ��� ������.\n";
        return;
    }
    std::cout << "������� � ���� ������:\n";
    for (const auto& pair : db.tables) {
        std::cout << "- " << pair.first << "\n";
    }
}

void createTable(UFO_DB& db) {
    std::string tableName;
    std::cout << "������� ��� �������: ";
    tableName = getStringInput();

    int numColumns;
    std::cout << "������� ���������� ��������: ";
    numColumns = getIntInput();

    std::vector<std::string> columns(numColumns);
    for (int i = 0; i < numColumns; ++i) {
        std::cout << "������� ��� ������� " << i + 1 << ": ";
        columns[i] = getStringInput();
    }

    db.createTable(tableName, columns);
    std::cout << "������� \"" << tableName << "\" ������� �������.\n";
}

void selectTable(UFO_DB& db) {
    std::string tableName;
    std::cout << "������� ��� �������: ";
    std::cin >> tableName;
    db.setCurrentTable(tableName);
    if (!db.currentTable.empty()) {
        std::cout << "������� ������� \"" << tableName << "\".\n";
    }
}

void addRecord(UFO_DB& db) {
    if (db.currentTable.empty()) {
        std::cerr << "������: ������� ������� �� �������.\n";
        return;
    }
    std::string format, data;

    std::cout << "������� ������ ������ (��������, $���$�������): ";
    format = getStringInput();

    std::cout << "������� ������ (��������, ����,30): ";
    data = getStringInput();

    db.addData(format, data);
}


void printRecords(UFO_DB& db) {
    std::string tableName;
    std::cout << "������� ��� �������: ";
    std::cin >> tableName;

    db.select(tableName);
}

void updateRecord(UFO_DB& db) {
    std::string tableName;
    std::cout << "������� ��� �������: ";
    std::cin >> tableName;

    int id;
    std::cout << "������� id ������ ��� ����������: ";
    id = getIntInput();

    int numUpdates;
    std::cout << "������� ���������� ����� ��� ����������: ";
    numUpdates = getIntInput();


    std::unordered_map<std::string, std::string> updates;
    for (int i = 0; i < numUpdates; ++i) {
        std::string fieldName, fieldValue;
        std::cout << "������� ��� ����: ";
        fieldName = getStringInput();
        std::cout << "������� ��������: ";
        fieldValue = getStringInput();

        updates[fieldName] = fieldValue;
    }

    db.update(tableName, id, updates);
}


void saveToFile(UFO_DB& db) {
    std::string filename;
    std::cout << "������� ��� ����� ��� ����������: ";
    std::cin >> filename;
    if (db.saveToFile(filename)) {
        std::cout << "������ ��������� � ���� " << filename << std::endl;
    }
}


void loadFromFile(UFO_DB& db) {
    std::string filename;
    std::cout << "������� ��� ����� ��� ��������: ";
    std::cin >> filename;
    if (db.loadFromFile(filename)) {
        std::cout << "������ ��������� �� ����� " << filename << std::endl;
    }
}





int main() {
    setlocale(LC_ALL, "rus");

    UFO_DB db;


    while (true) {
        clearScreen();
        std::cout << "�������� ��������:\n";
        std::cout << "1. ������� �������\n";
        std::cout << "2. ������� �������\n";
        std::cout << "3. �������� ������\n";
        std::cout << "4. ������� ��� ������\n";
        std::cout << "5. �������� ������\n";
        std::cout << "6. ��������� � ����\n";
        std::cout << "7. ��������� �� �����\n";
        std::cout << "8. ������� ������ ������\n";
        std::cout << "9. �����\n";


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
            std::cerr << "������: �������� �����." << std::endl;
        }
        system("pause");
    }

    return 0;
}