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

DWORD WINAPI F(LPVOID param);

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

    WaitForMultipleObjects(threadCount, threadHandle, TRUE, INFINITE);
    for (int i = 0; i < threadCount; i++)
    {
        CloseHandle(threadHandle[i]);
    }

    double *bestResult = nullptr;
    int sum[3] = {0};
    DWORD besttid = result[0][0];
    double bestDiff = -1;

    cout << "Note: boy1 => 0  **** boy2 => 1  **** girl => 2\n\n";

    for (int i = 0; i < threadCount; i++)
    {

        double *currentResult = result[i];

        double currentDiff = abs(currentResult[DataSize + 1] - 0.4 * totalLands) + abs(currentResult[DataSize + 2] - 0.4 * totalLands) + abs(currentResult[DataSize + 3] - 0.2 * totalLands);

        DWORD threadID = threadParams[i].threadID;

        cout << "Thread " << i + 1 << " (Thread ID = " << threadID << ") >>\n [ ";
        for (int j = 0; j < DataSize; j++)
        {
            cout << Lands[j] << "(" << currentResult[j + 1] << ") , ";
        }
        cout << " ] \n ";
        cout << "boy1 => " << currentResult[DataSize + 1] << " ****  boy2 => " << currentResult[DataSize + 2]
             << " ****  girl => " << currentResult[DataSize + 3];
        cout << " **** F => " << currentDiff;
        cout << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------\n";

        if (currentDiff < bestDiff || bestDiff == -1)
        {
            bestDiff = currentDiff;
            bestResult = currentResult;
            besttid = threadID;
        }
    }

    cout << "\n\nBest Thread ID (" << besttid << ") => f = " << bestDiff << endl;

    delete[] threadParams;
    delete[] threadHandle;
    delete[] Lands;
    for (int i = 0; i < threadCount; i++)
    {
        delete[] result[i];
    }
    delete[] result;

    return 0;
}

DWORD WINAPI F(LPVOID param)
{
    MyParam *input = static_cast<MyParam *>(param);
    double *localResult = input->localResult;
    DWORD threadID = input->threadID;

    srand(time(NULL) + threadID);
    auto start_time = high_resolution_clock::now();

    double diff = abs(localResult[DataSize + 1] - 0.4 * totalLands) + abs(localResult[DataSize + 2] - 0.4 * totalLands) + abs(localResult[DataSize + 3] - 0.2 * totalLands);

    while (true)
    {

        double *distribution = new double[DataSize];
        int sum[3] = {0};

        for (int i = 0; i < DataSize; i++)
        {
            int randomindex = rand() % 3;
            sum[randomindex] += Lands[i];
            distribution[i] = randomindex;
        }

        double newdiff = abs(sum[0] - 0.4 * totalLands) + abs(sum[1] - 0.4 * totalLands) + abs(sum[2] - 0.2 * totalLands);

        if (newdiff < diff)
        {
            diff = newdiff;

            for (int i = 0; i < DataSize; i++)
            {
                localResult[i + 1] = distribution[i];
            }
            for (int i = 0; i < 3; i++)
            {
                localResult[DataSize + i + 1] = sum[i];
            }
        }

        delete[] distribution;

        auto end_time = high_resolution_clock::now();
        auto elapsed_time = duration_cast<duration<double>>(end_time - start_time).count();

        if (elapsed_time >= runtime)
        {
            break;
        }
    }

    return 0;
}