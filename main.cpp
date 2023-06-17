#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <objidl.h>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <array>
#include <algorithm>

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
const int MAX_WEIGHT = 300;
const int P_WEIGHT = 70;
const int state_error = 5;

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
        std::vector<unsigned int> v_up; 
        std::vector<unsigned int> v_down;
        int p_count = 0;
        int pID =0;
        std::map<int, int> winID;
        std::map<int, int> pickID;
        std::array<int, MAX_WEIGHT/P_WEIGHT>winda;
        int weight = 0;
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
    SolidBrush  brush(Gdiplus::Color(0,255,255,255));
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
Button_Data draw_panel(Button_Data dt,HDC hdc,int font_value,int x,int y,int level)
{
    wchar_t L[LEVEL_COUNT][4];
    float length1 = 0, length2 = 0;
    for(int i = 0; i<LEVEL_COUNT; i++)  swprintf(L[i],4,L"%d",i);

    for(int i = 0; i<LEVEL_COUNT; i++)
    {
        if(i%2==0) {
            if(i!=level) dt = drawrectT(hdc,x+(i/2)*length1*font_value*Horizontal_panel_dispersion,y,L[i],font_value,i,dt);
            length1=wcslen(L[i]);
            if(wcslen(L[i])>1)length1=wcslen(L[i])/1.75;
        } 
        if(i%2==1) {
            if(i!=level) dt = drawrectT(hdc,x+(i/2)*length2*font_value*Horizontal_panel_dispersion,y+font_value*Vertical_panel_dispersion,L[i],font_value,i,dt);
            length2=wcslen(L[i]);
            if(wcslen(L[i])>1)length2=wcslen(L[i])/1.75;
        } 
        
    }
    
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
           // std::cout<<std::endl<<dt.buttons[i][0]<<" "<<dt.buttons[i][0]+dt.buttons[i][2]<<std::endl;
            if(dt.buttons[i][1]<=y && dt.buttons[i][1]+dt.buttons[i][3]>y)
            {
           // std::cout<<dt.buttons[i][1]<<" "<<dt.buttons[i][1]+dt.buttons[i][3]<<std::endl;
           // std::cout<<"Button"<<std::endl;
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
    float p[LEVEL_COUNT];

    float tab[LEVEL_COUNT+1];
    tab[0]=0;
    for(int i=1; i<LEVEL_COUNT; i++)
    {
        tab[i]=p[LEVEL_COUNT-i];
    }
    const int font_value=24;
    const int weightp_x = w-30-boxL_width;
    const int weightp_y = 30;
    for (int i = 0; i<LEVEL_COUNT; i++) p[i]=0.8*h*(LEVEL_COUNT-i-1)/LEVEL_COUNT+0.2*h;

    int buf = 0;
    int win_weigth = 0;

   switch(message)
   {
    case WM_PAINT: // pierwsze pomalowanie (inicjalizujące)
      
      hdc = BeginPaint(hWnd, &ps);
      data.win_state = p[0]-win_height;

        drawrect(hdc,w/2-win_width/2,data.win_state,win_width,win_height); // winda
        drawrect(hdc,w/2-win_width/2-win_margin,0.02*h,win_width+2*win_margin,0.92*h); // szyb windy

        for(int i=0;i<LEVEL_COUNT;i++)
        {
            if(i%2==0) {
                drawline(hdc,0,p[i],w/2-win_width/2-win_margin,p[i],5);
                data = draw_panel(data,hdc,font_value,w/40,p[i]-2*font_value*Vertical_panel_dispersion,i);
            } 
            if(i%2==1) {
                drawline(hdc,rect.right,p[i],w/2+win_width/2+win_margin,p[i],5); 
                data = draw_panel(data,hdc,font_value,0.95*w-LEVEL_COUNT*font_value*Horizontal_panel_dispersion/1.7,p[i]-2*font_value*Vertical_panel_dispersion,i);
            } 
        }

        //drawrectL(hdc,weightp_x,weightp_y,0,font_value);
        data = drawrectT(hdc,weightp_x-200,weightp_y,L"Start",font_value,7,data);

   //  std::cout<<std::endl;
    //    for(int i = 0; i<BUTTON_COUNT;i++)
    //    {
    //     std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<" "<<data.buttons[i][4]<<std::endl;
   //     }
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
       // std::cout<<std::endl;
       // for(int i = 0; i<BUTTON_COUNT;i++)
       // {
       //  std::cout<<i<<" "<<data.buttons[i][0]<<" "<<data.buttons[i][1]<<" "<<data.buttons[i][2]<<" "<<data.buttons[i][3]<<" "<<data.buttons[i][4]<<std::endl;
       // }
       // std::cout<<buf<<" "<<LOWORD(lParam)<<" "<<HIWORD(lParam)<<std::endl;
        
        if(buf>=0 && buf<6)
        {
            std::cout<<std::endl<<"buf: " <<buf;         
            switch(buf)
            {
                case 1: data.win_goal= p[0]-win_height; std::cout<<std::endl<<"case 1: " <<buf; break;
                case 2: data.win_goal= p[1]-win_height; std::cout<<std::endl<<"case 2: " <<buf; break;
                case 3: data.win_goal= p[2]-win_height; std::cout<<std::endl<<"case 3: " <<buf; break;
                case 4: data.win_goal= p[3]-win_height; std::cout<<std::endl<<"case 4: " <<buf; break;
                case 5: data.win_goal= p[4]-win_height; std::cout<<std::endl<<"case 5: " <<buf; break;
                default: data.win_goal= data.win_state; std::cout<<std::endl<<"default: " <<buf; break;
                

            }
            std::cout<<std::endl<<"data.win_goal: " <<data.win_goal;
            going_up = dir_decide(lParam,data);
            if(going_up)
            {
                int passengerID = data.pID++;
                data.v_up.push_back(lev_decide(lParam,tab)-win_height);
                data.v_up.push_back(data.win_goal);
                data.winID.insert(std::make_pair(data.win_goal,passengerID));
                data.pickID.insert(std::make_pair(lev_decide(lParam,tab)-win_height,passengerID));
            }
            
            else
            {   
                int passengerID = data.pID++;
                data.v_down.push_back(lev_decide(lParam,tab)-win_height);
                data.v_down.push_back(data.win_goal);
                data.winID.insert(std::make_pair(data.win_goal,passengerID));
                data.pickID.insert(std::make_pair(lev_decide(lParam,tab)-win_height,passengerID));
            }
                       
        }

        
        std::cout<<"if empty: "<<data.q_down.empty()<<" "<<data.q_up.empty()<<std::endl;

        drawrectL(hdc,weightp_x,weightp_y,win_weigth,font_value);
        std::cout<<"going_up "<<going_up<<std::endl;
//przesuwanie windy
        if(buf == 7)
        { 
                   
            std::cout<<"start ";
            int size_up = data.v_up.size();
            int size_down = data.v_down.size();

            std::sort(data.v_up.begin(), data.v_up.end());
            auto unique_end = std::unique(data.v_up.begin(), data.v_up.end());
            data.v_up.erase(unique_end, data.v_up.end());
            std::cout<<std::endl<<"queue up: ";
            for (int i = data.v_up.size()-1;i>=0;i--) 
            {std::cout<<data.v_up[i]<<" ";
                data.q_up.push(data.v_up[i]);
            }
            std::sort(data.v_down.begin(), data.v_down.end());
            auto unique_end2 = std::unique(data.v_down.begin(), data.v_down.end());
            data.v_down.erase(unique_end2, data.v_down.end());
            std::cout<<std::endl<<"queue down: ";
            for (const auto& element : data.v_down) 
            {std::cout<<element<<" ";
                data.q_down.push(element);
            }
            data.v_down.clear();
            data.v_up.clear();

            int i = 0;
            
            //std::cout<<data.q_up.empty()<<" "<<data.q_down.empty();
            while(!(data.q_up.empty()&&data.q_down.empty()))
            {
                std::cout<<std::endl<<"q_down.front: "<<data.q_down.front()<<std::endl;
                 std::cout<<std::endl<<"q_up.front: "<<data.q_up.front()<<std::endl;
                //std::cout<<"test2 ";
                i++;
                if(i==100) break;
                std::cout<<std::endl<<"Weight "<<data.weight;
                std::cout<<std::endl<<"p_count "<<data.p_count;
                data.weight = data.p_count*P_WEIGHT;
                drawrectL(hdc,weightp_x,weightp_y,data.weight,font_value);


                if(data.win_state<data.win_goal+state_error && data.win_state>data.win_goal-state_error)
                {
                    if(data.pickID.count(data.win_goal) > 0)
                    {std::cout<<"test2 ";
                        for(int i = 0; i<data.winda.size();i++)
                        {
                            if(data.winda[i]==0)
                            data.winda[i] = data.pickID[data.win_goal];
                            data.p_count++;
                            data.pickID.erase(data.win_goal);
                            break;
                        }
                    }
                    if(data.winID.count(data.win_goal) > 0)
                    {std::cout<<"test3 ";
                        for(int i = 0; i<data.winda.size();i++)
                        {
                            if(data.winID[data.win_goal]==data.winda[i])
                            {
                                data.winda[i] = 0;
                                data.p_count--;
                                data.winID.erase(data.win_goal);
                            }
                        }
                    }
                }

               

                if(data.q_up.empty()) going_up = false;
                if(data.q_down.empty()) going_up = true;
                if(going_up && !data.q_up.empty())
                {
                    data.win_goal = data.q_up.front();   
                    //std::cout<<std::endl<<"q_up.first(): "<<data.q_up.front();                
                    data.q_up.pop();
                }
                else if(!going_up && !data.q_down.empty())
                {
                    data.win_goal = data.q_down.front();
                   // std::cout<<std::endl<<"q_down.first(): "<<data.q_down.front();
                    data.q_down.pop();
                }
                //std::cout<<"test5 ";
                if(data.win_state<data.win_goal)
                {
                    while(data.win_state<data.win_goal && data.win_state+win_height<0.92*h)
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
                    
                    while(data.win_state>data.win_goal && data.win_state>0.04*h)
                    {
                            
                        hdc = GetDC(hWnd);
                        clearrect(hdc,w/2-win_width/2,data.win_state,win_width,win_height);  
                        drawrect(hdc,w/2-win_width/2,data.win_state-VELOCITY,win_width,win_height);           
                        ReleaseDC(hWnd, hdc);
                        data.win_state = data.win_state-VELOCITY;
                    }
                }
             
                std::cout<<" end " <<std::endl;                  
            }
           
        }
        
        ReleaseDC(hWnd, hdc);
//przesuwanie windy
        return 0;
    
    
        
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
}