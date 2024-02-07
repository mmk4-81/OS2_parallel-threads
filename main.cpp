#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <memory>

using namespace std;
using namespace std::chrono;

#define Filepath "\\dataset\\"

double runtime = 0;
int DataSize = 0;
double totalLands = 0;

int main()
{
    string filename;
    cout << "Enter dataset filename(1-6) : ";
    cin >> filename;

    ifstream file(Filepath + filename + ".txt");
    if (!file)
    {
        cout << "Cannot open file: " << filename << endl;
        return 1;
    }

    cout << "Enter runtime (seconds): ";
    cin >> runtime;

    int threadCount = thread::hardware_concurrency();

    double number;
    while (file >> number)
    {
        DataSize++;
        totalLands += number;
    }

    cout << endl
         << "dataset " << filename << " with " << DataSize << " records and runtime " << runtime << " seconds and with " << threadCount << " threads" << endl
         << endl;



    return 0;
}