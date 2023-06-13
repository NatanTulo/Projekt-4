#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <objidl.h>
#include <string>
#include <vector>
#include <queue>

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
const int boxL_width = 100;
const int boxL_Heigth = 60;
const int LEVEL_COUNT = 5;

const int VELOCITY = 1;



class Button_Data{
    public:
        int buttons[BUTTON_COUNT][5] ={ //x,y,width,heigth,ID_piętra
            {},
            {},
            {},
            {},
            {},
        };
        int win_goal = 0;
        int win_state = 0;
        std::queue<unsigned int> q_up; 
        std::queue<unsigned int> q_down; 
};

void drawrect(HDC hdc,int Rx, int Ry,int w, int h)
{
    HWND hwnd = GetActiveWindow();
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(a,r,g,b));
    
	graphics.DrawRectangle(&pen,Rx,Ry,w,h);    
}
void drawline(HDC hdc,int x1, int y1,int x2, int y2, int width)
{
    HWND hwnd = GetActiveWindow();
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(a,0,0,0));
    pen.SetWidth(width);
    
    graphics.DrawLine(&pen,x1,y1,x2,y2);
}
void clearrect(HDC hdc,int Rx, int Ry,int w, int h)
{
    HWND hwnd = GetActiveWindow(); // hwnd to inaczej "handle window", czyli jakby wzięcie okienka do obróbki
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(a,255,255,255));
    
	graphics.DrawRectangle(&pen,Rx,Ry,w,h);    
}
Button_Data drawrectT(HDC hdc,int x,int y,const WCHAR* text,int font_value,int ID, Button_Data dt)
{
    
    Gdiplus::Graphics    graphics(hdc);
    SolidBrush  brush(Gdiplus::Color(a,r,g,b));
    FontFamily  fontFamily(L"Comic Sans MS");
    Gdiplus::Font        font(&fontFamily, font_value, FontStyleRegular, UnitPixel);
    PointF      pointF(x,y);
    Gdiplus::Pen         pen(Gdiplus::Color(a,r,g,b));
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
        dt.buttons[i][4] = ID;
        i= BUTTON_COUNT;
     }   
     else
     i++;
     
     
    } while (i<BUTTON_COUNT);
    
    return dt;
}
void drawrectL(HDC hdc,int x,int y,int number,int font_value)
{
    
    Gdiplus::Graphics    graphics(hdc);
    SolidBrush  brush(Gdiplus::Color(255,255,255,255));
    SolidBrush  brush2(Gdiplus::Color(a,r,g,b));
    FontFamily  fontFamily(L"Comic Sans MS");
    Gdiplus::Font        font(&fontFamily, font_value, FontStyleRegular, UnitPixel);
    PointF      pointF(x,y);
    Gdiplus::Pen         pen(Gdiplus::Color(a,r,g,b));
    

    
    std::wstring numberString = std::to_wstring(number);
    const wchar_t* text = numberString.c_str();
    
    graphics.FillRectangle(&brush,x,y,boxL_width,boxL_Heigth);  
    graphics.DrawString(text, -1, &font, pointF, &brush2);
    graphics.DrawRectangle(&pen,x,y,boxL_width,boxL_Heigth);  
   
    
}
Button_Data draw_panel(Button_Data dt,HDC hdc,int font_value,int x,int y,const WCHAR* text1,const WCHAR* text2,const WCHAR* text3,const WCHAR* text4,int ID1, int ID2, int ID3, int ID4)
{
    size_t length1 = wcslen(text1);
    size_t length3 = wcslen(text3);

    dt = drawrectT(hdc,x,y,text1,font_value,ID1,dt);
    dt = drawrectT(hdc,x+length1*font_value*Horizontal_panel_dispersion,y,text2,font_value,ID2,dt);
    dt = drawrectT(hdc,x,y+font_value*Vertical_panel_dispersion,text3,font_value,ID3,dt);
    dt = drawrectT(hdc,x+length3*font_value*Horizontal_panel_dispersion,y+font_value*Vertical_panel_dispersion,text4,font_value,ID4,dt);
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
            return dt.buttons[i][4];
            }
            
            
        }           
    }
    return 0;
}

