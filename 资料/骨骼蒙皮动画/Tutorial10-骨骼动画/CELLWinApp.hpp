#pragma once

#include <Windows.h>
#include <tchar.h>
#include "glew.h"


namespace   CELL
{
namespace   Graphy
{

class   CELLWinApp
{
protected:
    /**
    *   应用程序实例句柄
    */
    HINSTANCE                   _hInstance;
    /**
    *   窗口句柄，操作窗口使用
    */
    HWND                        _hWnd;
    /**
    *   设备上下文，没有Opengl，绘图都使用它来。
    */
    HDC                         _hDC;
    /**
    *   Opengl上下文句柄。
    */
    HGLRC                       _hRC;
    /**
    *   窗口的宽度和高度
    */
    int                         _winWidth;
    int                         _winHeight;
public:
    CELLWinApp(HINSTANCE hInstance = 0)
    {
        _hWnd                   =   0;
        _hDC                    =   0;
        _hRC                    =   0;
        _winWidth               =   0;
        _winHeight              =   0;
        _hInstance              =   hInstance;
        /**
        *   要想创建一个窗口，首先要注册一个窗口类
        *   相关内存，可以了解windows变成，这里不做介绍。
        */
        ::WNDCLASSEX winClass;
        winClass.lpszClassName  =   _T("CELLWinApp");
        winClass.cbSize         =   sizeof(::WNDCLASSEX);
        winClass.style          =   CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
        winClass.lpfnWndProc    =   windowProc;
        winClass.hInstance      =   hInstance;
        winClass.hIcon	        =   0;
        winClass.hIconSm	    =   0;
        winClass.hCursor        =   LoadCursor(NULL, IDC_ARROW);
        winClass.hbrBackground  =   (HBRUSH)GetStockObject(BLACK_BRUSH);
        winClass.lpszMenuName   =   NULL;
        winClass.cbClsExtra     =   0;
        winClass.cbWndExtra     =   0;
        RegisterClassEx(&winClass);
    }
    virtual ~CELLWinApp()
    {
        destroy();
    }
    void    destroy()
    {
        shutDownDevice();
    }
    
    /**
    *   渲染函数
    */
    virtual void    render()
    {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        SwapBuffers( _hDC );
    }

