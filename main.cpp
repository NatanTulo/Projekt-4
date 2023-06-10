#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <objidl.h>
#include <string>
#include <map>

const int a = 255;
const int r = 0;
const int g = 20;
const int b = 255;
const int win_width =150; // szerokość i wysokość windy
const int win_height = 250;
const int win_margin = 5;
const int BUTTON_COUNT = 5;
int win_goal = 0;
int win_state = 0;
const int VELOCITY = 1;

int buttons[BUTTON_COUNT][4] ={ //x,y,width,heigth
    {},
    {},
    {},
    {},
    {},
};


using namespace Gdiplus;

void drawrect(HDC hdc,int Rx, int Ry,int w, int h)
{
    HWND hwnd = GetActiveWindow();
	Gdiplus::Graphics graphics(hdc);
	Pen pen(Color(a,r,g,b));
    
	graphics.DrawRectangle(&pen,Rx,Ry,w,h);    
}
void drawline(HDC hdc,int x1, int y1,int x2, int y2, int width)
{
    HWND hwnd = GetActiveWindow();
	Gdiplus::Graphics graphics(hdc);
	Pen pen(Color(a,0,0,0));
    pen.SetWidth(width);
    
    graphics.DrawLine(&pen,x1,y1,x2,y2);
}
void clearrect(HDC hdc,int Rx, int Ry,int w, int h)
{
    HWND hwnd = GetActiveWindow(); // hwnd to inaczej "handle window", czyli jakby wzięcie okienka do obróbki
	Gdiplus::Graphics graphics(hdc);
	Pen pen(Color(a,255,255,255));
    
	graphics.DrawRectangle(&pen,Rx,Ry,w,h);    
}
void drawrectT(HDC hdc,int x,int y,const WCHAR* text,int font_value)
{
    
    Gdiplus::Graphics    graphics(hdc);
    SolidBrush  brush(Color(a,r,g,b));
    FontFamily  fontFamily(L"Comic Sans MS");
    Font        font(&fontFamily, font_value, FontStyleRegular, UnitPixel);
    PointF      pointF(x,y);
    Pen         pen(Color(a,r,g,b));
    size_t length = wcslen(text);
    graphics.DrawString(text, length, &font, pointF, &brush);
    graphics.DrawRectangle(&pen,x,y,font_value*length,font_value*1.5);  
    int i = 0;
    do
    {
        
     if(buttons[i][0]==0) 
     {
        buttons[i][0] = x;
        buttons[i][1] = y;
        buttons[i][2] = font_value*length/2;
        buttons[i][3] = font_value*1.2;
        i= BUTTON_COUNT;
     }   
     else
     i++;
     
     
    } while (i<BUTTON_COUNT);
    
    
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;
   HDC                 hdc;
   
   // Initialize GDI+.
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   
   wndClass.style          = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc    = WndProc;
   wndClass.cbClsExtra     = 0;
   wndClass.cbWndExtra     = 0;
   wndClass.hInstance      = hInstance;
   wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wndClass.lpszMenuName   = NULL;
   wndClass.lpszClassName  = TEXT("Winda");
   
   RegisterClass(&wndClass);
   
   hWnd = CreateWindow(
      TEXT("Winda"),   // window class name
      TEXT("Winda"),  // window caption
      WS_OVERLAPPEDWINDOW  & ~WS_SIZEBOX,    // window style
      CW_USEDEFAULT,            // initial x position
      CW_USEDEFAULT,            // initial y position
      CW_USEDEFAULT,            // initial x size
      CW_USEDEFAULT,            // initial y size
      NULL,                     // parent window handle
      NULL,                     // window menu handle
      hInstance,                // program instance handle
      NULL);                    // creation parameters
      
   ShowWindow(hWnd, iCmdShow);
   UpdateWindow(hWnd);
    int width;
    int height;
    
    RECT rect;
    if (GetWindowRect(hWnd, &rect))
    {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;       
    }
    std::cout << win_state<<std::endl;
    win_goal = height/2;
    std::cout << win_goal<<std::endl;

    if(win_state<win_goal)
    {
        while(win_state<win_goal)
        {
            win_state = win_state+VELOCITY;
            hdc = GetDC(hWnd);
            clearrect(hdc,width/2,win_state+1,win_width,win_height);
            drawrect(hdc,width/2,win_state,win_width,win_height);
            clearrect(hdc,width/2,win_state-1,win_width,win_height);
            ReleaseDC(hWnd, hdc);
        }
        
    }
    


   
   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}
