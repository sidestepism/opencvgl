#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

#define WINDOW_NAME "test1"

void init(void);
void glut_display(void);
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void draw_pyramid(void);
void draw_cube(void);

double Angle1 = 0;
double Angle2 = 0;
double Distance = 10.0;
bool LeftButtonOn = false;
bool RightButtonOn = false;

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutCreateWindow(WINDOW_NAME);
    init();

    glutDisplayFunc(glut_display);
    glutKeyboardFunc(glut_keyboard);
    glutMouseFunc(glut_mouse);
    glutMotionFunc(glut_motion);
    glutPassiveMotionFunc(glut_motion);
    glutMainLoop();
    return 0;
}

void init(){
    glClearColor(0., 0., 0., 0.);
}

void glut_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'q':
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void glut_mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_UP){
            LeftButtonOn = false;
        }else if(state == GLUT_DOWN){
            LeftButtonOn = true;
        }
    }

    if(button == GLUT_RIGHT_BUTTON){
        if(state == GLUT_UP){
            RightButtonOn = false;
            printf("rightbuttonup");
        }else if(state == GLUT_DOWN){
            RightButtonOn = true;
            printf("rightbuttondown");
        }
    }

}
void glut_motion(int x, int y){
    static int px = -1, py = -1;
    if(LeftButtonOn){
        if(px >= 0 && py >= 0){
            Angle1 += (double)-(x - px) / 200;
            Angle2 += (double)(y - py) / 200;
        }
        px = x;
        py = y;
    }else if(RightButtonOn){
        if(px >= 0 && py >= 0){
            Distance += (double)(y - py) / 100;
        }
        printf("%fl %fl", (double)-(x - px) / 200, (double)-(y - py) / 200);
        px = x;
        py = y;
    }else{
        px = -1;
        py = -1;
    }
    glutPostRedisplay();
}

void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,
              0,
              1.0 * Distance,
              0, 0, 0, 0, 1.0, 0);
    glRotatef(Angle2 / M_PI * 180, 0.0, 0.0, 1.0);
    glRotatef(Angle1 / M_PI * 180, sin(Angle2), cos(Angle2), 0.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
//    draw_pyramid();
    draw_cube();
    glFlush();
    glDisable(GL_DEPTH_TEST);
    glutSwapBuffers();
}

void draw_pyramid(void){
    GLdouble pointO[] = {0.0, 1.0, 0.0};
    GLdouble pointA[] = {1.5, -1.0, 1.5};
    GLdouble pointB[] = {-1.5, -1.0, 1.5};
    GLdouble pointC[] = {-1.5, -1.0, -1.5};
    GLdouble pointD[] = {1.5, -1.0, -1.5};

    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glEnd();

    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glEnd();

    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();

    glColor3d(1.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3dv(pointO);
    glVertex3dv(pointD);
    glVertex3dv(pointA);
    glEnd();

    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();
}

void draw_cube(void){
    GLdouble pointA[] = {-1.0, -1.0, -1.0};
    GLdouble pointB[] = {1.0, -1.0, -1.0};
    GLdouble pointC[] = {1.0, 1.0, -1.0};
    GLdouble pointD[] = {-1.0, 1.0, -1.0};
    GLdouble pointE[] = {-1.0, -1.0, 1.0};
    GLdouble pointF[] = {1.0, -1.0, 1.0};
    GLdouble pointG[] = {1.0, 1.0, 1.0};
    GLdouble pointH[] = {-1.0, 1.0, 1.0};

    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glEnd();

    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointE);
    glVertex3dv(pointF);
    glVertex3dv(pointG);
    glVertex3dv(pointH);
    glEnd();

    glColor3d(0.0, 0.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointA);
    glVertex3dv(pointB);
    glVertex3dv(pointF);
    glVertex3dv(pointE);
    glEnd();

    glColor3d(0.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointB);
    glVertex3dv(pointC);
    glVertex3dv(pointG);
    glVertex3dv(pointF);
    glEnd();

    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointC);
    glVertex3dv(pointD);
    glVertex3dv(pointH);
    glVertex3dv(pointG);
    glEnd();

    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3dv(pointD);
    glVertex3dv(pointA);
    glVertex3dv(pointE);
    glVertex3dv(pointH);
    glEnd();
}