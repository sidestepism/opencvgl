#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

#define WINDOW_NAME "test1"
int display_mode = 2;
void init(void);
void glut_display(void);
void glut_keyboard(unsigned char key, int x, int y);

void draw_square1(void);
void draw_square2(void);
void draw_square3(void);

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(500, 500);
    glutCreateWindow(WINDOW_NAME);

    init();
    glutDisplayFunc(glut_display);
    glutKeyboardFunc(glut_keyboard);
    glutMainLoop();
    return 0;
}

void init(){
    glClearColor(0., 0., 0., 0.);
}

void glut_keyboard(unsigned char key, int x, int y){
    switch(key){
            case 'q':
            case 'Q':
            case '\033':
            exit(0);
            case '1':
            display_mode = 1;
            break;
            case '2':
            display_mode = 2;
            case '3':
            display_mode = 3;
            break;
    }
    glutPostRedisplay();
}

void glut_display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    switch(display_mode){
        case 1:
            draw_square1();
            break;
        case 2:
            draw_square2();
            break;
        case 3:
            draw_square3();
            break;
    }
    glFlush();
}

void draw_square1(){
    glBegin(GL_LINE_LOOP);
    glColor3d(1.0, 0.0, 0.0);
    glVertex2d(0.9, 0.9);
    glVertex2d(0.9, -0.9);
    glVertex2d(-0.9, -0.9);
    glVertex2d(-0.9, 0.9);
    glEnd();
}

void draw_square2(){
    glBegin(GL_POLYGON);
    glColor3d(1.0, 0.0, 0.0);
    glVertex2d(0.9, 0.9);
    glVertex2d(0.9, -0.9);
    glVertex2d(-0.9, -0.9);
    glVertex2d(-0.9, 0.9);
    glEnd();
}

void draw_square3(){
    glBegin(GL_POLYGON);
    glColor3d(1.0, 0.0, 0.0);
    glVertex2d(0.9, 0.9);
    glColor3d(1.0, 1.0, 0.0);
    glVertex2d(0.9, -0.9);
    glColor3d(0.0, 1.0, 1.0);
    glVertex2d(-0.9, -0.9);
    glColor3d(0.0, 0.0, 0.0);
    glVertex2d(-0.9, 0.9);
    glEnd();
}