#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <functional>

using namespace std;

// Вариант 3: Канцелярские принадлежности
// Общие параметры: цена и номер телефона владельца.
//
// Формат входного файла:
// ADD PENCIL <density> <color> <price> <phone>
// ADD PEN <type> <diameter> <price> <phone>
// ADD PAPER <density> <width> <height> <price> <phone>
// REM PRICE <operator> <value>
// REM TYPE <PENCIL|PEN|PAPER>
// PRINT

enum class Color {
    RED, ORANGE, YELLOW, GREEN, BLUE, VIOLET, UNKNOWN
};

string colorToString(Color color) {
    switch (color) {
        case Color::RED: return "RED";
        case Color::ORANGE: return "ORANGE";
        case Color::YELLOW: return "YELLOW";
        case Color::GREEN: return "GREEN";
        case Color::BLUE: return "BLUE";
        case Color::VIOLET: return "VIOLET";
        default: return "UNKNOWN";
    }
}

Color stringToColor(const string& s) {
    if (s == "RED") return Color::RED;
    if (s == "ORANGE") return Color::ORANGE;
    if (s == "YELLOW") return Color::YELLOW;
    if (s == "GREEN") return Color::GREEN;
    if (s == "BLUE") return Color::BLUE;
    if (s == "VIOLET") return Color::VIOLET;
    return Color::UNKNOWN;
}

class Stationery {
protected:
    double price;
    string phone;

public:
    Stationery(double price, const string& phone)
        : price(price), phone(phone) {}

    virtual ~Stationery() = default;

    double getPrice() const {
        return price;
    }

    string getPhone() const {
        return phone;
    }

    virtual string getType() const = 0;
    virtual void print(ostream& os) const = 0;
};

class Pencil : public Stationery {
private:
    int leadDensity;
    Color color;

public:
    Pencil(int density, Color color, double price, const string& phone)
        : Stationery(price, phone),
          leadDensity(density),
          color(color) {}

    string getType() const override {
        return "PENCIL";
    }

    void print(ostream& os) const override {
        os << "PENCIL: "
           << "leadDensity=" << leadDensity
           << ", color=" << colorToString(color)
           << ", price=" << fixed << setprecision(2) << price
           << ", phone=" << phone << '\n';
    }
};

class Pen : public Stationery {
private:
    string penType;
    double diameter;

public:
    Pen(const string& type, double diameter, double price, const string& phone)
        : Stationery(price, phone),
          penType(type),
          diameter(diameter) {}

    string getType() const override {
        return "PEN";
    }

    void print(ostream& os) const override {
        os << "PEN: "
           << "type=" << penType
           << ", diameter=" << fixed << setprecision(2) << diameter
           << ", price=" << fixed << setprecision(2) << price
           << ", phone=" << phone << '\n';
    }
};

class Paper : public Stationery {
private:
    int density;
    int width;
    int height;

public:
    Paper(int density, int width, int height,
          double price, const string& phone)
        : Stationery(price, phone),
          density(density),
          width(width),
          height(height) {}

    string getType() const override {
        return "PAPER";
    }

    void print(ostream& os) const override {
        os << "PAPER: "
           << "density=" << density
           << ", width=" << width
           << ", height=" << height
           << ", price=" << fixed << setprecision(2) << price
           << ", phone=" << phone << '\n';
    }
};

using Container = vector<unique_ptr<Stationery>>;

const unordered_map<string, function<bool(double, double)>> OPERATORS = {
    {">",  [](double a, double b) { return a > b;  }},
    {"<",  [](double a, double b) { return a < b;  }},
    {">=", [](double a, double b) { return a >= b; }},
    {"<=", [](double a, double b) { return a <= b; }},
    {"==", [](double a, double b) { return a == b; }},
    {"!=", [](double a, double b) { return a != b; }},
};

bool compare(double left, const string& op, double right) {
    auto it = OPERATORS.find(op);
    if (it == OPERATORS.end()) {
        return false;
    }
    return it->second(left, right);
}

void processAdd(istringstream& iss, Container& items) {
    string type;
    iss >> type;

    if (type == "PENCIL") {
        int density;
        string color;
        double price;
        string phone;

        if (!(iss >> density >> color >> price >> phone)) {
            cerr << "Ошибка ADD PENCIL\n";
            return;
        }

        Color c = stringToColor(color);
        if (c == Color::UNKNOWN) {
            cerr << "Неизвестный цвет карандаша: " << color << '\n';
            return;
        }

        items.push_back(
            make_unique<Pencil>(density, c, price, phone)
        );
    }
    else if (type == "PEN") {
        string penType;
        double diameter;
        double price;
        string phone;

        if (!(iss >> penType >> diameter >> price >> phone)) {
            cerr << "Ошибка ADD PEN\n";
            return;
        }

        if (penType != "BALL" && penType != "GEL") {
            cerr << "Тип ручки должен быть BALL или GEL\n";
            return;
        }

        items.push_back(
            make_unique<Pen>(penType, diameter, price, phone)
        );
    }
    else if (type == "PAPER") {
        int density;
        int width;
        int height;
        double price;
        string phone;

        if (!(iss >> density >> width >> height >> price >> phone)) {
            cerr << "Ошибка ADD PAPER\n";
            return;
        }

        items.push_back(
            make_unique<Paper>(
                density, width, height, price, phone
            )
        );
    }
    else {
        cerr << "Неизвестный тип объекта: " << type << '\n';
    }
}

void processRemove(istringstream& iss, Container& items) {
    string field;
    iss >> field;

    if (field == "TYPE") {
        string type;
        iss >> type;

        auto oldEnd = remove_if(
            items.begin(),
            items.end(),
            [&](const unique_ptr<Stationery>& item) {
                return item->getType() == type;
            }
        );

        items.erase(oldEnd, items.end());
    }
    else if (field == "PRICE") {
        string op;
        double value;

        if (!(iss >> op >> value)) {
            cerr << "Ошибка условия REM PRICE\n";
            return;
        }

        auto oldEnd = remove_if(
            items.begin(),
            items.end(),
            [&](const unique_ptr<Stationery>& item) {
                return compare(item->getPrice(), op, value);
            }
        );

        items.erase(oldEnd, items.end());
    }
    else if (field == "PHONE") {
        string phoneValue;
        if (!(iss >> phoneValue)) {
            cerr << "Ошибка условия REM PHONE\n";
            return;
        }

        auto oldEnd = remove_if(
            items.begin(),
            items.end(),
            [&](const unique_ptr<Stationery>& item) {
                return item->getPhone() == phoneValue;
            }
        );

        items.erase(oldEnd, items.end());
    }
    else {
        cerr << "Неизвестное поле условия REM: " << field << '\n';
    }
}

void processFile(const string& filename, Container& items) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Не удалось открыть файл: " << filename << '\n';
        return;
    }

    ofstream outFile("output.txt");
    if (!outFile.is_open()) {
        cerr << "Не удалось открыть файл для записи: output.txt\n";
        return;
    }

    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "ADD") {
            processAdd(iss, items);
        }
        else if (command == "REM") {
            processRemove(iss, items);
        }
        else if (command == "PRINT") {
            outFile << "\n--- Содержимое контейнера ---\n";

            if (items.empty()) {
                outFile << "Контейнер пуст.\n";
            } else {
                for (const auto& item : items) {
                    item->print(outFile);
                }
            }
        }
        else {
            cerr << "Неизвестная команда: " << command << '\n';
        }
    }
}

int main() {
    Container items;

    processFile("input.txt", items);

    return 0;
}