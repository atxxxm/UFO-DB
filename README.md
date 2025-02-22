![image](https://upload.wikimedia.org/wikipedia/commons/thumb/6/66/Noto_Emoji_Pie_1f6f8.svg/1024px-Noto_Emoji_Pie_1f6f8.svg.png)


# Документация библиотеки UFO DB

Данная документация описывает класс `UFO_DB`, предназначенный для работы с базой данных, хранящейся в виде таблиц.

## Класс `UFO_DB`

Класс `UFO_DB` представляет собой базу данных, состоящую из набора таблиц. Он позволяет создавать таблицы, добавлять записи, обновлять записи и сохранять/загружать данные из файлов.

### Методы

* **`createTable(const std::string& name, const std::vector<std::string>& columns)`**: Создает новую таблицу в базе данных.

    * `name`: Имя создаваемой таблицы (строка).
    * `columns`: Вектор имен столбцов таблицы (строки).
    * Возвращает: `void`
    * **Пример:**
        ```C++
        UFO_DB db;
        std::vector<std::string> columns = {"Имя", "Возраст"};
        db.createTable("Люди", columns);
        ```

* **`setCurrentTable(const std::string& tableName)`**: Устанавливает текущую таблицу базы данных.

    * `tableName`: Имя таблицы (строка).
    * Возвращает: `void`
    * **Пример:**
        ```C++
        UFO_DB db;
        db.setCurrentTable("Люди");
        ```

* **`addData(const std::string& format, const std::string& data)`**: Добавляет новую запись в текущую таблицу.

    * `format`: Строка, описывающая структуру данных в виде разделителей (например, `$Имя$Возраст`).
    * `data`: Данные для записи (строка, содержащая значения для столбцов, разделенные запятыми, например, "Иван,30").
    * Возвращает: `void`
    * **Пример:**
        ```C++
        UFO_DB db;
        db.setCurrentTable("Люди");
        db.addData("$Имя$Возраст", "Иван,30");
        ```

* **`select(const std::string& tableName)`**: Выводит все записи из указанной таблицы.

    * `tableName`: Имя таблицы (строка).
    * Возвращает: `void`
    * **Пример:**
        ```C++
        UFO_DB db;
        db.select("Люди");
        ```

* **`insert(const std::string& TableName, const UFORecords& record)`**: Вставляет запись в указанную таблицу.
    * `TableName`: Имя таблицы (строка).
    * `record`: Объект `UFORecords` с данными для записи.

* **`update(const std::string& tableName, int id, const std::unordered_map<std::string, std::string>& updates)`**: Обновляет запись в таблице.

    * `tableName`: Имя таблицы (строка).
    * `id`: Идентификатор записи для обновления (целое число).
    * `updates`: Словарь пар "имя поля" - "новое значение" (строки).
    * Возвращает: `void`
    * **Пример:**
        ```C++
        UFO_DB db;
        std::unordered_map<std::string, std::string> updates;
        updates["Возраст"] = "31";
        db.update("Люди", 1, updates);
        ```
* **`saveToFile(const std::string& filename)`**: Сохраняет данные базы данных в файл.

    * `filename`: Имя файла (строка).
    * Возвращает: `bool` (true, если сохранение прошло успешно; иначе false).

* **`loadFromFile(const std::string& filename)`**: Загружает данные базы данных из файла.

    * `filename`: Имя файла (строка).
    * Возвращает: `bool` (true, если загрузка прошла успешно; иначе false).


### Классы `UFORecords` и `UFOTable`

* **`UFORecords`**: Представляет собой запись в таблице, содержащую данные в виде `std::unordered_map`.
* **`UFOTable`**: Представляет собой таблицу, содержащую имя таблицы, вектор имен столбцов, вектор записей и id следующей записи.


### Возможные ошибки

Методы `addData`, `update`, `saveToFile`, `loadFromFile` могут возвращать false, сигнализируя о неудачном выполнении операции. Рекомендуется обрабатывать эти значения.


### Примеры использования

```C++
// Пример создания и заполнения таблицы
UFO_DB db;
db.createTable("Products", {"Name", "Price"});
db.setCurrentTable("Products");
db.addData("$Name$Price", "Milk,2.5");
db.addData("$Name$Price", "Bread,1.8");
db.select("Products"); // Выводит таблицу

```

# Изменения:

## Класс UFORecords:

Метод add изменен. Вместо приема списка пар std::initializer_list<std::pair<std::string, std::string>> он теперь принимает два отдельных аргумента: const std::string& fieldName, const std::string& value. Это упрощает добавление полей.

## Класс UFOTable:

Переменная ID переименована в nextID. Это более точно отражает её назначение — хранение следующего доступного ID.

Конструктор по умолчанию теперь инициализирует nextID значением 1.

Метод insertRecord модифицирован. Теперь он создает копию переданной записи record, добавляет к ней поле "id" и только потом добавляет её в вектор records. Это предотвращает изменение исходной записи вне функции.

Добавлен новый метод selectWhere, который позволяет выбирать записи, удовлетворяющие определенному условию (по заданному полю и значению). Он возвращает вектор UFORecords, содержащий выбранные записи.

Метод updateRecord модифицирован для использования std::find_if для поиска записи по ID и обработки случая, когда запись не найдена, выбрасывая исключение std::runtime_error.

Добавлен метод deleteRecord, который удаляет запись по ID. Он возвращает true, если запись была удалена, и false в противном случае.

Метод selectAll переписан. Логика вывода заголовка таблицы и самих записей вынесена в отдельные приватные методы printHeader и printRecord, что улучшает читаемость и структуру кода. Форматирование вывода тоже немного изменено.

## Класс UFO_DB:

Метод addData существенно изменен. Он теперь принимает имя таблицы и вектор строк std::vector<std::string> в качестве данных, вместо строки формата и строки данных. Это упрощает использование и делает обработку данных более надежной. Проверка на соответствие количества данных количеству столбцов выполняется с помощью tables.at(tableName).columns.size().

Добавлен метод deleteRecordFromTable для удаления записи из конкретной таблицы.

Обработка ошибок улучшена. Вместо вывода сообщений в std::cerr, большинство методов теперь бросают исключения std::runtime_error для более эффективной обработки ошибок.

Метод createTable теперь проверяет, существует ли уже таблица с таким именем, и бросает исключение std::runtime_error, если существует.

Метод setCurrentTable теперь проверяет существование таблицы и бросает исключение, если её нет.

Метод insert теперь проверяет существование таблицы и бросает исключение, если её нет.

Метод select теперь константный (const) и использует tables.at() для доступа к таблицам, что делает код немного безопаснее.

Метод update теперь использует tables.at() для доступа к таблицам, что делает код немного безопаснее и обрабатывает исключения.

В методе saveToFile, ID заменён на nextID. Выходные сообщения об ошибках заменены на исключения std::runtime_error. Метод теперь возвращает true в случае успешного сохранения.

В методе loadFromFile, обработка данных из файла существенно переработана для более эффективного и безопасного извлечения данных. Выходные сообщения об ошибках заменены на исключения std::runtime_error. Метод теперь очищает tables перед загрузкой данных.

## Примечание

Документация в дальнейшем может быть дополнена примерами использования с различными ошибками и методами обработки исключений, а также описанием внутренних представлений данных.




