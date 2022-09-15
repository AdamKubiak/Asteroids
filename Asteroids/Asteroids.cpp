// Asteroids.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <vector>
#include "olcConsoleGameEngine.h"
#include <array>

using namespace std;

class cObject
{
public:
    float x;
    float y;
    float dx;
    float dy;

    virtual int &size() = 0;

};

class cBullet :public cObject
{
private:
    int nSize;
public:

    cBullet(float x, float y, float dx, float dy, int size)
    {
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
        this->nSize = size;
    }

    int &size() override
    {
        return nSize;
    }
};

class cPlayer : public cObject
{
public:
    cPlayer()
    {
        this->x = 0;
        this->y = 0;
        this->dx = 0;
        this->dy = 0;
        this->angle = 0;
        this->acceleration = 0;
    }
    cPlayer(float x, float y, float dx, float dy, float angle,float acceleration)
    {
        
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
        this->angle = angle;
        this->acceleration = acceleration;
    }

    int &size() override
    {
        return Size;
    }

    void updatePlayer()
    {
        //rotate
        for (int i = 0; i < 3; i++)
        {
            sX[i] = mX[i] * cos(angle) - mY[i] * sin(angle);
            sY[i] = mX[i] * sin(angle) + mY[i] * cos(angle);
        }

        //translate

        for (int i = 0; i < 3; i++)
        {
            sX[i] += x;
            sY[i] += y;
        }
    }
 
public:
    float angle;
    float acceleration;
    float mX[3] = { 0.0f,-2.5f,+2.5f };
    float mY[3] = {-5.5f,2.5f,2.5f};
    float sX[3];
    float sY[3];
    int Size;

};

class objectAsteroid : public cObject
{
private:
    int nSize;
public:
    objectAsteroid()
    {
        this->x = 0;
        this->y = 0;
        this->dx = 0;
        this->dy = 0;
        this->nSize = 0;
    }
    objectAsteroid(float x, float y, float dx, float dy, int nSize)
    {
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
        this->nSize = nSize;
    }

    int &size() override
    {
        return nSize;
    }

};

class Asteroids : public olcConsoleGameEngine
{
public:
    Asteroids()
    {
        m_sAppName = L"Asteroids";
    }

    ~Asteroids()
    {
        for (auto p: spaceObjects)
        {
            delete p;
        }
        spaceObjects.clear();
    }

    void WrapCoordinates(float ix, float iy, float& ox, float& oy)
    {
        ox = ix;
        oy = iy;
        if (ix < 0.0f)	ox = ix + (float)ScreenWidth();
        if (ix >= (float)ScreenWidth())	ox = ix - (float)ScreenWidth();
        if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
        if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
    }

    // Overriden to handle toroidal drawing routines
    virtual  void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
    {
        float fx, fy;
        WrapCoordinates(x, y, fx, fy);
        olcConsoleGameEngine::Draw(fx, fy, c, col);
    }

    virtual  void DrawCircle(float x, float y,int r,float &xc,float &yc, wchar_t c = 0x2588, short col = 0x000F)
    {       
            WrapCoordinates(x, y, xc, yc);
            int xk = 0;
            int yk = r;
            int p = 3 - 2 * r;
            if (!r) return;

            while (yk >= xk) // only formulate 1/8 of circle
            {
                Draw(xc - xk, yc - yk, c, col);//upper left left
                Draw(xc - yk, yc - xk, c, col);//upper upper left
                Draw(xc + yk, yc - xk, c, col);//upper upper right
                Draw(xc + xk, yc - yk, c, col);//upper right right
                Draw(xc - xk, yc + yk, c, col);//lower left left
                Draw(xc - yk, yc + xk, c, col);//lower lower left
                Draw(xc + yk, yc + xk, c, col);//lower lower right
                Draw(xc + xk, yc + yk, c, col);//lower right right
                if (p < 0) p += 4 * xk++ + 6;
                else p += 4 * (xk++ - yk--) + 10;
            }
           
    }

    bool bulletColision(float cx, float cy, float radius, float x, float y)
    {
        return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
    }

    virtual bool OnUserCreate()
    {
        player = new cPlayer(50.0f, -70.0f, 0.0f, 0.0f, 0.0f,20.0f); 
        

        tempAsteroid =  new objectAsteroid(20.0f, 20.f, 8.0f, -6.0f, (int)16);
        spaceObjects.push_back(tempAsteroid);
        tempAsteroid = new objectAsteroid(100.0f, 20.0f, -5.0f, 3.0f, (int)16);
        spaceObjects.push_back(tempAsteroid);

        //spaceObjects.push_back({20.0f,20.f,8.0f,-6.0f})
        return true;
    }

