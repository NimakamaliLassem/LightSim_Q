/*********
CREATOR : Nima Kamali Lassem
----------
PROBLEMS: None that I noticed
----------
ADDITIONAL FEATURES:
1.Menu screen
    {Play, Guide, and Exit button both by click and keys}
2.Guide Menu
3.Menu and End button in Game while playing
4.Choosable Torpeado Chasing Methode
    {
     1.You can choose between FOCUS (using vector, not very fast but focused) and ULTRA (using basic glut functions, Very fast but fuzzy)
     2.Displays the current method
     }
5.Mouse OpenGL location displayed on the screen
6.Torpeado fire using Queue ( The Best option in my opinion :) )
    {Please add the queue header I have added to the game files in case it can not be found automatically}

*********/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"
#include "queue_struct.h"


#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 680

#define TIMER_PERIOD  20 
#define TIMER_ON         1

#define D2R 0.0174532
bool up = false, down = false, right = false, left = false, Guide = false, Tv = true;;
int  winWidth, winHeight, timer = 0, clcnt = 0, ChaseM = 0, xM, yM;
char ChaseMn[2][10]{ "FOCUS", "ULTRA" };
queue_t Q;
int state = 1;
typedef struct {
    vec_t pos;
    float angle;
} player_t;

typedef struct {
    vec_t pos;
    vec_t vel;
    float angle;

} enemy_t;

player_t P = { {0, 0}, 45 };
enemy_t  E = { { 0, 200 } , {0,0}, 90 };
double angle = angleV(subV(P.pos, E.pos)) + E.angle;
typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    vec_t   pos;
    color_t color;
    vec_t   vel;
} light_t;

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;


light_t light = { {P.pos.x, P.pos.y}, {.6,.5,0},{2 / 3,1 / 3} };

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

double distanceImpact(double d) {
    return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}

void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}


void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}
void drawTorpeado(enemy_t E, queue_t Q)
{

    glColor3ub(255, 255, 0);
    circle(E.pos.x, E.pos.y, 10);
    glColor3ub(255, 100, 0);
    int i = 1;
    while (!isEmptyQ(&Q))
    {
        tail_t templ;
        templ = remove(&Q);
        circle(templ.x, templ.y, i);
        i++;
    }
    glColor3ub(255, 0, 0);
    circle(E.pos.x, E.pos.y, 5);

}
void drawMoon(player_t p) {

    glColor3ub(200, 0, 0);
    circle_wire(p.pos.x, p.pos.y, 51);
    circle(p.pos.x, p.pos.y, 50);
    glColor3ub(100, 0, 0);
    circle(p.pos.x + 10, p.pos.y + 20, 5);
    circle(p.pos.x + 10, p.pos.y + 20, 5);
    circle(p.pos.x + 30, p.pos.y + 10, 10);
    circle(p.pos.x - 10, p.pos.y - 30, 8);
    circle(p.pos.x - 30, p.pos.y + 20, 15);
    circle(p.pos.x + 20, p.pos.y - 30, 11);
}
void display() {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    //In Menu 
    if (state == 1)
    {
        glColor3ub(0, 20, 0);
        glRectf(-WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        glColor3ub(255, 255, 0);
        glRectf(-80, -20, 80, 20);
        glRectf(-80, -70, 80, -30);
        glRectf(-80, -120, 80, -80);
        print(-35, 50, "MENU", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3b(0, 0, 0);
        print(-60, -5, "F1 to PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-70, -60, "F2 for GUIDE", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-60, -110, "Esc to EXIT", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    //In Game
    else if (state == 2)
    {
        glColor3ub(255, 255, 255);
        circle(-50, 80, 1);
        circle(-80, 40, 1);
        circle(-90, 100, 1);
        circle(-100, -50, 1);
        circle(-270, 60, 1);
        circle(-500, 170, 1);
        circle(80, 110, 1);
        circle(190, 220, 1);
        circle(450, 70, 1);
        circle(210, -50, 1);
        circle(99, 300, 1);
        circle(55, -80, 1);
        circle(-180, 60, 1);
        circle(-76, 50, 1);
        circle(60, -80, 1);

        drawMoon(P);
        //Tree
        glColor3f(0, .05, 0);
        glRectd(515, -100, 545, -145);
        glBegin(GL_TRIANGLES);
        glVertex2d(450, -120);
        glVertex2d(610, -120);
        glVertex2d(530, -60);
        glVertex2d(480, -80);
        glVertex2d(580, -80);
        glVertex2d(530, -40);
        glEnd();
        for (int x = -WINDOW_WIDTH / 2; x <= WINDOW_WIDTH / 2; x++) {
            vertex_t P = { { x, -150 }, { 0, 1 } };

            color_t resolution = { 0.0, 0.1, 0.0 };
            resolution = addColor(resolution, calculateColor(light, P));

            glBegin(GL_LINES);
            glColor3f(resolution.r, resolution.g, resolution.b);
            glVertex2f(x, -WINDOW_HEIGHT / 2 + 150 + x / 10);

            glColor3f(0, 0, 0);
            glVertex2f(x, -WINDOW_HEIGHT / 2);
            glEnd();


        }

        //Menu
        glColor3f(1, .5, 0);
        glRectf(-WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, -WINDOW_WIDTH / 2 + 70, WINDOW_HEIGHT / 2 - 35);
        glColor3ub(255, 255, 255);
        vprint(-WINDOW_WIDTH / 2 + 7.5, WINDOW_HEIGHT / 2 - 25, GLUT_BITMAP_HELVETICA_18, "MENU");
        //Guide
        glColor3f(1, .5, 0);
        glRectf(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 35);
        glColor3ub(255, 255, 255);
        vprint(WINDOW_WIDTH / 2 - 55, WINDOW_HEIGHT / 2 - 25, GLUT_BITMAP_HELVETICA_18, "EXIT");
        //Chase Method name
        if (ChaseM == 0)
            vprint(-32.5, WINDOW_HEIGHT / 2 - 67.5, GLUT_BITMAP_HELVETICA_18, "%s", ChaseMn[ChaseM]);
        else
            vprint(-30, WINDOW_HEIGHT / 2 - 67.5, GLUT_BITMAP_HELVETICA_18, "%s", ChaseMn[ChaseM]);
        circle_wire(0, WINDOW_HEIGHT / 2 - 60, 50);
        vprint(-30, WINDOW_HEIGHT / 2 - 130, GLUT_BITMAP_HELVETICA_18, "%d,%d", xM, yM);

        if (clcnt >= 1)
        {
            drawTorpeado(E, Q);
        }

        //Aim
        glColor3ub(100, 255, 100);
        circle_wire(xM, yM, 15);
    }
    if (Guide)
    {

        glColor3f(1, 1, 0);
        glRectf(-205, -155, 205, 155);
        glColor3ub(100, 0, 100);
        glRectf(-200, -150, 200, 150);
        glColor3ub(255, 255, 0);
        print(-40, 120, "[Guide]", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-100, 60, "Welcome to the Game!", GLUT_BITMAP_HELVETICA_18);
        print(-105, 25, "Press F1 for Main Menu", GLUT_BITMAP_HELVETICA_18);
        print(-150, -10, "Click on screen to launch a Torpeado", GLUT_BITMAP_HELVETICA_18);
        print(-160, -50, "Press Space to change Chasing Method", GLUT_BITMAP_HELVETICA_18);
        print(-120, -90, "Press Esc to close the Game", GLUT_BITMAP_HELVETICA_18);

    }

    glutSwapBuffers();
}


void onKeyDown(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);


    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{

    if (key == 32)
        ChaseM = fabs(ChaseM - 1);


    glutPostRedisplay();
}


void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    glutPostRedisplay();
}



void onSpecialKeyUp(int key, int x, int y)
{

    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    case GLUT_KEY_F1:
        if (state == 1)state = 2;
        else state = 1; break;
    case GLUT_KEY_F2: Guide = !Guide; break;
    }


    glutPostRedisplay();
}


void onClick(int button, int stat, int x, int y)
{
    if (state == 2) {
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && (x <= WINDOW_WIDTH && x >= WINDOW_WIDTH - 70 && y >= 0 && y <= 35))
        {
            exit(0);
        }
        else if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && (x >= 0 && x <= 70 && y >= 0 && y <= 35))
        {
            state = 1;
            clcnt = 0;
        }
        else if ((x >= 560 && x <= 720 && y >= 320 && y <= 360))
        {
        }
        else {
            Tv = true;
            clcnt++;
            E.pos.x = x - WINDOW_WIDTH / 2;
            E.pos.y = WINDOW_HEIGHT / 2 - y;
        }
    }
    if (state == 1) {
        if (Guide && button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
            Guide = !Guide;
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && x >= 560 && x <= 720 && y >= 320 && y <= 360)
            state = 2;
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && x >= 560 && x <= 720 && y >= 370 && y <= 410)
            Guide = !Guide;
        if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && x >= 560 && x <= 720 && y >= 420 && y <= 460)
            exit(0);
    }


    glutPostRedisplay();
}



