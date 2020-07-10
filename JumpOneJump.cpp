#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include <stdlib.h>
using namespace std;
COLORREF maps[1024][768];
#define person RGB(56,59,102)
#define sqr(x) ((x)*(x))
#define eps 5
#define DEBUG_R 3
struct  pos
{
    pos(int _x,int _y):x(_x),y(_y)
    {}
    int x, y;
};
COLORREF tmpColor,tmpColor1;
template <typename T>
auto count(std::vector<T> input)
{
    std::map<T, int> statistics;
    for (auto data : input)
        statistics[data]++;
    std::vector<std::pair<T, int>> returnValue;
    for (auto data : statistics)
    {
        returnValue.push_back(std::make_pair(data.first, data.second));
    }
    return  returnValue;
}
int distance(COLORREF d1, COLORREF d2)
{
    return sqrt(sqr(GetRValue(d1) - GetRValue(d2)) + sqr(GetGValue(d1) - GetGValue(d2)) + sqr(GetBValue(d1) - GetBValue(d2)));
}
int distance(pos d1, pos d2)
{
    return sqrt(sqr(d1.x - d2.x) + sqr(d1.y - d2.y));
}
bool checkDistance(COLORREF c1, COLORREF c2)
{
    return distance(c1, c2) < eps;
}
void debug(HDC now, int x, int y)
{
    printf("start debug");
    for (int i = 1; i <= 1024; i++)
        for (int xx = x - DEBUG_R; xx <= x + DEBUG_R; xx++)
            for (int yy = y - DEBUG_R; yy < y + DEBUG_R; yy++)
                SetPixel(now, xx, yy, RGB(100, 100, 100));
}
#define DEBUG(now) cout<<(int)GetRValue(now)<<" "<<(int)GetGValue(now)<<" "<<(int)GetBValue(now)<<"\n"
void   GetCaptureBmp(HWND now,LONG width,LONG height)
{
    HDC     hDC;
    HDC     MemDC;
    BYTE* Data;
    HBITMAP   hBmp;
    BITMAPINFO   bi;
    memset(&bi, 0, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFO);
    RECT rect;
    GetWindowRect(now, &rect);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;
    
    hDC = GetDC(now);  
    MemDC = CreateCompatibleDC(hDC);
    hBmp = CreateDIBSection(MemDC, &bi, DIB_RGB_COLORS, (void**)&Data, NULL, 0);
    SelectObject(MemDC, hBmp);
    BitBlt(MemDC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, hDC, 0, 0, SRCCOPY);
    ReleaseDC(now, hDC); 
    for (int i = 0; i < width; i++)
        for (int j=0;j<height;j++)
            maps[i][j]=GetPixel(MemDC, i, j);
    DeleteDC(MemDC); 

}   
int getBoxYpos(int width,int y)//扫描分析
{
    int check = 0;
    int maxn = 0;
    for (int j = 0; j < y; j++)
    {
        vector<COLORREF> temp;
        for (int i = 0; i < width; i++)
            temp.push_back(maps[i][j]);
        auto result = count(temp);
       sort(result.begin(), result.end(), [](auto a, auto b)
            {
                return a.second > b.second;
            });
       if (result[1].second > 80)
       {
           if (!check)
               check = 1;
           if (check == 1)
               if (result[1].second > maxn)
                   maxn = result[1].second;
               else
               {
                   tmpColor = result[1].first;
                   return j;
               }
       }
    }
}
int getBoxXPos(int width,int height,int y,int maxY)
{        
    vector<pair<int, int>> total;
    for (int i = 0; i <= width; i++)
        if (maps[i][y] == tmpColor)
        {

            int temp=0;
            for (int j = 0; j < maxY; j++)
                if (maps[i][j] == tmpColor)
                    temp++;
            total.push_back(move(make_pair(temp, i)));
    }
    return max_element(total.begin(), total.end(), [](auto x, auto y)
        {
            return x.first < y.first;
        })->second;
}
int main()
{
    auto window = ::GetDesktopWindow();
    window = ::FindWindowEx(window, 0, NULL, L"跳一跳"); 
    RECT rect;        
    window = ::FindWindowEx(window, 0, L"MINIGAMEVIEW", NULL);
    GetWindowRect(window, &rect);   
/*SendMessage(window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(100,100));
    Sleep(5000);
    SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(100,100));*/
    while (true)
    {
        GetCaptureBmp(window, rect.right - rect.left, rect.bottom - rect.top);
        auto dc = GetDC(window);
        // debug
        auto p = pos(0, 0);
        for (int j = 0; j < 768; j++)
            for (int i = 0; i < 1024; i++)
                if (checkDistance(maps[i][j], person))
                    p = pos(i, j);
        for (int i = 0; i < 1024; i++)
            if (checkDistance(maps[i][p.y], person))
            {
                p = pos((i + p.x) / 2, p.y);
                break;
            }
        debug(dc, p.x, p.y);
        auto yPos = getBoxYpos(rect.right - rect.left, p.y);
        auto xPos = getBoxXPos(rect.right - rect.left, rect.bottom - rect.top, yPos, p.y);
        auto next = pos(xPos, yPos);
        debug(dc, next.x, next.y);
        SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(100, 100));
        Sleep((float)distance(next, p) * 3.27);     
        SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(100, 100));
        Sleep(2000);
    }
}

