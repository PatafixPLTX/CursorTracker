#include <Windows.h>
#include <iostream>
#include <thread>
#include <cmath>

using namespace std;

// Mouse Settings

const int mouseHz = 1000;
const int sleepTime = 1000 / mouseHz;

int totalDistancePX = 0;

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
    thread mouseCapture(mouseDistanceBetweenCoordinates);
    while (true)
    {
        Sleep(1000);
        cout << "Total distance: " << totalDistancePX << "px" << endl;
    }
    
    return 0;
}