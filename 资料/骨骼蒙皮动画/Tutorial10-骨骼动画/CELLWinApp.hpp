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
    *   Ӧ�ó���ʵ�����
    */
    HINSTANCE                   _hInstance;
    /**
    *   ���ھ������������ʹ��
    */
    HWND                        _hWnd;
    /**
    *   �豸�����ģ�û��Opengl����ͼ��ʹ��������
    */
    HDC                         _hDC;
    /**
    *   Opengl�����ľ����
    */
    HGLRC                       _hRC;
    /**
    *   ���ڵĿ�Ⱥ͸߶�
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
        *   Ҫ�봴��һ�����ڣ�����Ҫע��һ��������
        *   ����ڴ棬�����˽�windows��ɣ����ﲻ�����ܡ�
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
    *   ��Ⱦ����
    */
    virtual void    render()
    {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        SwapBuffers( _hDC );
    }

    /**
    *   ��ں���
    *   width :�������ڿ��
    *   height:�������ڵĸ߶�
    */
    int     start(int width,int height)
    {
        _winWidth   =   width;
        _winHeight  =   height;

        /**
        *   ��������
        */
        if (!_createWindow(_winWidth,_winHeight))
        {
            return  -1;
        }
        /**
        *   ��ʼ��OpenGL������
        */
        if (!initDevice())
        {
            return  -2;
        }
        onInit();
        /**
        *   ������Ϣѭ��
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
            *   ����Ϣ��������Ϣ������Ϣ���������Ⱦ����
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
    *   ��ʼ��OpenGL
    */
    bool    initDevice()
    {
        /**
        *   ����������ǳ�ʼ��Opengl�����Ķ�����
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
        *   ��ȡ��ǰ����dc
        */
        _hDC        =   GetDC( _hWnd );
        /**
        *   ѡ��һ����pfd�ӽ������ظ�ʽ��
        *   ��Ҳ���Ե���int iCnt = DescribePixelFormat(_hDC, pixelFormat,0)����ֵΪ���ж��ٸ����õ�
        *   pfd��Ȼ��ѭ���ĵ��ã��õ�����Ҫ�ġ�
        *   for( int i = 1 ;i < iCnt ; ++ i )
        *   {
        *       PIXELFORMATDESCRIPTOR pfd;
        *       DescribePixelFormat(_hDC, iCnt,&pfd);
        *   }
        */
        pixelFormat =   ChoosePixelFormat( _hDC, &pfd );
        DescribePixelFormat( _hDC, pixelFormat, sizeof(pfd), &pfd );
        /**
        *   ���õ�ǰdc��pfd;
        */
        if(!SetPixelFormat( _hDC, pixelFormat, &pfd ))
        {
            return  false;
        }
        /**
        *   ����Opengl������
        */
        _hRC        =   wglCreateContext( _hDC );
        /**
        *   �뵱ǰ��dc���а�
        *   ע��������ڶ��߳��н��л��ƣ���仰�ڻ���֮ǰ���ã������߳��л�ʹ��
        */
        if(!wglMakeCurrent( _hDC, _hRC ))
        {
            return  false;
        }
      
        RECT    rt;
        GetClientRect(_hWnd,&rt);
        /**
        *   ���ڷ����仯֪ͨ
        */
        _winWidth   =   rt.right - rt.left;
        _winHeight  =   rt.bottom - rt.top;
        SendMessage(_hWnd,WM_SIZE,0,0);
        return  true;
    }
    /**
    *   �ر�
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
    *   �¼�
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
    *   ����һ����ʼ��OpenGL�ĺ���,�ڶ���������
    *   ���øú�����ɶ�OpenGL�Ļ���״̬�ĳ�ʼ��
    *   �ڽ�����Ϣѭ��֮ǰ��һ��֪ͨ,ֻ����һ��
    */
    virtual void    onInit()
    {
        /**
        *   ��մ���Ϊ��ɫ
        */
        glClearColor(0,0,0,1);
        /**
        *   ����OpenGL�ӿڵ�λ�úʹ�С��
        */
        glViewport( 0, 0, (GLint) _winWidth, (GLint) _winHeight );
    }
protected:
    /**
    *   �������ں���
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
                                    this    //! ����ע�⣬����ǰ���ָ����Ϊ����������,�μ� windowProc����.
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
    *   Windows��Ϣ���̴�����
    */
    static  LRESULT CALLBACK    windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        /**
        *   ʹ��this���ݣ���ȫ�ֺ�����ת��Ϊ��ĳ�Ա��������
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