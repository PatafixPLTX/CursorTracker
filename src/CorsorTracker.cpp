#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <cmath>
#include <signal.h>

#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

bool stopProgram = false;

// Monitor Settings

const float monitorDiagonalInches = 27;

// Mouse Settings

const float mouseHz = 1000;

// Program Variables Initialization

const float sleepTime = 1000 / mouseHz;

int numberOfMonitors = 0;
int monitorPrimaryWidth = 0;
int monitorPrimaryHeight = 0;

int totalDistancePX = 0;
float totalDistanceM = 0;

const int defaultAspectRatioX = 16;
const int defaultAspectRatioY = 9;
const float monitorDiagonalCM = monitorDiagonalInches * 2.54;
float monitorWidhtCM = monitorDiagonalCM * defaultAspectRatioX / sqrt(pow(defaultAspectRatioX, 2) + pow(defaultAspectRatioY, 2));
float monitorHeightCM = monitorDiagonalCM * defaultAspectRatioY / sqrt(pow(defaultAspectRatioX, 2) + pow(defaultAspectRatioY, 2));

void signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        cout << "Program Stopped!" << endl;
        stopProgram = true;
    }
}

void monitorSizeAndCoordinates()
{
    RECT rcMonitor;
    GetSystemMetrics(sizeof(rcMonitor));

    numberOfMonitors = GetSystemMetrics(SM_CMONITORS);
    monitorPrimaryWidth = GetSystemMetrics(SM_CXSCREEN);
    monitorPrimaryHeight = GetSystemMetrics(SM_CYSCREEN);

    cout << "Monitors: " << numberOfMonitors << "\n"
         << "Primary Monitor: " << monitorPrimaryWidth << "x" << monitorPrimaryHeight << endl;

    monitorWidhtCM = monitorDiagonalCM * monitorPrimaryWidth / sqrt(pow(monitorPrimaryWidth, 2) + pow(monitorPrimaryHeight, 2));
    monitorWidhtCM = monitorDiagonalCM * monitorPrimaryHeight / sqrt(pow(monitorPrimaryWidth, 2) + pow(monitorPrimaryHeight, 2));
}

void mouseDistanceBetweenCoordinates()
{
    POINT cursorPoint;
    POINT oldCursorPoint;
    if (GetCursorPos(&cursorPoint))
    {
        oldCursorPoint = cursorPoint;

        while (true)
        {
            GetCursorPos(&cursorPoint);
            int newCursorX = cursorPoint.x;
            int newCursorY = cursorPoint.y;

            int oldCursorX = oldCursorPoint.x;
            int oldCursorY = oldCursorPoint.y;

            int distancePx = sqrt(pow((newCursorX - oldCursorX), 2) + pow((newCursorY - oldCursorY), 2));

            totalDistancePX = totalDistancePX + distancePx;
            totalDistanceM = round(totalDistancePX * (monitorWidhtCM / monitorPrimaryWidth)) / 100;
            oldCursorPoint = cursorPoint;

            Sleep(sleepTime);
        }
    }
    else
    {
        cout << "Error: GetCursorPos was unable to retrieve data (mouseDistanceBetweenCoordinates)" << endl;
    }
}

int main()
{
    HMODULE hModule1 = LoadLibraryW(L"./libgcc_s_seh-1.dll");
    if (hModule1 == NULL)
    {
        wcout << "Error: libgcc_s_seh-1.dll not found!" << endl;
        return 1;
    }

    HMODULE hModule2 = LoadLibraryW(L"./libstdc++-6.dll");
    if (hModule2 == NULL)
    {
        wcout << "Error: libstdc++-6.dll not found!" << endl;
        return 1;
    }

    HMODULE hModule3 = LoadLibraryW(L"./libwinpthread-1.dll");
    if (hModule3 == NULL)
    {
        wcout << "Error: libwinpthread-1.dll not found!" << endl;
        return 1;
    }

    signal(SIGINT, signalHandler);    

    cout << "Program Started!" << endl;

    ifstream file("data.json");
    if (!file.is_open())
    {
        cerr << "Error: data.json file not found!" << endl;
        return 1;
    }

    string jsonDataString((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Document jsonDataParse;
    jsonDataParse.Parse(jsonDataString.c_str());

    if (jsonDataParse.HasParseError())
    {
        cerr << "Error: data.json file is not a valid JSON file!" << endl;
        return 1;
    }

    if (jsonDataParse.HasMember("totalDistancePX"))
    {
        totalDistancePX = jsonDataParse["totalDistancePX"].GetInt();
    }
    else
    {
        cerr << "Error: data.json file is missing totalDistancePX!" << endl;
        return 1;
    }

    if (jsonDataParse.HasMember("totalDistanceM"))
    {
        totalDistanceM = jsonDataParse["totalDistanceM"].GetFloat();
    }
    else
    {
        cerr << "Error: data.json file is missing totalDistanceM!" << endl;
        return 1;
    }

    thread monitorInformation(monitorSizeAndCoordinates);
    thread mouseCapture(mouseDistanceBetweenCoordinates);

    cout << "Monitor Diagonal: " << monitorDiagonalCM << "cm" << endl;
    cout << "Monitor Width: " << monitorWidhtCM << "cm" << endl;
    cout << "Monitor Height: " << monitorHeightCM << "cm" << endl;

    while (!stopProgram)
    {
        cout << "Total distance: " << totalDistancePX << " px" << endl;
        cout << "Total distance: " << totalDistanceM << " m" << endl;

        Document jsonData;
        jsonData.SetObject();

        Value totalDistancePXValue;
        totalDistancePXValue.SetInt(totalDistancePX);
        jsonData.AddMember("totalDistancePX", totalDistancePXValue, jsonData.GetAllocator());

        Value totalDistanceMValue;
        totalDistanceMValue.SetFloat(totalDistanceM);
        jsonData.AddMember("totalDistanceM", totalDistanceMValue, jsonData.GetAllocator());

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        jsonData.Accept(writer);

        ofstream file("data.json");
        file << buffer.GetString() << endl;
        file.close();

        cout << "Data saved!" << endl;

        Sleep(10000);
    }

    Document jsonData;
    jsonData.SetObject();

    Value totalDistancePXValue;
    totalDistancePXValue.SetInt(totalDistancePX);
    jsonData.AddMember("totalDistancePX", totalDistancePXValue, jsonData.GetAllocator());

    Value totalDistanceMValue;
    totalDistanceMValue.SetFloat(totalDistanceM);
    jsonData.AddMember("totalDistanceM", totalDistanceMValue, jsonData.GetAllocator());

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    jsonData.Accept(writer);

    ofstream fileEnd("data.json");
    fileEnd << buffer.GetString() << endl;
    fileEnd.close();

    cout << "Data saved!" << endl;

    FreeLibrary(hModule1);
    FreeLibrary(hModule2);

    return 0;
}