    /**
    *   入口函数
    *   width :创建窗口宽度
    *   height:创建窗口的高度
    */
    int     start(int width,int height)
    {
        _winWidth   =   width;
        _winHeight  =   height;

        /**
        *   创建窗口
        */
        if (!_createWindow(_winWidth,_winHeight))
        {
            return  -1;
        }
        /**
        *   初始化OpenGL环境。
        */
        if (!initDevice())
        {
            return  -2;
        }
        onInit();
        /**
        *   进入消息循环
        */
        MSG msg =   {0};
        while(msg.message != WM_QUIT)
        {
            if (msg.message == WM_DESTROY || 
                msg.message == WM_CLOSE)
            {
                break;
            }
            /**
            *   有消息，处理消息，无消息，则进行渲染绘制
            */
            if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
            { 
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
            {
                render();
            }
        }
        return  0;
    }
    /**
    *   初始化OpenGL
    */
    bool    initDevice()
    {
        /**
        *   这个东西就是初始化Opengl环境的东西。
        */
        unsigned    pixelFormat;
        PIXELFORMATDESCRIPTOR pfd = 
        {
            sizeof(PIXELFORMATDESCRIPTOR), 
            1, 
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SUPPORT_DIRECTDRAW | PFD_SWAP_EXCHANGE,
            PFD_TYPE_RGBA, 
            32, 
            8, 
            0,
            8,
            8,
            8,
            0,
            0, 
            0,
            32, 
            8,
            8,
            8, 
            8,
            24,
            8, 
            0, 
            PFD_MAIN_PLANE, 
            0,
            0, 
            0,
            0
        };
        /**
        *   获取当前窗口dc
        */
        _hDC        =   GetDC( _hWnd );
        /**
        *   选择一个与pfd接近的像素格式。
        *   你也可以调用int iCnt = DescribePixelFormat(_hDC, pixelFormat,0)返回值为，有多少个可用的
        *   pfd，然后循环的调用，得到你想要的。
        *   for( int i = 1 ;i < iCnt ; ++ i )
        *   {
        *       PIXELFORMATDESCRIPTOR pfd;
        *       DescribePixelFormat(_hDC, iCnt,&pfd);
        *   }
        */
        pixelFormat =   ChoosePixelFormat( _hDC, &pfd );
        DescribePixelFormat( _hDC, pixelFormat, sizeof(pfd), &pfd );
        /**
        *   设置当前dc的pfd;
        */
        if(!SetPixelFormat( _hDC, pixelFormat, &pfd ))
        {
            return  false;
        }
        /**
        *   创建Opengl上下文
        */
        _hRC        =   wglCreateContext( _hDC );
        /**
        *   与当前的dc进行绑定
        *   注意如果是在多线程中进行绘制，这句话在绘制之前调用，用作线程切换使用
        */
        if(!wglMakeCurrent( _hDC, _hRC ))
        {
            return  false;
        }
      
        RECT    rt;
        GetClientRect(_hWnd,&rt);
        /**
        *   窗口发生变化通知
        */
        _winWidth   =   rt.right - rt.left;
        _winHeight  =   rt.bottom - rt.top;
        SendMessage(_hWnd,WM_SIZE,0,0);
        return  true;
    }
    /**
    *   关闭
    */
    void    shutDownDevice()
    {
        if( _hRC != NULL )
        {
            wglMakeCurrent( NULL, NULL );
            wglDeleteContext( _hRC );
            _hRC = NULL;							
        }
        if( _hDC != NULL )
        {
            ReleaseDC( _hWnd, _hDC );
            _hDC = NULL;
        }
        UnregisterClass( _T("CELLWinApp"), _hInstance );
    }
    /**
    *   事件
    */
    virtual int     events(unsigned msg, unsigned wParam, unsigned lParam)
    {
        switch( msg )
        {
        case WM_SIZE:
            {
                RECT    rt;
                GetClientRect(_hWnd,&rt);
                _winWidth   =   rt.right - rt.left;
                _winHeight  =   rt.bottom - rt.top;
                if (_hRC)
                {
                    glViewport(0,0,_winWidth,_winHeight);
                }
            }
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            {
                ::PostQuitMessage(0);
            }
            break;
        default:
            return DefWindowProc(_hWnd, msg, wParam, lParam );
        }
        return  0;
    }
public:
    /**
    *   增加一个初始化OpenGL的函数,第二课中增加
    *   调用该函数完成对OpenGL的基本状态的初始化
    *   在进入消息循环之前的一次通知,只调用一次
    */
    virtual void    onInit()
    {
        /**
        *   清空窗口为红色
        */
        glClearColor(0,0,0,1);
        /**
        *   设置OpenGL视口的位置和大小。
        */
        glViewport( 0, 0, (GLint) _winWidth, (GLint) _winHeight );
    }
protected:
    /**
    *   创建窗口函数
    */
    bool    _createWindow(int width,int height)
    {
        _hWnd   =   CreateWindowEx(
                                    NULL,
                                    _T("CELLWinApp"),
                                    _T("CELLWinApp"),
                                    WS_OVERLAPPEDWINDOW,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    width,
                                    height, 
                                    NULL, 
                                    NULL,
                                    _hInstance, 
                                    this    //! 这里注意，将当前类的指针作为参数，传递,参见 windowProc函数.
                                    );
       
        if( _hWnd == 0 )
        {
            return  false;
        }
        ShowWindow( _hWnd, SW_SHOW );
        UpdateWindow( _hWnd );
        return  true;
    }
    /**
    *   Windows消息过程处理函数
    */
    static  LRESULT CALLBACK    windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        /**
        *   使用this数据，将全局函数，转化为类的成员函数调用
        */
        CELLWinApp*  pThis   =   (CELLWinApp*)GetWindowLong(hWnd,GWL_USERDATA);
        if (pThis)
        {
            return  pThis->events(msg,wParam,lParam);
        }
        if (WM_CREATE == msg)
        {
            CREATESTRUCT*   pCreate =   (CREATESTRUCT*)lParam;
            SetWindowLong(hWnd,GWL_USERDATA,(DWORD_PTR)pCreate->lpCreateParams);
        }
        return  DefWindowProc( hWnd, msg, wParam, lParam );
    }
};

}
}