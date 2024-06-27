#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

// Класи для обробки помилок
class ErrorRange
{
    string str = "ErrorRange";

public:
    double rd;
    ErrorRange(double d) : rd(d) {}
    void Message()
    {
        cout << "ErrorRange : current x is " << rd << " but -10 < x < 10 " << endl;
    }
};

class ErrorNoFile
{
    string str = "ErrorNoFile";

public:
    ErrorNoFile(string s) : str(s) {}
    void Message()
    {
        cout << "ErrorNoFile " << str << endl;
    }
};

// Прототипи функцій
double func_regr(double r, double m, double k);
double func(double x, double y, double z) throw(ErrorRange, ErrorNoFile);
double Qnk(double x, double y);
double Qn(double x, double y, double z);
double Qn1(double x, double y, double z);
double Rsv(double x, double y, double z);
double U(double x);
double T(double x);
double GetFrom(const std::string &text);
double Min(double x, double y, double z, double u);
double Stext(double x, const std::string &text);
double Ktext(double x, double y, double z, double t, const std::string &text);
double Qnk1(double x, double y, double z, double u);

// Глобальні змінні для зберігання даних, отриманих з файлів
std::unordered_map<std::string, double> textMap; // Мапа для зберігання текстових значень
std::vector<std::pair<double, double>> Udata;    // Вектор пар для зберігання даних U
std::vector<std::pair<double, double>> Tdata;    // Вектор пар для зберігання даних T

bool readDataFromFile1() throw(ErrorNoFile); // Прототип функції для зчитування даних з першого файлу
bool readDataFromFile2() throw(ErrorNoFile); // Прототип функції для зчитування даних з другого файлу
bool readDataFromFile3() throw(ErrorNoFile); // Прототип функції для зчитування даних з третього файлу

int main()
{
    try
    {
        // Зчитування даних з файлів
        bool file1Available = readDataFromFile1(); // Зчитування даних з першого файлу
        bool file2Available = readDataFromFile2(); // Зчитування даних з другого файлу
        bool file3Available = readDataFromFile3(); // Зчитування даних з третього файлу

        // Перевірка наявності всіх файлів
        if (!file1Available || !file2Available || !file3Available)
        {
            throw ErrorNoFile("One or more required data files are missing or not readable.");
        }

        // Приклад змінних для тестування
        double x = 1.0, y = 2.0, z = 3.0;
        std::string text = "set";

        double r = func(x, y, z);             // Обчислення значення функції func
        double m = Qnk(x, y);                 // Обчислення значення функції Qnk
        double k = Ktext(x, y, z, 1.0, text); // Обчислення значення функції Ktext

        // Обчислення результату функції func_regr
        double result = func_regr(r, m, k);

        // Виведення результату
        std::cout << "Result of func_regr: " << result << std::endl;
    }
    catch (ErrorNoFile &e)
    {
        e.Message();
        return -1; // Повернення коду помилки
    }
    catch (ErrorRange &e)
    {
        e.Message();
        return -2; // Повернення коду помилки
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred." << std::endl;
        return -3; // Повернення коду помилки
    }

    return 0; // Успішне завершення програми
}

// Реалізація функцій
// Функція обчислення func_regr
double func_regr(double r, double m, double k)
{
    // Функція повертає результат обчислень за формулою: 10 * k^2 * r^2 - m * r
    return 10 * k * k * r * r - m * r;
}

// Функція обчислення func
double func(double x, double y, double z) throw(ErrorRange, ErrorNoFile)
{
    if (x > 10 || x < -10)
        throw ErrorRange(x);
    if (y > 10 || y < -10)
        throw ErrorRange(y);
    if (z > 10 || z < -10)
        throw ErrorRange(z);

    // Порівняння x, y і z, вибір дій в залежності від результату
    if (x > y && x > z)
        return Qnk(x, y) + z;
    else if (z > x && z > y)
        return Qnk(z, y) + x;
    else
        return Qnk(x, z) - Qn(x, y, z);
}

// Функція обчислення Qnk з перевіркою основного випадку
double Qnk(double x, double y)
{
    // Перевірка на граничні значення
    if (fabs(x) <= 5)
    {
        if (fabs(x) > 1e5 || fabs(y) > 1e5)
            return 0; // Повертає 0, якщо значення дуже великі
        // Обчислення значення Qnk на основі Qnk1
        return x * Qnk1(x, y, y, x) + y * Qnk1(y, x, x, y) - 0.05 * Qnk1(x, y, y, x) * Qnk1(y, x, x, y);
    }
    else
    {
        // Повернення значення Qn для великих значень x
        return Qn(x, y, y);
    }
}

