#include <iostream>
#include <Windows.h>
#include <thread>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 30;


int main()
{
    // Create Screen Buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;


    struct sPlayer{
        int x_pos;
        int y_pos;
    };

    int nScore = 0;
    bool bDead = false;
    bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;
    sPlayer player = {60,29};
    // Game Loop
    while(1)
    {
        // Get Input
        bKeyLeft = (0x800 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
        bKeyRight = (0x800 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;

        if(bKeyLeft && !bKeyLeftOld)
            player.x_pos--;
        if(bKeyRight && !bKeyRightOld)
            player.x_pos++;
        bKeyLeftOld = bKeyLeft;
        bKeyRightOld = bKeyRight;


        // Game Logic


        // Collision Detection

        
        // Display to Player
        // Clear Screen
        for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
            screen[i] = L' ';

        // Setup Border
        for(int i = 0; i < nScreenWidth; i++)
        {
            screen[i] = L'=';
            screen[2 * nScreenWidth + i] = L'=';
        }
        wsprintf(&screen[nScreenWidth + 5], L"S P A C E    I N V A D E R S");

        // Display Player
        screen[player.y_pos * nScreenWidth + player.x_pos] = L'O';

        // Display Frame
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
    }
}