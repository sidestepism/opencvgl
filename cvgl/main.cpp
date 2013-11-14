#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/glut.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>


#define WINDOW_NAME "test1"

void init(void);
void glut_display(void);
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void draw_pyramid(void);
void draw_cube(void);
void draw_plane(void);
void set_texture(void);
void set_cam_texture(void);
void glut_idle(void);

#define TEXHEIGHT 512
#define TEXWIDTH 512
GLuint TextureHandle[4];

double Angle1 = 0;
double Angle2 = 0;
double Angle3 = 0;
double Angle4 = 0;
double Distance = 10.0;
bool LeftButtonOn = false;
bool RightButtonOn = false;

// OpenCV
cv::VideoCapture cap;
cv::Mat frame, camTexture;


int main(int argc, char *argv[])
{

    cap.open(0);
    cap >> frame;
    cv::resize(frame, camTexture, cv::Size(512, 512), 0, 0, cv::INTER_LINEAR);

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
    glutIdleFunc(glut_idle);
    glutMainLoop();
    return 0;
}

void init(){
    glClearColor(0., 0., 0., 0.);
    glGenTextures(4, TextureHandle);
    for (int i = 0; i < 4; i++) {
        glBindTexture(GL_TEXTURE_2D, TextureHandle[i]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXWIDTH, TEXHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    set_texture();
    set_cam_texture();
}

void glut_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'a':
            Angle3 += 0.03;
            break;
        case 's':
            Angle4 += 0.03;
            break;

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
    gluLookAt(Distance * cos(Angle2) * sin(Angle1),
              Distance * sin(Angle2),
              Distance * cos(Angle2) * cos(Angle1),
              0, 0, 0, 0, 1.0, 0);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    set_cam_texture();
    draw_pyramid();
    glPopMatrix();
    glFlush();

    glDisable(GL_DEPTH_TEST);
    glutSwapBuffers();
}

void set_cam_texture(){
    cap >> frame;
    cv::resize(frame, camTexture, cv::Size(512, 512), 0, 0, cv::INTER_LINEAR);
    glBindTexture(GL_TEXTURE_2D, TextureHandle[3]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, (TEXWIDTH - camTexture.cols) / 2, (TEXHEIGHT - camTexture.rows) / 2, camTexture.cols, camTexture.rows, GL_BGR, GL_UNSIGNED_BYTE, camTexture.data);
    printf("tex %d", camTexture.cols);

}

void set_texture(){
    for(int i = 0; i < 3; i++){
        char inputfile[256];
        switch (i) {
            case 0:
                sprintf(inputfile, "/Users/ryohei/gitrepos/cvgl/sushitex.jpg");
                break;
            case 1:
                sprintf(inputfile, "/Users/ryohei/gitrepos/cvgl/fruits.jpg");
                break;
            case 2:
                sprintf(inputfile, "/Users/ryohei/gitrepos/cvgl/lena.jpg");
                break;
        }
        cv::Mat input;
        input = cv::imread(inputfile, 1);
        glBindTexture(GL_TEXTURE_2D, TextureHandle[i]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, (TEXWIDTH - input.cols) / 2, (TEXHEIGHT - input.rows) / 2, input.cols, input.rows, GL_BGR, GL_UNSIGNED_BYTE, input.data);
    }
}

void glut_idle(){
    static int counter = 0;
    if(counter == 0){
        glBindTexture(GL_TEXTURE_2D, TextureHandle[0]);
    }else if(counter == 1000){
        glBindTexture(GL_TEXTURE_2D, TextureHandle[1]);
    }else if(counter == 2000){
        glBindTexture(GL_TEXTURE_2D, TextureHandle[2]);
    }
    counter ++;
    if(counter == 3000){
        counter = 0;
    }
    glutPostRedisplay();
}

void draw_pyramid(void){
    GLdouble pointO[] = {0.0, 1.0, 0.0};
    GLdouble pointA[] = {1.5, -1.0, 1.5};
    GLdouble pointB[] = {-1.5, -1.0, 1.5};
    GLdouble pointC[] = {-1.5, -1.0, -1.5};
    GLdouble pointD[] = {1.5, -1.0, -1.5};


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureHandle[3]);

//    glColor3d(1.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0.5, 0.0);
    glVertex3dv(pointO);
    glTexCoord2d(0.0, 1.0);
    glVertex3dv(pointD);
    glTexCoord2d(1.0, 1.0);
    glVertex3dv(pointA);
    glEnd();


    glBindTexture(GL_TEXTURE_2D, TextureHandle[3]);
//    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0.5, 0.0);
    glVertex3dv(pointO);
    glTexCoord2d(0.0, 1.0);
    glVertex3dv(pointA);
    glTexCoord2d(1.0, 1.0);
    glVertex3dv(pointB);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TextureHandle[3]);
//    glColor3d(1.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0.5, 0.0);
    glVertex3dv(pointO);
    glTexCoord2d(0.0, 1.0);
    glVertex3dv(pointB);
    glTexCoord2d(1.0, 1.0);
    glVertex3dv(pointC);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, TextureHandle[3]);
//    glColor3d(0.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0.5, 0.0);
    glVertex3dv(pointO);
    glTexCoord2d(0.0, 1.0);
    glVertex3dv(pointC);
    glTexCoord2d(1.0, 1.0);
    glVertex3dv(pointD);
    glEnd();


    glColor3d(1.0, 1.0, 1.0);

    glBegin(GL_POLYGON);

    glTexCoord2d(1.0, 0.0);
    glVertex3dv(pointA);
    glTexCoord2d(0.0, 0.0);
    glVertex3dv(pointB);
    glTexCoord2d(0.0, 1.0);
    glVertex3dv(pointC);
    glTexCoord2d(1.0, 1.0);
    glVertex3dv(pointD);
    glEnd();

    glDisable(GL_TEXTURE_2D);
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


void draw_plane(void){
    GLfloat facecolor[] = {0.9, 0.9, 0.9, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
    glNormal3d(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
    glVertex3d(4.0, 0.0, 4.0);
    glVertex3d(4.0, 0.0, -4.0);
    glVertex3d(-4.0, 0.0, -4.0);
    glVertex3d(-4.0, 0.0, 4.0);
    glEnd();
}
