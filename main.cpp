#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <objidl.h>
#include <string>
#include <map>
using namespace Gdiplus;

const int a = 255;
const int r = 0;
const int g = 20;
const int b = 255;
const int win_width =200; // szerokość i wysokość windy
const int win_height = 120;
const int win_margin = 5;
const int BUTTON_COUNT = 30;
const float Horizontal_panel_dispersion = 1.2;
const float Vertical_panel_dispersion = 2;
const double Textbox_heigth_mult = 1.5;

const int VELOCITY = 1;



class Button_Data{
private:


public:
int buttons[BUTTON_COUNT][4] ={ //x,y,width,heigth
    {},
    {},
    {},
    {},
    {},
};
int win_goal = 0;
int win_state = 0;
};
Button_Data data;  

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
Button_Data drawrectT(HDC hdc,int x,int y,const WCHAR* text,int font_value, Button_Data dt)
{
    
    Gdiplus::Graphics    graphics(hdc);
    SolidBrush  brush(Color(a,r,g,b));
    FontFamily  fontFamily(L"Comic Sans MS");
    Font        font(&fontFamily, font_value, FontStyleRegular, UnitPixel);
    PointF      pointF(x,y);
    Pen         pen(Color(a,r,g,b));
    size_t length = wcslen(text);
    graphics.DrawString(text, length, &font, pointF, &brush);
    graphics.DrawRectangle(&pen,x,y,font_value*length,font_value*Textbox_heigth_mult);  
    int i = 0;
    do
    {
        
     if(dt.buttons[i][0]==0) 
     {
        dt.buttons[i][0] = x;
        dt.buttons[i][1] = y;
        dt.buttons[i][2] = font_value*length;
        dt.buttons[i][3] = font_value*Textbox_heigth_mult;
        i= BUTTON_COUNT;
     }   
     else
     i++;
     
     
    } while (i<BUTTON_COUNT);
    
    return dt;
}
Button_Data draw_panel(Button_Data dt,HDC hdc,int x,int y,const WCHAR* text1,const WCHAR* text2,const WCHAR* text3,const WCHAR* text4,int font_value)
{
    size_t length1 = wcslen(text1);
    size_t length3 = wcslen(text3);

    dt = drawrectT(hdc,x,y,text1,font_value,dt);
    dt = drawrectT(hdc,x+length1*font_value*Horizontal_panel_dispersion,y,text2,font_value,dt);
    dt = drawrectT(hdc,x,y+font_value*Vertical_panel_dispersion,text3,font_value,dt);
    dt = drawrectT(hdc,x+length3*font_value*Horizontal_panel_dispersion,y+font_value*Vertical_panel_dispersion,text4,font_value,dt);
    return dt;
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

    



   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}
int check_coords(LPARAM lParam, Button_Data dt)
{
    
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    for(int i = 0; i<BUTTON_COUNT; ++i)
    {
        
        if(dt.buttons[i][0]<=x && dt.buttons[i][0]+dt.buttons[i][2]>x)
        {
            std::cout<<std::endl<<dt.buttons[i][0]<<" "<<dt.buttons[i][0]+dt.buttons[i][2]<<std::endl;
            if(dt.buttons[i][1]<=y && dt.buttons[i][1]+dt.buttons[i][3]>y)
            {
            std::cout<<dt.buttons[i][1]<<" "<<dt.buttons[i][1]+dt.buttons[i][3]<<std::endl;
            std::cout<<"Button"<<std::endl;
            return i+1;
            }
            
            
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
    int buf = 0;
   int wys=h-30; // do usunięcia po poprawieniu równania na h
   switch(message)
   {
    case WM_PAINT: // pierwsze pomalowanie (inicjalizujące)
      
      hdc = BeginPaint(hWnd, &ps);
      data.win_state = h/2;
        drawrect(hdc,w/2-win_width/2,h/2,win_width,win_height); // winda
        drawrect(hdc,w/2-win_width/2-win_margin,win_margin,win_width+2*win_margin,wys-2*win_margin); // szyb windy
        drawline(hdc,0,wys/5+2*wys/3,w/2-win_width/2-win_margin,wys/5+2*wys/3,5);   // 1 piętro
        drawline(hdc,rect.right,2*h/3,w/2+win_width/2+win_margin,2*h/3,5);          // 2 piętro
        drawline(hdc,0,wys/5+wys/3,w/2-win_width/2-win_margin,wys/5+wys/3,5);       // 3 piętro
        drawline(hdc,rect.right,h/3,w/2+win_width/2+win_margin,h/3,5);              // 4 piętro
        drawline(hdc,0,wys/5,w/2-win_width/2-win_margin,wys/5,5);                   // 5 piętro
     data = draw_panel(data,hdc,w-w/4,h/4,L"1",L"1",L"1",L"1",24);
     
     std::cout<<std::endl;
        for(int i = 0; i<BUTTON_COUNT;i++)
        {
         std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<std::endl;
        }
        std::cout<<buf<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<std::endl;
      EndPaint(hWnd, &ps);
      return 0;
    case WM_DESTROY: // coś tam do wyłączania programu, po prostu musi być aby komputer nie wybuchnął
      PostQuitMessage(0);
      return 0;
    case WM_LBUTTONDOWN: // reakcja na wciśnięcie lewego przycisku myszy
        hdc = GetDC(hWnd);
        
        buf = check_coords(lParam,data); // reakcja guzika, buf jest zmienną która daje ID wciśniętego guzika
         
         //debugging macierzy guzików
        std::cout<<std::endl;
        for(int i = 0; i<BUTTON_COUNT;i++)
        {
         std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<std::endl;
        }
        std::cout<<buf<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<std::endl;
        
        if(buf==2 && data.win_state+win_height<h-10)
        {
            std::cout << data.win_goal<<std::endl;
            data.win_goal= 2*wys/3+wys/5;
            std::cout << data.win_goal;
        }
        if(buf==3 && data.win_state>10) 
        {
            std::cout << data.win_goal<<std::endl;
            data.win_goal= 2*h/3;
            std::cout << data.win_goal<<std::endl;
            std::cout << data.win_state;
        }
//przesuwanie windy
        if(data.win_state<data.win_goal)
        {
            while(data.win_state<data.win_goal && data.win_state+win_height<wys-2*win_margin)
            {              
                hdc = GetDC(hWnd);
                clearrect(hdc,w/2-win_width/2,data.win_state,win_width,win_height);  
                drawrect(hdc,w/2-win_width/2,data.win_state+VELOCITY,win_width,win_height);           
                ReleaseDC(hWnd, hdc);
                data.win_state = data.win_state+VELOCITY;
            }
        }
        if(data.win_state>data.win_goal)
        {
            
            while(data.win_state>data.win_goal && data.win_state>2*win_margin)//
            {
                      
                hdc = GetDC(hWnd);
                clearrect(hdc,w/2-win_width/2,data.win_state,win_width,win_height);  
                drawrect(hdc,w/2-win_width/2,data.win_state-VELOCITY,win_width,win_height);           
                ReleaseDC(hWnd, hdc);
                data.win_state = data.win_state-VELOCITY;
            }
        }
        ReleaseDC(hWnd, hdc);
//przesuwanie windy
        return 0;
    
        
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
}