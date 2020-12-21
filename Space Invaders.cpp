#include <iostream>
#include <Windows.h>
#include <thread>
#include <list>
#include <vector>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 30;
int nShieldXSize = 4;
int nShieldYSize = 3;
const int nEnemyCount = 7;

struct sPlayer
{
    int x_pos;
    int y_pos;
};

struct sEnemy
{
    int x_pos;
    int y_pos;
    bool isdead = false;
};
struct sShield
{
    sEnemy s1;
    sEnemy s2 = {this->s1.x_pos+1,this->s1.y_pos};
    sEnemy s3 = {this->s2.x_pos+1,this->s1.y_pos};
    sEnemy s4 = {this->s1.x_pos,this->s1.y_pos+1};
    sEnemy s5 = {this->s4.x_pos+1,this->s4.y_pos};
    sEnemy s6 = {this->s5.x_pos+1,this->s4.y_pos};
    sEnemy s7 = {this->s1.x_pos,this->s4.y_pos+1};
    sEnemy s8 = {this->s7.x_pos+1,this->s7.y_pos};
    sEnemy s9 = {this->s8.x_pos+1,this->s7.y_pos};
};

sEnemy aShield1[9] = {sEnemy{nScreenWidth / 3, nScreenHeight - 5}};
list<sPlayer> lShield1;
int main()
{
    // Create Screen Buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;


    sPlayer player{nScreenWidth/2, nScreenHeight-1};


    vector<sPlayer> vBullet;
    sEnemy aEnemy[nEnemyCount] = {sEnemy{(nScreenWidth / 2) - 3,10},sEnemy{(nScreenWidth / 2) - 2,10},
        sEnemy{(nScreenWidth / 2) - 1,10},sEnemy{(nScreenWidth / 2),10},sEnemy{(nScreenWidth / 2) + 1,10},
        sEnemy{(nScreenWidth / 2) + 2,10},sEnemy{(nScreenWidth / 2) + 3,10}};
    //list<sPlayer> lEnemy{sPlayer{(nScreenWidth / 2)-3,10},sPlayer{(nScreenWidth / 2) - 2,10},sPlayer{(nScreenWidth / 2) - 1,10},sPlayer{(nScreenWidth / 2),10},sPlayer{(nScreenWidth / 2) + 1,10},sPlayer{(nScreenWidth / 2) + 2,10},sPlayer{(nScreenWidth / 2) + 3,10}};
    sShield shield1 = {nScreenWidth/3, nScreenHeight-5};
    sShield shield2 = {2 * nScreenWidth / 3, nScreenHeight - 5};
    sShield shield3 = {nScreenWidth-nShieldXSize, nScreenHeight - 5};
    sShield aShield[3] = {sShield{nScreenWidth / 3, nScreenHeight - 5},sShield{2 * nScreenWidth / 3, nScreenHeight - 5},sShield{nScreenWidth - 3,nScreenHeight - 5}};

    int nPlayerDirection = 0; // 0 == STOPPED | 1 == LEFT | 2 == RIGHT
    int nEnemyDirection = 0; // 0 == LEFT | 1 == RIGHT
    int nScore = 0;
    bool bDead = false;
    bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false, bKeyUp = false, bKeyUpOld = false;
    bool bShot = false;

    // Game Loop
    while(1)
    {

        // Get Input ==========================================================================================================
        auto t1 = chrono::system_clock::now();
        while((chrono::system_clock::now() - t1) < 200ms)
        {
            bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
            bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
            bKeyUp = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;

            if(bKeyLeft/* && !bKeyLeftOld*/)
                nPlayerDirection = 1;
            if(bKeyRight/* && !bKeyRightOld*/)
                nPlayerDirection = 2;
            if(bKeyUp && !bKeyUpOld)
                bShot = true;
            bKeyLeftOld = bKeyLeft;
            bKeyRightOld = bKeyRight;
            bKeyUpOld = bKeyUp;
        }
        // Game Logic =========================================================================================================
        

        // Player Movement
        switch(nPlayerDirection)
        {
        case 0: // STOPPED
            break;
        case 1: // LEFT
            player.x_pos--;
            nPlayerDirection = 0;
            break;
        case 2: // RIGHT
            player.x_pos++;
            nPlayerDirection = 0;
            break;
        }

        // Enemy Movement
        switch(nEnemyDirection)
        {
        case 0: // LEFT
            for(int i = 0; i < nEnemyCount; i++)
                aEnemy[i].x_pos--;

            for(int i = 0; i < nEnemyCount; i++)
            {
                if(aEnemy[i].x_pos == 1)
                {
                    for(int j = 0; j < nEnemyCount; j++)
                    {
                        aEnemy[j].y_pos++;
                    }
                    nEnemyDirection++;
                }
            }
            break;
        case 1: // RIGHT
            for(int i = 0; i < nEnemyCount; i++)
                aEnemy[i].x_pos++;
            for(int i = 0; i < nEnemyCount; i++)
            {
                if(aEnemy[i].x_pos == nScreenWidth - 1)
                {
                    for(int i = 0; i < nEnemyCount; i++)
                    {
                        aEnemy[i].y_pos++;
                    }
                    nEnemyDirection--;
                }
            }
            break;
        }

        // Bullet Movement 
        if(!vBullet.empty())
        {
            for(vector<sPlayer>::iterator it = vBullet.begin(); it != vBullet.end();)
            {
                if(it->y_pos > 3)
                {
                    it->y_pos--;
                    it++;
                }
                else
                    it = vBullet.erase(it);
            }
        }

        
        // Collission Detection

        // Bullet vs Enemy
        for(int i = 0; i < nEnemyCount; i++)
        {
            for(std::vector<sPlayer>::iterator it = vBullet.begin(); it != vBullet.end();)
            {
                if(it->x_pos == aEnemy[i].x_pos && it->y_pos == aEnemy[i].y_pos)
                {
                    aEnemy[i].isdead = true;
                    it = vBullet.erase(it);
                    nScore++;
                }
                else
                    it++;
            }
        }

        // Bullet vs Shield
        //TODO: Shields break the game. MUST FIX
        for(int i = 0; i < 3; i++)
        {
            for(std::vector<sPlayer>::iterator it = vBullet.begin(); it != vBullet.end();)
            {
                if(it->x_pos == aShield[i].s1.x_pos && it->y_pos == aShield[i].s1.y_pos)
                {
                    if(!aShield[i].s1.isdead)
                    {
                        aShield[i].s1.isdead = true;
                        it = vBullet.erase(it);
                    }                    
                }
                else if(it->x_pos == aShield[i].s2.x_pos && it->y_pos == aShield[i].s2.y_pos)
                {
                    if(!aShield[i].s2.isdead)
                    {
                        aShield[i].s2.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s3.x_pos && it->y_pos == aShield[i].s3.y_pos)
                {
                    if(!aShield[i].s3.isdead)
                    {
                        aShield[i].s3.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s4.x_pos && it->y_pos == aShield[i].s4.y_pos)
                {
                    if(!aShield[i].s4.isdead)
                    {
                        aShield[i].s4.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s5.x_pos && it->y_pos == aShield[i].s5.y_pos)
                {
                    if(!aShield[i].s5.isdead)
                    {
                        aShield[i].s5.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s6.x_pos && it->y_pos == aShield[i].s6.y_pos)
                {
                    if(!aShield[i].s6.isdead)
                    {
                        aShield[i].s6.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s7.x_pos && it->y_pos == aShield[i].s7.y_pos)
                {
                    if(!aShield[i].s7.isdead)
                    {
                        aShield[i].s7.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s8.x_pos && it->y_pos == aShield[i].s8.y_pos)
                {
                    if(!aShield[i].s8.isdead)
                    {
                        aShield[i].s8.isdead = true;
                        it = vBullet.erase(it);
                    }
                }
                else if(it->x_pos == aShield[i].s9.x_pos && it->y_pos == aShield[i].s9.y_pos)
                {
                    if(!aShield[i].s9.isdead)
                    {
                        aShield[i].s9.isdead = true;
                        it = vBullet.erase(it);
                    }
                    }
                else
                    it++;

            }
        }


        // Display to Player===================================================================================================
        // Clear Screen
        for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
            screen[i] = L' ';

        // Setup Border
        for(int i = 0; i < nScreenWidth; i++)
        {
            screen[i] = L'=';
            screen[2 * nScreenWidth + i] = L'=';
        }
        wsprintf(&screen[nScreenWidth + 5], L"S P A C E    I N V A D E R S              |   Score: %d", nScore);

        // Draw Player
        screen[player.y_pos * nScreenWidth + player.x_pos] = L'O';


        // Draw Enemy
        for(int i = 0; i < nEnemyCount; i++)
            if(!aEnemy[i].isdead)
                screen[aEnemy[i].y_pos * nScreenWidth + aEnemy[i].x_pos] = L'X';
            else
                screen[aEnemy[i].y_pos * nScreenWidth + aEnemy[i].x_pos] = L' ';

        // Draw Shield
        for(int i = 0; i < 3; i++)
        {
            if(!aShield[i].s1.isdead)
                screen[aShield[i].s1.y_pos * nScreenWidth + aShield[i].s1.x_pos] = L'#';
            if(!aShield[i].s2.isdead)
                screen[aShield[i].s2.y_pos * nScreenWidth + aShield[i].s2.x_pos] = L'#';
            if(!aShield[i].s3.isdead)
                screen[aShield[i].s3.y_pos * nScreenWidth + aShield[i].s3.x_pos] = L'#';
            if(!aShield[i].s4.isdead)
                screen[aShield[i].s4.y_pos * nScreenWidth + aShield[i].s4.x_pos] = L'#';
            if(!aShield[i].s5.isdead)
                screen[aShield[i].s5.y_pos * nScreenWidth + aShield[i].s5.x_pos] = L'#';
            if(!aShield[i].s6.isdead)
                screen[aShield[i].s6.y_pos * nScreenWidth + aShield[i].s6.x_pos] = L'#';
            if(!aShield[i].s7.isdead)
                screen[aShield[i].s7.y_pos * nScreenWidth + aShield[i].s7.x_pos] = L'#';
            if(!aShield[i].s8.isdead)
                screen[aShield[i].s8.y_pos * nScreenWidth + aShield[i].s8.x_pos] = L'#';
            if(!aShield[i].s9.isdead)
                screen[aShield[i].s9.y_pos * nScreenWidth + aShield[i].s9.x_pos] = L'#';
        }



        // Draw Bullet
        if(bShot)
        {
            vBullet.push_back(sPlayer{player.x_pos,player.y_pos-1});
            bShot = false;
        }       
        if(!vBullet.empty())
        {
            for(std::vector<sPlayer>::iterator it = vBullet.begin(); it != vBullet.end(); it++)
            {
                screen[it->y_pos * nScreenWidth + it->x_pos] = L'|';
            }
        }
        // Player Death
        if(bDead)
            wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS 'SPACE' TO RESTART");

        // Display Frame
        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
    }
}


// TODO: Convert aShield to a linked list with shield/enemy elements
// TODO: Fix middle and back row of shields