bool dir_decide(LPARAM lParam, Button_Data data)
{
    if(data.win_goal<HIWORD(lParam))
            return true;
            else
            return false;
}
int lev_decide(LPARAM lParam,const float tab[])
{
    std::cout<<std::endl<<"Mouse Y for lev_decide: "<<HIWORD(lParam)<<std::endl;
    for(int i = 0; i<LEVEL_COUNT;i++)
    {
        std::cout<<std::endl<<"lev_decide process: "<<tab[i]<<" "<<tab[i+1]<<std::endl;
        if(HIWORD(lParam)>=tab[i] && HIWORD(lParam)<tab[i+1])
        {
            std::cout<<std::endl<<"lev_decide output: "<<tab[i+1]<<std::endl;
            return tab[i+1];
        }
        
    }
    
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam,LPARAM lParam)
{
   HDC          hdc;
   PAINTSTRUCT  ps;
   RECT         rect;
   static Button_Data  data;  

   bool going_up = false;
   int w;
   int h;
    if (GetWindowRect(hWnd, &rect))
    {
       w = rect.right - rect.left;
       h = rect.bottom - rect.top;   
         
    }
    int wys=h-30; // do usunięcia po poprawieniu równania na h
    const float p0 =wys/5+2*wys/3;
    const float p1 =2*h/3;
    const float p2 =wys/5+wys/3;
    const float p3 =h/3;
    const float p4 =wys/5;
    const float tab[LEVEL_COUNT+1] ={0,p4,p3,p2,p1,p0};
    const int font_value=24;
    const int weightp_x = w-30-boxL_width;
    const int weightp_y = 30;

    

    int buf = 0;
    int win_weigth = 0;

   switch(message)
   {
    case WM_PAINT: // pierwsze pomalowanie (inicjalizujące)
      
      hdc = BeginPaint(hWnd, &ps);
      data.win_state = h/2;
        drawrect(hdc,w/2-win_width/2,h/2,win_width,win_height); // winda
        drawrect(hdc,w/2-win_width/2-win_margin,4*win_margin,win_width+2*win_margin,wys-5*win_margin); // szyb windy

        drawline(hdc,0,p0,w/2-win_width/2-win_margin,p0,5);                         // 0 piętro (parter)
        data = draw_panel(data,hdc,font_value,w/40,p0-2*font_value*Vertical_panel_dispersion,L"1",L"2",L"3",L"4",2,3,4,5);

        drawline(hdc,rect.right,p1,w/2+win_width/2+win_margin,p1,5);                // 1 piętro
        data = draw_panel(data,hdc,font_value,75*w/80,p1-2*font_value*Vertical_panel_dispersion,L"0",L"2",L"3",L"4",1,3,4,5);

        drawline(hdc,0,p2,w/2-win_width/2-win_margin,p2,5);                         // 2 piętro
        data = draw_panel(data,hdc,font_value,w/40,p2-2*font_value*Vertical_panel_dispersion,L"0",L"1",L"3",L"4",1,2,4,5);

        drawline(hdc,rect.right,p3,w/2+win_width/2+win_margin,p3,5);                // 3 piętro
        data = draw_panel(data,hdc,font_value,75*w/80,p3-2*font_value*Vertical_panel_dispersion,L"0",L"1",L"2",L"4",1,2,3,5);

        drawline(hdc,0,p4,w/2-win_width/2-win_margin,p4,5);                         // 4 piętro
        data = draw_panel(data,hdc,font_value,w/40,p4-2*font_value*Vertical_panel_dispersion,L"0",L"1",L"2",L"3",1,2,3,4);

        drawrectL(hdc,weightp_x,weightp_y,0,font_value);
        data = drawrectT(hdc,weightp_x-200,weightp_y,L"Start",font_value,7,data);

     std::cout<<std::endl;
        for(int i = 0; i<BUTTON_COUNT;i++)
        {
         std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<" "<<data.buttons[i][4]<<std::endl;
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
         std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<" "<<data.buttons[i][4]<<std::endl;
        }
       // std::cout<<buf<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<std::endl;
        
        if(buf>=0 && buf<6)
        {
            std::cout<<std::endl<<"buf: " <<buf;         
            switch(buf)
            {
                case 1: data.win_goal= p0-win_height; std::cout<<std::endl<<"case 1: " <<buf; break;
                case 2: data.win_goal= p1-win_height; std::cout<<std::endl<<"case 2: " <<buf; break;
                case 3: data.win_goal= p2-win_height; std::cout<<std::endl<<"case 3: " <<buf; break;
                case 4: data.win_goal= p3-win_height; std::cout<<std::endl<<"case 4: " <<buf; break;
                case 5: data.win_goal= p4-win_height; std::cout<<std::endl<<"case 5: " <<buf; break;
                default: data.win_goal= data.win_state; std::cout<<std::endl<<"default: " <<buf; break;
                

            }
            std::cout<<std::endl<<"data.win_goal: " <<data.win_goal;
            going_up = dir_decide(lParam,data);
            if(going_up)
            {
                data.q_up.push(lev_decide(lParam,tab)-win_height);
                data.q_up.push(data.win_goal);
            }
            
            else
            {
                data.q_down.push(lev_decide(lParam,tab)-win_height);
                data.q_down.push(data.win_goal);
            }
            
        }

        
        std::cout<<"if empty: "<<data.q_down.empty()<<" "<<data.q_up.empty()<<std::endl;
        std::cout<<"q_down.front: "<<data.q_down.front()<<std::endl;
        std::cout<<"q_up.front: "<<data.q_up.front()<<std::endl;
        drawrectL(hdc,weightp_x,weightp_y,win_weigth,font_value);
        std::cout<<"going_up "<<going_up<<std::endl;
//przesuwanie windy
        if(buf == 7)
        { int i = 0;
            std::cout<<"test ";
            //std::cout<<data.q_up.empty()<<" "<<data.q_down.empty();
            while(!(data.q_up.empty()&&data.q_down.empty()))
            {
                //std::cout<<"test2 ";
                i++;
                if(i==150) break;

               // for(int i = 0;i<data.q_down.size();i++)
               // {
               //     std::cout<<data.q_down.front()<<" ";
               // }

                std::cout<<" end " ;
                if(data.q_up.empty()) going_up = false;
                if(going_up && !data.q_up.empty())
                {
                    data.win_goal = data.q_up.front();   
                    std::cout<<std::endl<<"q_up.first(): "<<data.q_up.front();                
                    data.q_up.pop();
                }
                else if(!going_up && !data.q_down.empty())
                {
                    data.win_goal = data.q_down.front();
                    std::cout<<std::endl<<"q_down.first(): "<<data.q_down.front();
                    data.q_down.pop();
                }
                //std::cout<<"test5 ";
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
                
            }
           
        }
        
        ReleaseDC(hWnd, hdc);
//przesuwanie windy
        return 0;
    
        
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
}