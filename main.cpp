#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include"shader.h"
#include<GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <string>
#include<vector>

using namespace std;
typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

//鼠标交互有关的
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;
//与实现角度大小相关的参数，只需要两个就可以完成
float xRotate = 0.0f;
float yRotate = 0.0f;
float ty = 0.0f;
float scale = 1;

//文件读取有关的
MyMesh mesh;
const string file = "/home/popo/CLionProjects/opengl_demo/sphere.off";

GLuint showFaceList;

vector<float> Positions;
vector<float> Normals;

unsigned int VAO,VBO;


//初始化顶点和面
void initGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(2.0);

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST); //用来开启深度缓冲区的功能，启动后OPengl就可以跟踪Z轴上的像素，那么它只有在前面没有东西的情况下才会绘制这个像素，在绘制3d时，最好启用，视觉效果会比较真实
    // ------------------- Lighting
    glEnable(GL_LIGHTING); // 如果enbale那么就使用当前的光照参数去推导顶点的颜色
    glEnable(GL_LIGHT0); //第一个光源，而GL_LIGHT1表示第二个光源
    // ------------------- Display List
    showFaceList = glGenLists(1);
    int temp = mesh.n_edges();

    // 绘制flat
    glNewList(showFaceList, GL_COMPILE);
    for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
        glBegin(GL_TRIANGLES);
        for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
            Positions.push_back(mesh.point(*fv_it).data()[0]);
            Positions.push_back(mesh.point(*fv_it).data()[1]);
            Positions.push_back(mesh.point(*fv_it).data()[2]);
            glNormal3fv(mesh.normal(*fv_it).data());
            glVertex3fv(mesh.point(*fv_it).data());
            Normals.push_back(mesh.normal(*fv_it).data()[0]);
            Normals.push_back(mesh.normal(*fv_it).data()[1]);
            Normals.push_back(mesh.normal(*fv_it).data()[2]);
            cout<<Positions.size()<<endl;
        }
        glEnd();
    }
    glEndList();
}

// 当窗体改变大小的时候
void myReshape(GLint w, GLint h)
{
    glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
    else
        glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// 读取文件的函数
void readfile(string file) {
    // 请求顶点法线 vertex normals
    mesh.request_vertex_normals();
    //如果不存在顶点法线，则报错
    if (!mesh.has_vertex_normals())
    {
        cout << "错误：标准定点属性 “法线”不存在" << endl;
        return;
    }
    // 如果有顶点发现则读取文件
    OpenMesh::IO::Options opt;
    if (!OpenMesh::IO::read_mesh(mesh, file, opt))
    {
        cout << "无法读取文件:" << file << endl;
        return;
    }
    else cout << "成功读取文件:" << file << endl;
    cout << endl; // 为了ui显示好看一些
    //如果不存在顶点法线，则计算出
    if (!opt.check(OpenMesh::IO::Options::VertexNormal))
    {
        // 通过面法线计算顶点法线
        mesh.request_face_normals();
        // mesh计算出顶点法线
        mesh.update_normals();
        // 释放面法线
        mesh.release_face_normals();
    }
}

//  键盘交互
void mySpecial(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            ty += 0.01;
            break;
        case GLUT_KEY_DOWN:
            ty -= 0.01;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

// 鼠标交互
void myMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mousetate = 1;
        Oldx = x;
        Oldy = y;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        mousetate = 0;
    //滚轮事件
    if (state == GLUT_UP && button == 3) {

        scale -= 0.1;
    }
    if (state == GLUT_UP && button == 4) {

        scale += 0.1;
    }
    glutPostRedisplay();
}

// 鼠标运动时
void onMouseMove(int x, int y) {
    if (mousetate) {
        //x对应y是因为对应的是法向量
        yRotate += x - Oldx;
        glutPostRedisplay();
        Oldx = x;
        xRotate += y - Oldy;
        glutPostRedisplay();
        Oldy = y;
    }
}

void myDisplay()
{
    //要清除之前的深度缓存
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //与显示相关的函数
    glRotatef(xRotate, 1.0f, 0.0f, 0.0f); // 让物体旋转的函数 第一个参数是角度大小，后面的参数是旋转的法向量
    glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, ty);
    glScalef(scale, scale, scale); // 缩放

    //每次display都要使用glcalllist回调函数显示想显示的顶点列表
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,Positions.size());
    glBindVertexArray(0);

    glFlush();

    glutSwapBuffers(); //这是Opengl中用于实现双缓存技术的一个重要函数
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double 表示使用双缓存而非单缓存
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("Mesh Viewer");
    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way
    //一开始默认读取文件1
    readfile(file);
    initGL();
    Shader myShader=Shader("/home/popo/Desktop/webgl/shader.vs","/home/popo/Desktop/webgl/shader.fg");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0])*Positions.size(), &Positions[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    //glVertexPointer(3,GL_FLOAT,0,0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    myShader.Use();

    glutDisplayFunc(&myDisplay);

    glutMainLoop();
    return 0;
}