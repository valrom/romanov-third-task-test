#include <iostream>
#include <regex>
#include <optional>
#include <vector>

// Задача 3
// Есть GCODE Данного формата X(value);Y(value);Z(value).
// Он используется для отдачи команд микроконтроллеры, который в свою очередь управляет устройством управления.
// Зачастую GCODE выполняется построчно, поэтому если в скрипте оказалась ошибка, то только на ошибочной строке он остановится.
//
// Дан  GCODE - выведете конечные координаты устройства.


// Тестовая строка с GCODE
const char * str_in = R"TEST(
10 20 0
X10;Y20;Z30;
X40;Y-20;<Z20;
X-10;>20;Z-10;
)TEST";


struct Command {
    int x, y, z;
};

class GcodeActor {
private:
    int _x, _y, _z;
public:
    GcodeActor() : _x(0), _y(0), _z(0) {}


    void apply_command(Command command) {
        _x += command.x;
        _y += command.y;
        _z += command.z;
    }

    friend std::istream& operator>>(std::istream& in, GcodeActor& actor) {
        in >> actor._x >> actor._y >> actor._z;
        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const GcodeActor& actor) {
        out << actor._x << " " << actor._y << " " << actor._z;
        return out;
    }

};

//  Поиск всех символов в строке
std::vector<size_t> find_all(const std::string& str, char ch) {
    std::vector<size_t> ret;

    for ( size_t i = 0; i < str.size(); ++i) {
        if (str[i] == ch) {
            ret.push_back(i);
        }
    }
    return ret;
}

// возвращает подстроку from..to
std::string str_from_to(const std::string& str, int from, int to) {
    if (from > to || from < 0 || to > str.length() ) {
        throw std::exception();
    }
    return str.substr( from, to - from);
}

// Парсинг команды из строки
std::optional<Command> get_command(const std::string& str) {
    if (!std::regex_match(str, std::regex("X-{0,1}[0-9]+;Y-{0,1}[0-9]+;Z-{0,1}[0-9]+;"))) {
        return std::nullopt;
    }

    auto vs = find_all(str, ';');

    std::cout << "String: " << str << std::endl;

    Command out;
    out.x = std::stoi(str_from_to(str, 1, vs[0]));
    out.y = std::stoi(str_from_to(str, vs[0]+2, vs[1]));
    out.z = std::stoi(str_from_to(str, vs[1]+2, vs[2]));

    return std::optional{out};
}


int main() {
    std::stringstream in_str(str_in);
    std::string str;
    bool ends_with_error = false;

    GcodeActor actor;
    in_str >> actor;


    while ( in_str >> str, str.length() != 0 && !in_str.eof() ) {
        auto get_cmd = get_command(str);

        if (get_cmd.has_value()) {
            auto cmd = get_cmd.value();
            actor.apply_command(cmd);
        } else {
            ends_with_error = true;
            break;
        }

        str.clear();
    }

    std::cout << actor << std::endl;
    if (ends_with_error) {
        std::cout << "ERROR SCRIPT" << std::endl;
    }

    return 0;
}
