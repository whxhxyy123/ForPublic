

#include "CELLWinApp.hpp"
#include <assert.h>
#include <math.h>
#include "matrix4x4f.h"
#pragma comment(lib,"opengl32.lib")


float g_fSpinX_R  =   0.0f;
float g_fSpinY_R  =   0.0f;

struct Vertex
{
    //! 颜色
    float r, g, b, a;
    //! 位置
    float x, y, z;
    //! 影响度
    float weights[2];
    //! 矩阵的索引
    short matrixIndices[2];
    //! 影响整个定点的骨头个数
    short numBones;
};

Vertex g_quadVertices[12] =
{
    { 1.0f,1.0f,0.0f,1.0f,   -1.0f,0.0f,0.0f,  1.0f,0.0f,  0,0,  1 }, // 蓝色
    { 1.0f,1.0f,0.0f,1.0f,    1.0f,0.0f,0.0f,  1.0f,0.0f,  0,0,  1 },
    { 1.0f,1.0f,0.0f,1.0f,    1.0f,2.0f,0.0f,  0.5f,0.5f,  0,1,  2 },
    { 1.0f,1.0f,0.0f,1.0f,   -1.0f,2.0f,0.0f,  0.5f,0.5f,  0,1,  2 },

    { 0.0f,1.0f,0.0f,1.0f,   -1.0f,2.0f,0.0f,  0.5f,0.5f,  0,1,  2 }, // 绿色
    { 0.0f,1.0f,0.0f,1.0f,    1.0f,2.0f,0.0f,  0.5f,0.5f,  0,1,  2},
    { 0.0f,1.0f,0.0f,1.0f,    1.0f,4.0f,0.0f,  0.5f,0.5f,  0,1,  2 },
    { 0.0f,1.0f,0.0f,1.0f,   -1.0f,4.0f,0.0f,  0.5f,0.5f,  0,1,  2 },

    { 0.0f,0.0f,1.0f,1.0f,   -1.0f,4.0f,0.0f,  0.5f,0.5f,  0,1,  2 }, // 黄色
    { 0.0f,0.0f,1.0f,1.0f,    1.0f,4.0f,0.0f,  0.5f,0.5f,  0,1,  2 },
    { 0.0f,0.0f,1.0f,1.0f,    1.0f,6.0f,0.0f,  1.0f,0.0f,  1,0,  1 },
    { 0.0f,0.0f,1.0f,1.0f,   -1.0f,6.0f,0.0f,  1.0f,0.0f,  1,0,  1 }
};


float   arBone[]    =   
{
     0.0f, 0.0f, 0.0f,	
    -0.2f, 0.2f,-0.2f,
     0.2f, 0.2f,-0.2f,
     0.0f, 3.0f, 0.0f,
    -0.2f, 0.2f,-0.2f,
    -0.2f, 0.2f, 0.2f,
     0.0f, 0.0f, 0.0f,
     0.2f, 0.2f,-0.2f,
     0.2f, 0.2f, 0.2f,
     0.0f, 0.0f, 0.0f,
    -0.2f, 0.2f, 0.2f,
     0.0f, 3.0f, 0.0f,
     0.2f, 0.2f, 0.2f,
    -0.2f, 0.2f, 0.2f,
};


matrix4x4f g_boneMatrix[2];
matrix4x4f g_matrixToRenderBone[2];


inline vector3f operator * (const vector3f& v, const matrix4x4f& mat)
{
    return  vector3f
        (
        v.x*mat.v[0][0] + v.y*mat.v[1][0] + v.z*mat.v[2][0] + 1*mat.v[3][0],
        v.x*mat.v[0][1] + v.y*mat.v[1][1] + v.z*mat.v[2][1] + 1*mat.v[3][1],
        v.x*mat.v[0][2] + v.y*mat.v[1][2] + v.z*mat.v[2][2] + 1*mat.v[3][2]
    );
}