void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display();
}

void onMoveDown(int x, int y) {

    glutPostRedisplay();
}


void onMove(int x, int y) {
    xM = x - WINDOW_WIDTH / 2;
    yM = WINDOW_HEIGHT / 2 - y;
    printf("%d, %d\n", xM, yM);
    glutPostRedisplay();
}


void onTimer(int v) {
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    timer++;
    light = { {P.pos.x, P.pos.y}, {1,0,0},{3,2} };

    P.pos.x += 3;
    P.pos.y += .5;
    if (P.pos.x > WINDOW_WIDTH / 2 + 150)
    {
        P.pos.x = -WINDOW_WIDTH / 2 - 150;
        P.pos.y = -150;
    }
    light.pos = addV(light.pos, light.vel);
    if (ChaseM == 1)
    {
        if (E.pos.x < P.pos.x)
            E.pos.x += 5;
        else if (E.pos.x > P.pos.x)
            E.pos.x -= 5;
        else E.pos.x = P.pos.x;
        if (E.pos.y < P.pos.y)
            E.pos.y += 5;
        else if (E.pos.y > P.pos.y)
            E.pos.y -= 5;
        else E.pos.y = P.pos.y;
    }
    if (ChaseM == 0) {
        E.vel = mulV(3.5, unitV(subV(P.pos, E.pos)));
        vec_t prevPos = E.pos;
        if (clcnt >= 1) {
            E.pos = addV(E.pos, E.vel);
        }
    }
    if (fabs(P.pos.x - E.pos.x) + fabs(P.pos.y - E.pos.y) < 25)
    {
        clcnt = 0;

    }
    if (!isFullQ(&Q))
    {
        tail_t Temp;
        Temp.x = E.pos.x;
        Temp.y = E.pos.y;
        insert(&Q, Temp);
    }
    else
    {
        remove(&Q);
    }

    glutPostRedisplay();

}


void Init() {


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Follow the moon-Nima Kamali Lassem");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);


    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);


    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}