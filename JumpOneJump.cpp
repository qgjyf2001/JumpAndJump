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
vector<COLORREF> background = { RGB(171, 167, 119),RGB(133,136,160),RGB(140,142,149),RGB(168,149,148),RGB(142,160,154),RGB(168,147,114) };
#define sqr(x) ((x)*(x))
#define eps 8
#define DEBUG_R 3
#define maxX 440
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
void debugLine(HDC now, int y)
{
    printf("start debug");
    for (int i = 1; i <= 40; i++)
        for (int x = 0; x < maxX; x++)
            SetPixel(now, x, y, RGB(100, 100, 100));
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
int getBoxXPos(int width,int y,int px)
{        
    //98 345
    //133 345
#define head RGB(52,52,59)
    for (int j = 200; j < y; j++)
        for (int i = 0; i <= width-20; i++)
            if (maps[i][j] != maps[maxX][j])
            {
                if (checkDistance(maps[i][j], head))
                    return px + (y - j)/5*4;
                return i;
                
            }


}
int main()
{
    double times = 3.783;
    printf("进行下一步前需要打开跳一跳点击开始游戏\n请输入倍率，按回车确定(推荐值3.783)");
   // cin >> times;
    auto window = ::GetDesktopWindow();
    window = ::FindWindowEx(window, 0, NULL, L"跳一跳"); 
    RECT rect;        
    window = ::FindWindowEx(window, 0, L"MINIGAMEVIEW", NULL);
    GetWindowRect(window, &rect);   
/*SendMessage(window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(100,100));
    Sleep(5000);
    SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(100,100));*/     
    auto dc = GetDC(window);
    while (true)
    {
        GetCaptureBmp(window, rect.right - rect.left, rect.bottom - rect.top);
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
        
        //auto yPos = getBoxYpos(rect.right - rect.left, p.y);
        auto xPos = getBoxXPos(rect.right - rect.left, p.y,p.x);
       /* auto next = pos(xPos, yPos);
        debug(dc, next.x, next.y);
        debugLine(dc, next.y);*/
       SendMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(100, 100));
        Sleep((float)(fabs(xPos-p.x)) * times);     
        SendMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(100, 100));
       
        Sleep(2000);
    }
}

