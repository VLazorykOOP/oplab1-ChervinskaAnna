#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

// Function Prototypes
double func_regr(double r, double m, double k);
double func(double x, double y, double z);
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

// Global data holders for values from files
std::unordered_map<std::string, double> textMap;
std::vector<std::pair<double, double>> Udata;
std::vector<std::pair<double, double>> Tdata;

bool readDataFromFile1();
bool readDataFromFile2();
bool readDataFromFile3();

int main()
{
    // Read the data from the files
    bool file1Available = readDataFromFile1();
    bool file2Available = readDataFromFile2();
    bool file3Available = readDataFromFile3();

    if (!file1Available || !file2Available || !file3Available)
    {
        std::cerr << "Error: One or more required data files are missing or not readable." << std::endl;
        return -1;
    }

    // Example variables for testing
    double x = 1.0, y = 2.0, z = 3.0;
    std::string text = "set";

    double r = func(x, y, z);
    double m = Qnk(x, y);
    double k = Ktext(x, y, z, 1.0, text);

    // Calculate func_regr
    double result = func_regr(r, m, k);

    std::cout << "Result of func_regr: " << result << std::endl;

    return 0;
}

// Function Implementations
double func_regr(double r, double m, double k)
{
    return 10 * k * k * r * r - m * r;
}

double func(double x, double y, double z)
{
    if (x > y && x > z)
        return Qnk(x, y) + z;
    else if (z > x && z > y)
        return Qnk(z, y) + x;
    else
        return Qnk(x, z) - Qn(x, y, z);
}

// Added base case check for recursion depth
double Qnk(double x, double y)
{
    if (fabs(x) <= 5)
    {
        if (fabs(x) > 1e5 || fabs(y) > 1e5)
            return 0;
        return x * Qnk1(x, y, y, x) + y * Qnk1(y, x, x, y) - 0.05 * Qnk1(x, y, y, x) * Qnk1(y, x, x, y);
    }
    else
    {
        return Qn(x, y, y);
    }
}

// New function definition for Qnk1
double Qnk1(double x, double y, double z, double u)
{
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5 || fabs(u) > 1e5)
        return 0;
    return 1.15 * Qn(x, y, z) - 0.95 * Qn1(x, y, z);
}

// New function definition for Qn1
double Qn1(double x, double y, double z)
{
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5)
        return 0;
    return x / (U(x) + T(y)) - U(z) * T(z);
}

double Qn(double x, double y, double z)
{
    if (fabs(x) > 1e5 || fabs(y) > 1e5 || fabs(z) > 1e5)
        return 0;
    return Qnk(x, y) + y + z;
}

double Rsv(double x, double y, double z)
{
    if (z > x && z > y)
        return Qnk(z, x) + y;
    else if (y > x && y > z)
        return Qnk(y, x) + z;
    else
        return Qnk(y, z) - x * y;
}

double U(double x)
{
    auto it = std::find_if(Udata.begin(), Udata.end(),
                           [x](const std::pair<double, double> &p)
                           { return fabs(p.first - x) < 1e-6; });
    if (it != Udata.end())
        return it->second;
    if (fabs(x) > 1e5)
        return 0;
    return x / (U(x) + T(x));
}

double T(double x)
{
    auto it = std::find_if(Tdata.begin(), Tdata.end(),
                           [x](const std::pair<double, double> &p)
                           { return fabs(p.first - x) < 1e-6; });
    if (it != Tdata.end())
        return it->second;
    if (fabs(x) > 1e5)
        return 0;
    return x / (U(x) + T(x));
}

double GetFrom(const std::string &text)
{
    auto it = textMap.find(text);
    return it != textMap.end() ? it->second : 0;
}

double Min(double x, double y, double z, double u)
{
    return std::min({x, y, z, u});
}

double Stext(double x, const std::string &text)
{
    if (x > 0)
        return GetFrom(text.substr(0, 2)) + x;
    else
        return GetFrom(text.substr(0, 3)) - x;
}

double Ktext(double x, double y, double z, double t, const std::string &text)
{
    return Stext(Min(x, y, z, t), text);
}

// Reading data from files
bool readDataFromFile1()
{
    std::ifstream file("dat1.dat");
    if (!file.is_open())
        return false;

    double x, u;
    while (file >> x >> u)
    {
        Udata.push_back({x, u});
    }
    file.close();
    return true;
}

bool readDataFromFile2()
{
    std::ifstream file("dat2.dat");
    if (!file.is_open())
        return false;

    double x, t;
    while (file >> x >> t)
    {
        Tdata.push_back({x, t});
    }
    file.close();
    return true;
}

bool readDataFromFile3()
{
    std::ifstream file("dat3.dat");
    if (!file.is_open())
        return false;

    std::string text;
    double value;
    while (file >> text >> value)
    {
        textMap[text] = value;
    }
    file.close();
    return true;
}
