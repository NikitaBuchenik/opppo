#pragma once

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

enum class Color {
    RED, ORANGE, YELLOW, GREEN, BLUE, VIOLET, UNKNOWN
};

string colorToString(Color color);
Color stringToColor(const string& s);

class Stationery {
protected:
    double price;
    string phone;

public:
    Stationery(double price, const string& phone)
        : price(price), phone(phone) {}

    virtual ~Stationery() = default;

    double getPrice() const { return price; }
    string getPhone() const { return phone; }

    virtual string getType() const = 0;
    virtual void print(ostream& os) const = 0;
};

class Pencil : public Stationery {
private:
    int leadDensity;
    Color color;

public:
    Pencil(int density, Color color, double price, const string& phone)
        : Stationery(price, phone), leadDensity(density), color(color) {}

    string getType() const override { return "PENCIL"; }
    void print(ostream& os) const override;
};

class Pen : public Stationery {
private:
    string penType;
    double diameter;

public:
    Pen(const string& type, double diameter, double price, const string& phone)
        : Stationery(price, phone), penType(type), diameter(diameter) {}

    string getType() const override { return "PEN"; }
    void print(ostream& os) const override;
};

class Paper : public Stationery {
private:
    int density;
    int width;
    int height;

public:
    Paper(int density, int width, int height, double price, const string& phone)
        : Stationery(price, phone), density(density), width(width), height(height) {}

    string getType() const override { return "PAPER"; }
    void print(ostream& os) const override;
};

using Container = vector<unique_ptr<Stationery>>;

unique_ptr<Stationery> parsePencil(istringstream& iss);
unique_ptr<Stationery> parsePen(istringstream& iss);
unique_ptr<Stationery> parsePaper(istringstream& iss);

void processAdd(istringstream& iss, Container& items);
void processRemove(istringstream& iss, Container& items);
void processFile(const string& filename, Container& items);

bool compare(double left, const string& op, double right);
std::string formatItem(const Stationery& item);
void applyCommand(const std::string& line, Container& items);