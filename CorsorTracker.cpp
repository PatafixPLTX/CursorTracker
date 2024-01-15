#include <Windows.h>
#include <iostream>
#include <thread>
#include <cmath>

using namespace std;

// Monitor Settings

const float monitorDiagonalInches = 27;
const int aspectRatioX = 16;
const int aspectRatioY = 9;

// Mouse Settings

const float mouseHz = 1000;
const float sleepTime = 1000 / mouseHz;

// Program Variables Initialization

int numberOfMonitors = 0;
int monitorPrimaryWidth = 0;
int monitorPrimaryHeight = 0;

int totalDistancePX = 0;
int totalDistanceCM = 0;

const float monitorDiagonalCM = monitorDiagonalInches * 2.54;
const float monitorWidhtCM = monitorDiagonalCM * aspectRatioX / sqrt(pow(aspectRatioX, 2) + pow(aspectRatioY, 2));
const float monitorHeightCM = monitorDiagonalCM * aspectRatioY / sqrt(pow(aspectRatioX, 2) + pow(aspectRatioY, 2));

// const float pixelSizeCM = monitorWidhtCM / monitorPrimaryWidth;

void monitorSizeAndCoordinates()
{
    RECT rcMonitor;
    GetSystemMetrics(sizeof(rcMonitor));

    numberOfMonitors = GetSystemMetrics(SM_CMONITORS);
    monitorPrimaryWidth = GetSystemMetrics(SM_CXSCREEN);
    monitorPrimaryHeight = GetSystemMetrics(SM_CYSCREEN);

    cout << "Monitors: " << numberOfMonitors << "\n" << "Primary Monitor: " << monitorPrimaryWidth << "x" << monitorPrimaryHeight << endl;
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
    cout << "Program Started!" << endl;

    thread monitorInformation(monitorSizeAndCoordinates);
    thread mouseCapture(mouseDistanceBetweenCoordinates);

    cout << "Monitor Diagonal: " << monitorDiagonalCM << "cm" << endl;
    cout << "Monitor Width: " << monitorWidhtCM << "cm" << endl;
    cout << "Monitor Height: " << monitorHeightCM << "cm" << endl;

    while (true)
    {
        Sleep(1000);
        cout << "Total distance: " << totalDistancePX << " px" << endl;
        cout << "Total distance: " << totalDistancePX * (monitorWidhtCM / monitorPrimaryWidth) << " cm" << endl;
    }

    return 0;
}