    virtual bool OnUserUpdate(float fElaspedTime)
    {
        if (!playerDead)
        {
            Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

            //update and draw player
            if (m_keys[VK_LEFT].bHeld)
                player->angle -= 5.0f * fElaspedTime;

            if (m_keys[VK_RIGHT].bHeld)
                player->angle += 5.0f * fElaspedTime;

            if (m_keys[VK_UP].bHeld)
            {
                player->dx += sin(player->angle) * player->acceleration * fElaspedTime;
                player->dy += -cos(player->angle) * player->acceleration * fElaspedTime;
            }

            player->x += player->dx * fElaspedTime;
            player->y += player->dy * fElaspedTime;

            WrapCoordinates(player->x, player->y, player->x, player->y);

            player->updatePlayer();

            for (int i = 0; i < 3; i++)
            {
                int j = i + 1;
                DrawLine(player->sX[i % 3], player->sY[i % 3], player->sX[j % 3], player->sY[j % 3]);
            }


            //bullets framework
            if (m_keys[VK_SPACE].bPressed)
            {
                tempBullet = new cBullet(player->x, player->y, 50.0f * sin(player->angle), -50.0f * cos(player->angle), 1);

                bullets.push_back(tempBullet);
            }

            for (auto& a : spaceObjects)
            {
                a->x += a->dx * fElaspedTime;
                a->y += a->dy * fElaspedTime;
                DrawCircle(a->x, a->y, a->size(), a->x, a->y, PIXEL_QUARTER, FG_BLUE);
            }

            for (auto& b : bullets)
            {
                b->x += b->dx * fElaspedTime;
                b->y += b->dy * fElaspedTime;
                Draw(b->x, b->y, PIXEL_QUARTER, FG_YELLOW);

                for (auto& a : spaceObjects)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (bulletColision(a->x, a->y, a->size(), player->sX[i], player->sY[i]))
                        {
                            playerDead = true;
                        }
                    }
                    
                    if (bulletColision(a->x, a->y, a->size(), b->x, b->y))
                    {


                        if (a->size() > 4)
                        {
                            if (a->size() > 8)
                            {
                                float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                                float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                                tempAsteroid = new objectAsteroid(a->x, a->y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)a->size() >> 1);
                                childObjects.push_back(tempAsteroid);
                                tempAsteroid = new objectAsteroid(a->x, a->y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), (int)a->size() >> 1);
                                childObjects.push_back(tempAsteroid);
                            }
                            if (a->size() <= 8)
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    float angle = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                                    tempAsteroid = new objectAsteroid(a->x, a->y, 20.0f * sinf(angle), 20.0f * cosf(angle), (int)a->size() >> 1);
                                    childObjects.push_back(tempAsteroid);
                                }
                            }
                        }
                        a->x = -100;
                        auto i = remove_if(spaceObjects.begin(), spaceObjects.end(), [&](cObject* o) {return (o->x < 0); });

                        if (i != spaceObjects.end())
                            spaceObjects.erase(i);

                        b->x = -1;
                        auto y = remove_if(bullets.begin(), bullets.end(), [&](cObject* o) {return (o->x < 1 || o->y < 1 || o->x >= ScreenWidth() || o->y >= ScreenHeight()); });

                        if (y != bullets.end())
                            bullets.erase(y);


                    }


                }
            }

            //remove bullet of boudry
            if (bullets.size() > 0)
            {
                auto i = remove_if(bullets.begin(), bullets.end(), [&](cObject* o) {return (o->x < 1 || o->y < 1 || o->x >= ScreenWidth() || o->y >= ScreenHeight()); });

                if (i != bullets.end())
                    bullets.erase(i);

            }
            //update and draw asteroids
            for (auto a : childObjects)
            {
                spaceObjects.push_back(a);
            }
            childObjects.clear();

            for (auto a : spaceObjects)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (bulletColision(a->x, a->y, a->size(), player->sX[i], player->sY[i]))
                        playerDead = true;
                }
            }
            return true;
        }


        if (playerDead)
        {
                Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);
                delete player;
                spaceObjects.clear();
                player = new cPlayer(80.0f, -50.0f, 0.0f, 0.0f, 0.0f, 20.0f);


                tempAsteroid = new objectAsteroid(20.0f, 20.f, 8.0f, -6.0f, (int)16);
                spaceObjects.push_back(tempAsteroid);
                tempAsteroid = new objectAsteroid(100.0f, 20.0f, -5.0f, 3.0f, (int)16);
                spaceObjects.push_back(tempAsteroid);

                //spaceObjects.push_back({20.0f,20.f,8.0f,-6.0f})
                playerDead = false;
                return true;
            }
    }
private:
    cObject* tempAsteroid;
    cObject* tempBullet;
    cPlayer* player;
    vector<cObject*> spaceObjects;
    vector<cObject*> childObjects;
    vector<cObject*> bullets;
    public:
        bool playerDead = false;
};




int main()
{
    Asteroids demo;
    demo.ConstructConsole(160, 100, 8, 8);
    demo.Start();

    return 0;
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