int check_coords(LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    for(int i = 0; i<BUTTON_COUNT; ++i)
    {
        if(buttons[i][0]<=x && buttons[i][0]+buttons[i][2]>x)
        {
            if(buttons[i][1]<=y && buttons[i][1]+buttons[i][3]>y)
            return i+1;
            
        }           
    }
    return 0;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam,LPARAM lParam)
{
   HDC          hdc;
   PAINTSTRUCT  ps;
   RECT rect;
   int w;
   int h;
    if (GetWindowRect(hWnd, &rect))
    {
       w = rect.right - rect.left;
       h = rect.bottom - rect.top;       
    }
    int buf;
   
   switch(message)
   {
    case WM_PAINT: // pierwsze pomalowanie (inicjalizujące)
        hdc = BeginPaint(hWnd, &ps);
        win_state = h/2;
        drawrect(hdc,w/2-win_width/2,h/2,win_width,win_height); // winda
        drawrect(hdc,w/2-win_width/2-win_margin,5,win_width+2*win_margin,h-5); // szyb windy
        drawline(hdc,0,h/5,w/2-win_width/2-win_margin,h/5,5); // lewe piętra --
        drawline(hdc,0,2*h/5,w/2-win_width/2-win_margin,2*h/5,5);
        drawline(hdc,0,3*h/5,w/2-win_width/2-win_margin,3*h/5,5);
        drawline(hdc,0,4*h/5,w/2-win_width/2-win_margin,4*h/5,5); // lewe piętra --
        drawline(hdc,rect.right,h/5,w/2+win_width/2+win_margin,h/5,5); // prawe piętra --
        drawline(hdc,rect.right,2*h/5,w/2+win_width/2+win_margin,2*h/5,5);
        drawline(hdc,rect.right,3*h/5,w/2+win_width/2+win_margin,3*h/5,5);
        drawline(hdc,rect.right,4*h/5,w/2+win_width/2+win_margin,4*h/5,5); // prawe piętra --
        drawrectT(hdc,w-w/4,h/4,L"Add Weight",24);
        drawrectT(hdc,w-w/4,h/4+24*2,L"1",24);
        drawrectT(hdc,w-w/4,h/4+24*4,L"0",24);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_DESTROY: // coś tam do wyłączania programu, po prostu musi być aby komputer nie wybuchnął
      PostQuitMessage(0);
      return 0;
    case WM_LBUTTONDOWN: // reakcja na wciśnięcie lewego przycisku myszy
        hdc = GetDC(hWnd);
        buf = check_coords(lParam); // reakcja guzika, buf jest zmienną która daje ID wciśniętego guzika
        drawrect(hdc,buttons[buf][0]+10,buttons[buf][1],buttons[buf][2],buttons[buf][3]); // tutaj na przykład wyświetla się prostokąt na koordynatach guzika jako znak, że działa
        /* //debugging macierzy guzików
        std::cout<<std::endl;
        for(int i = 0; i<BUTTON_COUNT;i++)
        {
         std::cout<<i<<" "<<buttons[i][0]<<" "<<buttons[i][1]<<" "<<buttons[i][2]<<" "<<buttons[i][3]<<std::endl;
        }
        std::cout<<buf<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<std::endl;
        */
        if(buf==2 && win_state+win_height<h-10)
        {
            std::cout << win_goal<<std::endl;
            win_goal= win_goal+50;
            std::cout << win_goal;
        }
        if(buf==3 && win_state>10) 
        {
            std::cout << win_goal<<std::endl;
            win_goal= win_goal-50;
            std::cout << win_goal<<std::endl;
            std::cout << win_state;
        }
//przesuwanie windy
        if(win_state<win_goal)
        {
            while(win_state<win_goal && win_state+win_height<h-10)
            {              
                hdc = GetDC(hWnd);
                clearrect(hdc,w/2-win_width/2,win_state,win_width,win_height);  
                drawrect(hdc,w/2-win_width/2,win_state+VELOCITY,win_width,win_height);           
                ReleaseDC(hWnd, hdc);
                win_state = win_state+VELOCITY;
            }
        }
        if(win_state>win_goal)
        {
            
            while(win_state>win_goal && win_state>10)//
            {
                      
                hdc = GetDC(hWnd);
                clearrect(hdc,w/2-win_width/2,win_state,win_width,win_height);  
                drawrect(hdc,w/2-win_width/2,win_state-VELOCITY,win_width,win_height);           
                ReleaseDC(hWnd, hdc);
                win_state = win_state-VELOCITY;
            }
        }
        ReleaseDC(hWnd, hdc);
//przesuwanie windy
        return 0;
    case WM_MOUSEMOVE:
         // tu można dać coś tak myszka się rusza
        return 0;
        
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
}