class   Tutorial10 :public  CELL::Graphy::CELLWinApp
{
public:
    Tutorial10(HINSTANCE hInstance)
        :CELL::Graphy::CELLWinApp(hInstance)
    {
        _lbtnDownFlag   =   false;
        _fSpinY         =   0;
        _fSpinX         =   0;
        _bMousing_R     =   0;
    }
    virtual void    render()
    {
        do 
        {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

 
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
            glTranslatef( 0.0f, 0.0f, -15 );

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);

            {
                g_boneMatrix[0].identity();
                g_matrixToRenderBone[0].identity();

                matrix4x4f rotationMatrixY;
                matrix4x4f rotationMatrixZ;
                matrix4x4f boneRotationMatrix;


                g_boneMatrix[1].identity();
                g_matrixToRenderBone[1].identity();

                matrix4x4f offsetMatrix_toBoneEnd;
                matrix4x4f offsetMatrix_backFromBoneEnd;

                offsetMatrix_toBoneEnd.translate_y( 3.0f );
                offsetMatrix_backFromBoneEnd.translate_y( -3.0f );

                rotationMatrixY.rotate_y( g_fSpinY_R);
                rotationMatrixZ.rotate_z(-g_fSpinX_R);
                boneRotationMatrix  =   rotationMatrixY * rotationMatrixZ;

                g_boneMatrix[1]           =   g_boneMatrix[0] * offsetMatrix_toBoneEnd * boneRotationMatrix;
                g_matrixToRenderBone[1]   =   g_boneMatrix[1];
                g_boneMatrix[1]           =   g_boneMatrix[1] * offsetMatrix_backFromBoneEnd;
            }
            /**
            *   绘制表皮,保存临时点数据
            */
            Vertex  calQuadVertices[12];
            memcpy(calQuadVertices,g_quadVertices,sizeof(g_quadVertices));
            for (int i = 0 ;i < 12 ; ++ i )
            {
                vector3f    vec(0,0,0);
                vector3f    vecSrc(g_quadVertices[i].x,g_quadVertices[i].y,g_quadVertices[i].z);
                for (int x = 0 ; x < calQuadVertices[i].numBones ; ++ x)
                {
                    //! 计算位置
                    vector3f    temp    =   vecSrc* g_boneMatrix[g_quadVertices[i].matrixIndices[x]];
                    //! 计算权重位置
                    vec  += temp * g_quadVertices[i].weights[x];
                }
                calQuadVertices[i].x    =   vec.x;
                calQuadVertices[i].y    =   vec.y;
                calQuadVertices[i].z    =   vec.z;
            }
            glColorPointer(4,GL_FLOAT,sizeof(Vertex),calQuadVertices);
            glVertexPointer(3,GL_FLOAT,sizeof(Vertex),((float*)calQuadVertices) + 4);
            for (int i = 0 ;i < 3 ; ++ i )
            {
                glDrawArrays(GL_LINE_LOOP,i * 4,4);
            }
            glDisableClientState(GL_COLOR_ARRAY);

            /**
            *   绘制骨头
            */
            glVertexPointer(3,GL_FLOAT,0,arBone);
            glPushMatrix();
            {
                //! 绿色骨头
                glMultMatrixf( g_matrixToRenderBone[0].m );
                glColor3f( 1.0f, 1.0f, 0.0 ); 
                glDrawArrays(GL_LINE_STRIP,0,sizeof(arBone)/12);
            }
            glPopMatrix();

            glPushMatrix();
            {
                //! 蓝色骨头
                glMultMatrixf( g_matrixToRenderBone[1].m );
                glColor3f( 0.0f, 0.0f, 1.0 );
                glDrawArrays(GL_LINE_STRIP,0,sizeof(arBone)/12);
            }
            glPopMatrix();

            SwapBuffers( _hDC );
        } while (false);
    }

    /**
    *   生成投影矩阵
    *   后面为了重用性，我们会写一个专门的matrix类，完成矩阵的一系列擦做
    *   这个是很有必须要的，当你对Opengl了解的不断深入，你会发现，很多都是和数学有关的
    */
    void    perspective(float fovy,float aspect,float zNear,float zFar,float matrix[4][4])
    {
        assert(aspect != float(0));
        assert(zFar != zNear);
        #define PI 3.14159265358979323f

            float rad       =   fovy * (PI / 180);
           

            float halfFovy  =   tan(rad / float(2));
            matrix[0][0]    =   float(1) / (aspect * halfFovy);
            matrix[1][1]    =   float(1) / (halfFovy);
            matrix[2][2]    =   -(zFar + zNear) / (zFar - zNear);
            matrix[2][3]    =   -float(1);
            matrix[3][2]    =   -(float(2) * zFar * zNear) / (zFar - zNear);
        #undef PI
    }
    virtual void    onInit()
    {
        /**
        *   调用父类的函数。
        */
        CELL::Graphy::CELLWinApp::onInit();
        glMatrixMode( GL_PROJECTION );
        GLfloat matrix[4][4]  =   
        {
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            0,0,0,0
        };
        perspective(45.0f, (GLfloat)_winWidth / (GLfloat)_winHeight, 0.1f, 100.0f,matrix);
        glLoadMatrixf((float*)matrix);
        glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
        
    }

    virtual int    events(unsigned msg, unsigned wParam, unsigned lParam)
    {
        switch(msg)
        {
        case WM_LBUTTONDOWN:
            {
                _mousePos.x     =   LOWORD (lParam);
                _mousePos.y     =   HIWORD (lParam);
                _lbtnDownFlag   =   true;
                SetCapture(_hWnd);
            }
            break;
        case WM_LBUTTONUP:
            {
                _lbtnDownFlag   =   false;
                ReleaseCapture();
            }
            break;
        case WM_RBUTTONDOWN:
            {
                _ptLastMousePosit_R.x = _ptCurrentMousePosit_R.x = LOWORD (lParam);
                _ptLastMousePosit_R.y = _ptCurrentMousePosit_R.y = HIWORD (lParam);
                _bMousing_R = true;
            }
            break;
        case WM_RBUTTONUP:
            {
                _bMousing_R = false;
            }
            break;
        case WM_MOUSEMOVE:
            {
                int curX    = LOWORD (lParam);
                int curY    = HIWORD (lParam);

                if( _lbtnDownFlag )
                {
                    _fSpinX -= (curX - _mousePos.x);
                    _fSpinY -= (curY - _mousePos.y);
                }

                _mousePos.x = curX;
                _mousePos.y = curY;

                _ptCurrentMousePosit_R.x = LOWORD (lParam);
                _ptCurrentMousePosit_R.y = HIWORD (lParam);


                if( _bMousing_R )
                {
                    g_fSpinX_R -= (_ptCurrentMousePosit_R.x - _ptLastMousePosit_R.x);
                    g_fSpinY_R -= (_ptCurrentMousePosit_R.y - _ptLastMousePosit_R.y);
                }
                _ptLastMousePosit_R.x = _ptCurrentMousePosit_R.x;
                _ptLastMousePosit_R.y = _ptCurrentMousePosit_R.y;

            }
            break;
        }
        return  __super::events(msg,wParam,lParam);
    }
protected:
    unsigned    _primitiveType;
    /**
    *   保存纹理Id
    */
    unsigned    _textureId;

    float       _fSpinX ;
    float       _fSpinY;
    POINT       _mousePos;
    bool        _lbtnDownFlag;

    POINT       _ptLastMousePosit_R;
    POINT       _ptCurrentMousePosit_R;
    bool        _bMousing_R;
};

int CALLBACK _tWinMain(
                       HINSTANCE hInstance, 
                       HINSTANCE hPrevInstance, 
                       LPTSTR lpCmdLine, 
                       int nShowCmd 
                       )
{
    (void*)hInstance;
    (void*)hPrevInstance;
    (void*)lpCmdLine;
    (void*)nShowCmd;

    Tutorial10 winApp(hInstance);
    winApp.start(640,480);
    return  0;
}