// Функція обчислення Qnk1
double Qnk1(double x, double y, double z, double u)
{
    // Перевірка на граничні значення
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5 || fabs(u) > 1e5)
        return 0; // Повертає 0, якщо значення дуже великі
    // Обчислення значення Qnk1 на основі Qn та Qn1
    return 1.15 * Qn(x, y, z) - 0.95 * Qn1(x, y, z);
}

// Функція обчислення Qn1
double Qn1(double x, double y, double z)
{
    // Перевірка на граничні значення
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5)
        return 0; // Повертає 0, якщо значення дуже великі
    // Обчислення значення Qn1 на основі U та T
    return x / (U(x) + T(y)) - U(z) * T(z);
}

// Функція обчислення Qn
double Qn(double x, double y, double z)
{
    // Перевірка на граничні значення
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5)
        return 0; // Повертає 0, якщо значення дуже великі
    // Обчислення значення Qn на основі Qnk
    return Qnk(x, y) + y + z;
}

// Функція обчислення Rsv
double Rsv(double x, double y, double z)
{
    // Порівняння x, y і z, вибір дій в залежності від результату
    if (z > x && z > y)
        return Qnk(z, x) + y;
    else if (y > x && y > z)
        return Qnk(y, x) + z;
    else
        return Qnk(y, z) - x * y;
}

// Функція обчислення U
double U(double x)
{
    // Пошук значення U для заданого x у векторі Udata
    auto it = std::find_if(Udata.begin(), Udata.end(),
                           [x](const std::pair<double, double> &p)
                           { return fabs(p.first - x) < 1e-6; });
    // Повернення знайденого значення або розрахунок, якщо не знайдено
    if (it != Udata.end())
        return it->second;
    if (fabs(x) > 1e5)
        return 0; // Повертає 0, якщо значення дуже великі
    // Обчислення значення U рекурсивно
    return x / (U(x) + T(x));
}

// Функція обчислення T
double T(double x)
{
    // Пошук значення T для заданого x у векторі Tdata
    auto it = std::find_if(Tdata.begin(), Tdata.end(),
                           [x](const std::pair<double, double> &p)
                           { return fabs(p.first - x) < 1e-6; });
    // Повернення знайденого значення або розрахунок, якщо не знайдено
    if (it != Tdata.end())
        return it->second;
    if (fabs(x) > 1e5)
        return 0; // Повертає 0, якщо значення дуже великі
    // Обчислення значення T рекурсивно
    return x / (U(x) + T(x));
}

// Функція отримання значення з мапи textMap за заданим текстом
double GetFrom(const std::string &text)
{
    // Пошук значення у мапі textMap
    auto it = textMap.find(text);
    return it != textMap.end() ? it->second : 0;
}

// Функція обчислення мінімального значення з чотирьох заданих
double Min(double x, double y, double z, double u)
{
    // Використання стандартної функції std::min
    return std::min({x, y, z, u});
}

// Функція обчислення Stext на основі значення x та тексту
double Stext(double x, const std::string &text)
{
    // Повернення обчисленого значення на основі GetFrom
    if (x > 0)
        return GetFrom(text.substr(0, 2)) + x;
    else
        return GetFrom(text.substr(0, 3)) - x;
}

// Функція обчислення Ktext на основі кількох параметрів та тексту
double Ktext(double x, double y, double z, double t, const std::string &text)
{
    // Використання функції Stext для обчислення значення Ktext
    return Stext(Min(x, y, z, t), text);
}

// Функція зчитування даних з файлу dat1.dat
bool readDataFromFile1() throw(ErrorNoFile)
{
    std::ifstream file("dat1.dat");
    if (!file.is_open())
    {
        throw ErrorNoFile("dat1.dat");
    }

    double x, u;
    while (file >> x >> u)
    {
        Udata.push_back({x, u});
    }
    file.close();
    return true;
}

// Функція зчитування даних з файлу dat2.dat
bool readDataFromFile2() throw(ErrorNoFile)
{
    std::ifstream file("dat2.dat");
    if (!file.is_open())
    {
        throw ErrorNoFile("dat2.dat");
    }

    double x, t;
    while (file >> x >> t)
    {
        Tdata.push_back({x, t});
    }
    file.close();
    return true;
}

// Функція зчитування даних з файлу dat3.dat
bool readDataFromFile3() throw(ErrorNoFile)
{
    std::ifstream file("dat3.dat");
    if (!file.is_open())
    {
        throw ErrorNoFile("dat3.dat");
    }

    std::string text;
    double value;
    while (file >> text >> value)
    {
        textMap[text] = value;
    }
    file.close();
    return true;
}
