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
double **result;
double *Lands;
struct MyParam
{
    DWORD threadID;
    double *localResult;
};

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

    Lands = new double[DataSize];

    file.clear();
    file.seekg(0, ios::beg);

    for (int i = 0; i < DataSize; i++)
    {
        file >> Lands[i];
    }
    file.close();

    result = new double *[threadCount];
    for (int i = 0; i < threadCount; i++)
    {
        result[i] = new double[DataSize + 4];

        for (int j = 0; j < DataSize + 4; j++)
        {
            result[i][j] = 0;
        }
    }

    MyParam *threadParams = new MyParam[threadCount];
    HANDLE *threadHandle = new HANDLE[threadCount];

    for (int i = 0; i < threadCount; i++)
    {
        threadParams[i].threadID = i + 1;
        threadParams[i].localResult = result[i];

        threadHandle[i] = CreateThread(NULL, 0, F, &threadParams[i], 0, NULL);

        if (!threadHandle[i])
        {
            cout << "Could not create thread, program will terminate." << endl;
            exit(1);
        }
    }

    return 0;
}