#include "variant3.h"

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

void Pencil::print(ostream& os) const {
    os << "PENCIL: "
       << "leadDensity=" << leadDensity
       << ", color=" << colorToString(color)
       << ", price=" << fixed << setprecision(2) << price
       << ", phone=" << phone << '\n';
}

void Pen::print(ostream& os) const {
    os << "PEN: "
       << "type=" << penType
       << ", diameter=" << fixed << setprecision(2) << diameter
       << ", price=" << fixed << setprecision(2) << price
       << ", phone=" << phone << '\n';
}

void Paper::print(ostream& os) const {
    os << "PAPER: "
       << "density=" << density
       << ", width=" << width
       << ", height=" << height
       << ", price=" << fixed << setprecision(2) << price
       << ", phone=" << phone << '\n';
}

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

unique_ptr<Stationery> parsePencil(istringstream& iss) {
    int density;
    string color;
    double price;
    string phone;

    if (!(iss >> density >> color >> price >> phone)) {
        cerr << "Ошибка ADD PENCIL\n";
        return nullptr;
    }

    Color c = stringToColor(color);
    if (c == Color::UNKNOWN) {
        cerr << "Неизвестный цвет карандаша: " << color << '\n';
        return nullptr;
    }

    return make_unique<Pencil>(density, c, price, phone);
}

unique_ptr<Stationery> parsePen(istringstream& iss) {
    string penType;
    double diameter;
    double price;
    string phone;

    if (!(iss >> penType >> diameter >> price >> phone)) {
        cerr << "Ошибка ADD PEN\n";
        return nullptr;
    }

    if (penType != "BALL" && penType != "GEL") {
        cerr << "Тип ручки должен быть BALL или GEL\n";
        return nullptr;
    }

    return make_unique<Pen>(penType, diameter, price, phone);
}

unique_ptr<Stationery> parsePaper(istringstream& iss) {
    int density;
    int width;
    int height;
    double price;
    string phone;

    if (!(iss >> density >> width >> height >> price >> phone)) {
        cerr << "Ошибка ADD PAPER\n";
        return nullptr;
    }

    return make_unique<Paper>(density, width, height, price, phone);
}

void processAdd(istringstream& iss, Container& items) {
    string type;
    iss >> type;

    unique_ptr<Stationery> item;

    if (type == "PENCIL") {
        item = parsePencil(iss);
    }
    else if (type == "PEN") {
        item = parsePen(iss);
    }
    else if (type == "PAPER") {
        item = parsePaper(iss);
    }
    else {
        cerr << "Неизвестный тип объекта: " << type << '\n';
        return;
    }

    if (item) {
        items.push_back(move(item));
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

string formatItem(const Stationery& item) {
    ostringstream oss;
    item.print(oss);
    string s = oss.str();
    if (!s.empty() && s.back() == '\n') {
        s.pop_back();
    }
    return s;
}

void applyCommand(const string& line, Container& items) {
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
        // в тестах PRINT не проверяется через applyCommand
    }
    else if (command.empty()) {
        // пустая строка
    }
    else {
        cerr << "Неизвестная команда: " << command << '\n';
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
        if (line.empty()) continue;

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

#ifndef UNIT_TESTING
int main() {
    Container items;
    processFile("input.txt", items);
    return 0;
}
#endif