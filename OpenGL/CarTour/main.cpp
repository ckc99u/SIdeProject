
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "CSCIx229.h"
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#define Vex glVertex3f

int  new_mouse = 0, mouse_rot = 1, mouse_trn = 0;

float turn = 50.0, tipp = 30.0, twst = 0.0, x_click = 0, y_click = 0, xtm = 0.0, ytm = 0.0,
scl = 1.0, xcr = 0.0;
int rSeg = 16,  cSeg = 16;
double r = 0.07, c = 0.15;
const double PI = 3.1415926535897932384;
const double TAU = 2 * PI;
float zh =180.0;
int distance = 15;//light distance
int previous = -1;

int tp = -1;
float humanpos = 0.0;
float humanpos2 = 25.0;
unsigned int texname[13];
float smooth  = 1.0;  // Smooth/Flat shading
float no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
float mat_ambient[] = { 0.4, 0.23, 0.5, 1.0 };
float mat_ambient_color[] = { 0.5, 0.4, 0.5, 1.0 };
float mat_diffuse[] = { 0.3, 0.15, 0.45, 1.0 };
float mat_specular[] = { 0.45, 0.35, 0.5, 1.0 };
float no_shininess[] = { 0.0 };
float low_shininess[] = { 5.0 };
float high_shininess[] = { 100.0 };
float mat_emission[] = {0.3, 0.5, 0.2, 1.0};
float shiny   =   1.0;
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
GLfloat lit1_spc[] = {0.0, 0.0,  0.0, 1.00},
lit1_dfs[] = {1, 1,  1, 1.00},
lit1_amb[] = {0.3, 0.3,  0.3, 1.00},
lit1_pos[] = {0.20, 2.50,  5.20, 1.00};



/*static void Vertex(double th,double ph)
{
    double x = sin(th)*cos(ph);
    double y = cos(th)*cos(ph);
    double z =         sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}*/

void Keybord (unsigned char ch, int x, int y)
{
    //  printf ("   Keyboard -> '%c' pressed.\n", ch);
    
    switch (ch)  {

       // case 'r' :  mouse_trn = 0;
        //    mouse_rot = 1;            break;
        //case 't' :  mouse_trn = 1;
         //   mouse_rot = 0;            break;
        case 'z' :  turn = 0.0; tipp = 0.0;
            xtm  = 0.0; ytm  = 0.0;
            scl  = 1.0;               break;
        case '-' :  scl *= 0.92;              break;
        case '+' :  scl *= 1.06;              break;
        case 'w' :  xcr += 2.00;              break;
        case 's' :  xcr -= 2.00;              break;
            
        case  27 :  exit (0);                 break;
    }
    
    glutPostRedisplay();
}

//------------------------------------------   Mouse   ---------------------------------------------

void Mouse (int button, int state, int x, int y)
{
    x_click = x;
    y_click = y;
    
    new_mouse = 1;
}


//------------------------------------------   Motion   --------------------------------------------

void Motion (int x, int y)
{
    float turn_del, tipp_del, x_del, y_del;
    
    static int x_sav, y_sav;
    // static float turn_save = 0.0, tipp_save = 0.0;
    
    // printf ("   Motion:  %5d ->   x = %5d, y = %5d.\n", ++c, x, y);
    
    if (new_mouse)  {
        new_mouse = 0;
        x_sav = x_click;
        y_sav = y_click;
    }
    
    if (mouse_rot)  {
        
        turn_del = x - x_sav;
        turn += turn_del;
        
        tipp_del = y - y_sav;
        tipp += tipp_del;
        
    }  else if (mouse_trn)  {
        
        x_del = float(x - x_sav) * 0.01;
        xtm  += x_del;
        
        y_del = float(y - y_sav) * 0.01;
        ytm  -= y_del;
        
    }
    
    x_sav = x;
    y_sav = y;
    
    glutPostRedisplay();
}

//---------------------------------------   Init_Lights   ------------------------------------------

void Init_Lights (void)
{

    
    glEnable (GL_AUTO_NORMAL);
     glEnable (GL_NORMALIZE );
}
void draw_cone(int radius, float height, int opt)
{
    if(opt!= -1){
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[opt]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    glBegin(GL_TRIANGLE_FAN);
    
    glTexCoord2f(1,0.0);
    glVertex3f(0, 0, height);
    for (int angle = 0; angle < 360; angle++) {
        glNormal3f(-sin(angle)+sin(angle+5), -cos(angle)+cos(angle+5), cos(angle)*sin(angle+5)-sin(angle)*cos(angle+5));
        glTexCoord2f(0.5,1);
        glVertex3f(sin(angle) * radius, cos(angle) * radius, 0);
    }
    glEnd();
    
    // draw the base of the cone
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int angle = 0; angle < 360; angle++) {
        // normal is just pointing down
        
        glVertex3f(sin(angle) * radius, cos(angle) * radius, 0);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    }
    else{

        glBegin(GL_TRIANGLE_FAN);

        Vex(0, 0, height);
        for (int angle = 0; angle < 360; angle++) {
glNormal3f(-sin(angle)+sin(angle+5), -cos(angle)+cos(angle+5), cos(angle)*sin(angle+5)-sin(angle)*cos(angle+5));
            Vex(sin(angle) * radius, cos(angle) * radius, 0);
        }
        glEnd();
        
    
    }
}
void draw_ball(double r, int lats, int longs, int opt)
{
    
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    if(opt!=-1){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[opt]);
    double s,t;
    int i, j;

    for(i = 0; i <= lats; i++) {

        double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
        double z0  = sin(lat0);
        double zr0 =  cos(lat0);
        
        double lat1 = M_PI * (-0.5 + (double) i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
          s = lat0/(M_PI);
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
          
            
            double lng = 2 * M_PI * (double) (j - 1) / longs;
            t= 1 - lng/M_PI;
            double x = cos(lng);
            double y = sin(lng);
            
            glNormal3f(0.1*x * zr0, y * zr0, z0);
            glTexCoord2f(s,t);
            Vex(0.1*x * zr0, 0.1*y * zr0, z0);
            glNormal3f(0.1*x * zr1, 0.1*y * zr1, z1);
            s=lat1/(M_PI);
            
            glTexCoord2f(s,t);
            Vex(0.1*x * zr1, 0.1*y * zr1, z1);
            
        }
        glEnd();
        
    }glDisable(GL_TEXTURE_2D);
        
    }
    else{
        double s,t;
        int i, j;
        
        for(i = 0; i <= lats; i++) {
            
            double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
            double z0  = sin(lat0);
            double zr0 =  cos(lat0);
            
            double lat1 = M_PI * (-0.5 + (double) i / lats);
            double z1 = sin(lat1);
            double zr1 = cos(lat1);
            s = lat0/(M_PI);
            glBegin(GL_QUAD_STRIP);
            for(j = 0; j <= longs; j++) {
                
                
                double lng = 2 * M_PI * (double) (j - 1) / longs;
                t= 1 - lng/M_PI;
                double x = cos(lng);
                double y = sin(lng);
                
                glNormal3f(0.1*x * zr0, y * zr0, z0);
                glTexCoord2f(s,t);
                Vex(0.1*x * zr0, 0.1*y * zr0, z0);
                glNormal3f(0.1*x * zr1, 0.1*y * zr1, z1);
                s=lat1/(M_PI);
                
                glTexCoord2f(s,t);
                Vex(0.1*x * zr1, 0.1*y * zr1, z1);
                
            }
            glEnd();
            
        }
    }
    
}
void Draw_snow(){
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glScaled(45, 42, 10);
    draw_ball(10, 10, 7, 3);
    glPopMatrix();
}
//draw_Pyramids
void Draw_pyramid(int opt)
{
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[opt]);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    glBegin( GL_TRIANGLES );
   
    glNormal3f(0, -0.447, 0.894);
    glTexCoord2f(0,0);Vex( 0.0f, 1.f, 0.0f );
    glTexCoord2f(0,1);Vex( -1.0f, -1.0f, 1.0f );
    glTexCoord2f(0.5,1);Vex( 1.0f, -1.0f, 1.0f);
    glNormal3f(0.872, 0.218, 0.436);
    glTexCoord2f(0,0);Vex( 0.0f, 1.0f, 0.0f);
    glTexCoord2f(0,1);Vex( -1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.5,1);Vex( 0.0f, -1.0f, -1.0f);
    glNormal3f(-0.872, 0.218, 0.436);
    glTexCoord2f(0,0);Vex( 0.0f, 1.0f, 0.0f);
    glTexCoord2f(0,1);Vex( 0.0f, -1.0f, -1.0f);
    glTexCoord2f(0.5,1);Vex( 1.0f, -1.0f, 1.0f);
    
    glNormal3f(0, 1, 0);
    glTexCoord2f(0,0);Vex( -1.0f, -1.0f, 1.0f);
    glTexCoord2f(0,1);Vex( 0.0f, -1.0f, -1.0f);
   glTexCoord2f(0.5,1); Vex( 1.0f, -1.0f, 1.0f);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);

}
void Draw_snowMan(void){
    //nose
    glColor3f(1, 0, 0);
    glPushMatrix();
    glScaled(0.5, 0.5, 0.5);
    draw_cone(5, 10, -1);
    glPopMatrix();
    //body
    glColor3f(1, 1, 1);
    for(int i = 0 ; i < 2; i++){
    glPushMatrix();
    glScaled(100+i*50, 100+i*50, 10+i*5);
    glTranslated(0, -i*0.15, -1);
    draw_ball(10, 50, 50, -1);
    glPopMatrix();
        
    }
    for(int i = 0 ; i < 2; i++ )
    {
    
        glColor3f(0.0, 0.0, 0.0);
        //eyes
        
        glPushMatrix();
        glScaled(10, 10, 1);
        glTranslated(0.5-i*1, 0.1, 0);
        draw_ball(10, 50, 50, -1);
        glPopMatrix();
    
    }
}
//----------------------------------------   Draw_Road   -------------------------------------------
void Draw_cube(int opt)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[opt]);
    //texture color replace original one


//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    
    glBegin(GL_QUADS);
    
    glNormal3f(0.0f,1.0f,0.0f);//crossproduct find normal.
    glTexCoord2f(0,0);Vex( 1.0f, 1.0f, -1.0f);
    glTexCoord2f(1,0);Vex(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1,1);Vex(-1.0f, 1.0f,  1.0f);
    glTexCoord2f(0,1);Vex( 1.0f, 1.0f,  1.0f);
    
    // Bottom face (y = -1.0f)
    
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0,0);Vex( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1,0);Vex(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1,1);Vex(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0,1);Vex( 1.0f, -1.0f, -1.0f);
    
    // Front face  (z = 1.0f)
    
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0,0);Vex( 1.0f,  1.0f, 1.0f);
    glTexCoord2f(1,0);Vex(-1.0f,  1.0f, 1.0f);
    glTexCoord2f(1,1);Vex(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(0,1);Vex( 1.0f, -1.0f, 1.0f);
    
    // Back face (z = -1.0f)
    
    glNormal3f(0, 0, -1);
    glTexCoord2f(0,0);Vex( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1,0);Vex(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1,1);Vex(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0,1);Vex( 1.0f,  1.0f, -1.0f);
    
    // Left face (x = -1.0f)
    
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);Vex(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1,0);Vex(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(1,1);Vex(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0,1);Vex(-1.0f, -1.0f,  1.0f);
    
    // Right face (x = 1.0f)
    
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);Vex(1.0f,  1.0f, -1.0f);
    glTexCoord2f(1,0);Vex(1.0f,  1.0f,  1.0f);
    glTexCoord2f(1,1);Vex(1.0f, -1.0f,  1.0f);
    glTexCoord2f(0,1);Vex(1.0f, -1.0f, -1.0f);
    glEnd();  // End of drawing
    glDisable(GL_TEXTURE_2D);
}
void Draw_tube(float radius, float height){
    GLfloat x              = 0.0;
    GLfloat y              = 0.0;
    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    /** Draw the tube */
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while( angle < TAU ) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glNormal3f(x/radius, y/radius, 0.0);

        Vex(x, y , height);

        Vex(x, y , 0.0);
        angle = angle + angle_stepsize;
    }
    
    glNormal3f(x/radius, y/radius, 0.0);

    Vex(radius, 0.0, height);

    Vex(radius, 0.0, 0.0);
    glEnd();

    
    /** Draw the circle on top of cylinder */
    glBegin(GL_POLYGON);
    angle = 0.0;
    while( angle < TAU ) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        Vex(x, y , height);
        angle = angle + angle_stepsize;
    }
    Vex(radius, 0.0, height);
    glEnd();
    
}
void Draw_tunnel(float radius, float height, int opt)
{

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[opt]);
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

    
    glColor3f(1.0, 1.0, 1.0);
    /** Draw the tube */
    double majorStep = height / 50;
    double minorStep = 2.0 * M_PI / 20;
    int i, j;
    
    for (i = 0; i < 50; ++i) {
        GLfloat z0 = 0.5 * height - i * majorStep;
        GLfloat z1 = z0 - majorStep;
        
        glBegin(GL_TRIANGLE_STRIP);
        for (j = 0; j <= 20; ++j) {
            double a = j * minorStep;
            GLfloat x = radius * cos(a);
            GLfloat y = radius * sin(a);
            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat) 20, i / (GLfloat) 50);
            glVertex3f(x, y, z0);
            
            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat) 20, (i + 1) / (GLfloat) 50);
            glVertex3f(x, y, z1);
        }
        glEnd();
       
    } glDisable(GL_TEXTURE_2D);


}
void Draw_tree(void){
    glPushMatrix();
    glColor3f(0, 0.5, 0.0);
    glTranslated(2, 25, 0);
    glRotated(90, 1, 0, 0);
    glScaled(200, 120, 17);
    draw_ball(10, 10, 4, 2);
    
    glPopMatrix();
    
    glPushMatrix();
    
    glTranslated(0, -2, 0);
    glScaled(1, 7, 1);
    glRotated(90, 1, 0, 0);
    glColor3f(1.0, 1.0, 1.0);
    Draw_tunnel(3, 10,9);
    glPopMatrix();
    
}
void Draw_tree2(void){

    glPushMatrix();
    glColor3f(0.01, 0.8, 0.3);
    glTranslated(2, 54, 0);
    glRotated(90, 1, 0, 0);
    glScaled(200, 120, 17);
    draw_ball(8, 8, 8,2);
    glPopMatrix();
    for(int i = 0 ; i < 5;i++){
        if (i != 2){
    glPushMatrix();
    glColor3f(0.95, 0.1, 0.02);
        float x = 17*sinf(30*i);
        float z = 15*cosf(30*i);
    glTranslated(-2+x, 54+i, -5+z);
    glRotated(10, 0, 1, 0);
    glScaled(20, 12, 1.7);
    draw_ball(8, 8, 8,-1);
    glPopMatrix();}
    }
    
    glTranslated(0, 30, 0);
    glScaled(1, 7, 1);
    glRotated(90, 1, 0, 0);
    glColor3f(1.0, 1.0, 1.0);
    Draw_tunnel(3, 10,1);
    glPopMatrix();
    
}
void Draw_human(void){
    glPushMatrix();
    //head
    glScaled(80, 100, 5);
    glRotated(15, 0, 0, 1);
    glColor3f(0.7, 0.3, 0.6);
    draw_ball(15, 15, 5, -1);
    glPopMatrix();
    //body
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glRotated(90, 1, 0, 0);
    glTranslated(0, 0, 17);
    glScaled(5, 3, 9);
    Draw_cube(10);
    glPopMatrix();
    //legs&&hand
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(0.2, 0.2, 1);
    glTranslated(15, 0, 20);
    glColor3f(0.4, 0.8, 0.8);
    Draw_tube(5,20);
    glPopMatrix();
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(0.2, 0.2, 1);
    glTranslated(-15, 0, 20);
    glColor3f(0.4, 0.8, 0.8);
    Draw_tube(5,20);
    glPopMatrix();
    glPushMatrix();
    
    glRotated(-90, 0, 1, 0);
    glRotated(-120, 1, 0, 0);
    
    glScaled(0.2, 0.2, 1);
    glTranslated(-0, 3, -28);
    glColor3f(0.4, 0.8, 0.8);
    Draw_tube(5,17);
    glPopMatrix();
    glPushMatrix();
    
    glRotated(-90, 0, 1, 0);
    glRotated(-70, 1, 0, 0);
    
    glScaled(0.2, 0.2, 1);
    glTranslated(-0, 3, -28);
    glColor3f(0.4, 0.8, 0.8);
    Draw_tube(5,17);
    glPopMatrix();
}
void Draw_Road (void)
{
    glColor3f (0.45, 0.5, 0.45);
    glEnable (GL_LIGHTING);
    
    glPushMatrix();
    glTranslated(-950, -23.5, 0);
    glScalef(1000.0, 2,20);
    Draw_cube(5);
    
    glPopMatrix();
}
void Draw_ChristmasTree(void)
{
    for(int i = 0 ; i < 3; i++){
        glPushMatrix();
        glColor3f(0.276, 1, 0.1);
        glScaled(20, 20, 20);
        glTranslated(0, 7 - i*1.5, 0);
        glRotated(30*i, 0, 1, 0);
        Draw_pyramid(2);
        glPopMatrix();
    }
    glPushMatrix();
    glScaled(1, 7, 1);
    glRotated(90, 1, 0, 0);
    glTranslated(0, 0, -4);
    glColor3f(1.0, 1.0, 1.0);
    Draw_tunnel(3, 10,1);
    glPopMatrix();
    
    
}
void Draw_land(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_LIGHTING);
    
    glPushMatrix();
    glScaled(100, 2,500);
    Draw_cube(2);
    glPopMatrix();
    
    
}
//---------------------------------------   Render_Wheel   -----------------------------------------

void Render_Wheel (int xt, int yt, int zt, int scale)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);
    glPushMatrix();
    glTranslatef (xt, yt, zt);
    glScaled(scale, scale, scale);
    
    for (int i = 0; i < rSeg; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= cSeg; j++) {
            for (int k = 0; k <= 1; k++) {
                double s = (i + k) % rSeg + 0.5;
                double t = j % (cSeg + 1);
                
                double x = (c + r * cos(s * TAU / rSeg)) * cos(t * TAU / cSeg);
                double y = (c + r * cos(s * TAU / rSeg)) * sin(t * TAU / cSeg);
                double z = r * sin(s * TAU / rSeg);
                
                // double u = (i + k) / (float) rSeg;
                //double v = t / (float) cSeg;
                
                glNormal3f(2 * x, 2 * y, 2 * z);
                Vex(2 * x, 2 * y, 2 * z);
            }
        }
        glEnd();
    }
    
    glPopMatrix();
}

//-----------------------------------------   Draw_Car   -------------------------------------------

void Draw_Car (void)
{
    glDisable (GL_LIGHTING);
    // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, no_mat);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);
    glBegin (GL_QUADS);
    glColor3f (0.3, 0.6, 0.6);
    Vex(-40,  0, 15); Vex( 40,  0, 15); Vex( 40,-15, 15); Vex(-40,-15, 15);  // left
    Vex(-40,  0,-15); Vex( 40,  0,-15); Vex( 40,-15,-15); Vex(-40,-15,-15);  // right
    glColor3f (0.4, 0.6, 0.6);
    Vex( 40,  0,-15); Vex( 40,  0, 15); Vex( 40,-15, 15); Vex( 40,-15,-15);  // back
    Vex(-40,  0,-15); Vex(-40,  0, 15); Vex(-40,-15, 15); Vex(-40,-15,-15);  // front
    glColor3f (0.4, 0.6, 0.8);
    Vex(-40,  0, 15); Vex(-40,  0,-15); Vex( 40,  0,-15); Vex( 40,  0, 15);  // trunks
    Vex(-10, 10, 15); Vex(-10, 10,-15); Vex( 20, 10,-15); Vex( 20, 10, 15);  // roof
    glColor3f (0.7, 0.7, 0.5);
    Vex(-20,  0, 15); Vex(-10, 10, 15); Vex( 20, 10, 15); Vex( 25,  0, 15);  // window l
    Vex(-20,  0,-15); Vex(-10, 10,-15); Vex( 20, 10,-15); Vex( 25,  0,-15);  // window r
    glColor3f (0.7, 0.6, 0.5);
    Vex(-10, 10, 15); Vex(-20,  0, 15); Vex(-20,  0,-15); Vex(-10, 10,-15);  // windshield
    Vex( 20, 10, 15); Vex( 20, 10,-15); Vex( 25,  0,-15); Vex( 25,  0, 15);  // rear window
    glColor3f (0.4, 0.5, 0.5);
    Vex(-40,-15, 15); Vex(-40,-15,-15); Vex( 40,-15,-15); Vex( 40,-15, 15);  // bottom
    glEnd ();
    
    
    // Render wheels using torus's with lighting on.
    
    glEnable     (GL_LIGHTING);
    glColor3f    (0.3, 0.3, 0.3);
    Render_Wheel (-20,-15, 15,15);
    Render_Wheel ( 20,-15, 15,15);
    Render_Wheel (-20,-15,-15,15);
    Render_Wheel ( 20,-15,-15,15);
}
void Draw_frame(void)
{
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);

    glBegin(GL_QUADS);
    //BWALL
    
    glNormal3f(0,0,-1);
    Vex(0.2, 0.05, 0.0);
    Vex(0.2, 0.0, 0.0);
    Vex(0.3, 0.0 , 0.0);
    Vex(0.3, 0.05, 0.0);
    glEnd();

    glBegin(GL_QUADS);
    
    glNormal3f(0,0,1);
    Vex(0.2, 0.05, 0.05);
    Vex(0.2, 0.0, 0.05);
    Vex(0.3, 0.0, 0.05);
    Vex(0.3, 0.05, 0.05);
    glEnd();
    //LWALL

    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    Vex(0.2, 0.05, 0.0);
    Vex(0.2, 0, 0.0);
    Vex(0.2, 0, 0.05);
    Vex(0.2, 0.05, 0.05);
    glEnd();
    

    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    Vex(0.3, 0.05, 0.05);
    Vex(0.3, 0, 0.05);
    Vex(0.3, 0, 0);
    Vex(0.3, 0.05, 0.0);
    glEnd();
    
    //BULID ROOF

    glBegin(GL_QUADS);
    
    glNormal3f(0, -0.707, -0.707);
    
    Vex(0.2, 0.05, -0.00);
    Vex(0.2, 0.075, 0.025);
    Vex(0.3, 0.075, 0.025);
    Vex(0.3, 0.05, -0.00);
    glEnd();
    
    glBegin(GL_QUADS);
    
    glNormal3f(0, -0.707, 0.707);
    
    Vex(0.2, 0.075, 0.025);
    Vex(0.2, 0.05, 0.05);
    Vex(0.3, 0.05, 0.05);
    Vex(0.3, 0.075, 0.025);
    glEnd();
    

    
    // BUILD  FRONT roof

    
    glBegin(GL_TRIANGLES);
    
    glNormal3f(-1,0,0);
    Vex(0.2, 0.05, 0.0);
    Vex(0.2, 0.075, 0.025);
    Vex(0.2, 0.05, 0.05);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    
    glNormal3f(1,0,0);
    Vex(0.3, 0.05, 0.0);
    Vex(0.3, 0.075, 0.025);
    Vex(0.3, 0.05, 0.05);
    glEnd();

    
 

}
void Draw_house(void)
{
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[7]);
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    //BWALL
    
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);Vex(0.2, 0.05, 0.0);
    glTexCoord2f(1,0);Vex(0.2, 0.0, 0.0);
    glTexCoord2f(1,1);Vex(0.3, 0.0 , 0.0);
    glTexCoord2f(0,1);Vex(0.3, 0.05, 0.0);
    glEnd();

    glBegin(GL_QUADS);
    
    glNormal3f(0,0,1);
    glTexCoord2f(0,0);Vex(0.2, 0.05, 0.05);
    glTexCoord2f(1,0);Vex(0.2, 0.0, 0.05);
    glTexCoord2f(1,1);Vex(0.3, 0.0, 0.05);
    glTexCoord2f(0,1);Vex(0.3, 0.05, 0.05);
    glEnd();
    //LWALL
    

    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);Vex(0.2, 0.05, 0.0);
    glTexCoord2f(1,0);Vex(0.2, 0, 0.0);
    glTexCoord2f(1,1);Vex(0.2, 0, 0.05);
    glTexCoord2f(0,1);Vex(0.2, 0.05, 0.05);
    glEnd();
    

    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);Vex(0.3, 0.05, 0.05);
    glTexCoord2f(1,0);Vex(0.3, 0, 0.05);
    glTexCoord2f(1,1);Vex(0.3, 0, 0);
    glTexCoord2f(0,1);Vex(0.3, 0.05, 0.0);
    glEnd();
    
    //BULID ROOF

    glBindTexture(GL_TEXTURE_2D, texname[9]);
    glColor3f(0.2, 0.2, 0.1);
    glBegin(GL_QUADS);
    
    glNormal3f(0, -0.707, -0.707);
    glTexCoord2f(0,0);Vex(0.193, 0.045, -0.005);
    glTexCoord2f(1,0);Vex(0.193, 0.075, 0.025);
    glTexCoord2f(1,1);Vex(0.307, 0.075, 0.025);
    glTexCoord2f(0,1);Vex(0.307, 0.045, -0.005);
    glEnd();
    
    glBegin(GL_QUADS);
    
    glNormal3f(0, -0.707, 0.707);
    glTexCoord2f(0,0);Vex(0.193, 0.075, 0.025);
    glTexCoord2f(1,0);Vex(0.193, 0.045, 0.055);
    glTexCoord2f(1,1);Vex(0.307, 0.045, 0.055);
    glTexCoord2f(0,1);Vex(0.307, 0.075, 0.025);
    glEnd();
    
    //chimney

    
    // BUILD  FRONT roof

    glBindTexture(GL_TEXTURE_2D, texname[3]);
    glColor3f(1.0, 1.0, 1.0);
    
    glBegin(GL_TRIANGLES);
    
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);Vex(0.2, 0.05, 0.0);
    glTexCoord2f(1,0);Vex(0.2, 0.075, 0.025);
    glTexCoord2f(0.5,1);Vex(0.2, 0.05, 0.05);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);Vex(0.3, 0.05, 0.0);
    glTexCoord2f(1,0);Vex(0.3, 0.075, 0.025);
    glTexCoord2f(0.5,1);Vex(0.3, 0.05, 0.05);
    glEnd();
    //=========DOOR

    glBindTexture(GL_TEXTURE_2D, texname[8]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    
    
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);Vex(0.199, 0.035, 0.015);
    glTexCoord2f(1,0);Vex(0.199, 0.0, 0.015);
    glTexCoord2f(1,1);Vex(0.199, 0.0, 0.03);
    glTexCoord2f(0,1);Vex(0.199, 0.035, 0.03);
    glEnd();
    //BUILD WINDOW
    glBindTexture(GL_TEXTURE_2D, texname[6]);
    glBegin(GL_QUADS);
    
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);Vex(0.215, 0.035, -0.0001);
    glTexCoord2f(1,0);Vex(0.214, 0.013, -0.0001);
    glTexCoord2f(1,1);Vex(0.233, 0.013, -0.0001);
    glTexCoord2f(0,1);Vex(0.233, 0.035, -0.0001);
    glEnd();
    
    glBegin(GL_QUADS);
    
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);Vex(0.265, 0.035, -0.0001);
    glTexCoord2f(1,0);Vex(0.265, 0.013, -0.0001);
    glTexCoord2f(1,1);Vex(0.283, 0.013, -0.0001);
    glTexCoord2f(0,1);Vex(0.283, 0.035, -0.0001);
    glEnd();
    
    glBegin(GL_QUADS);
    
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0);Vex(0.215, 0.035, 0.0501);
    glTexCoord2f(1,0);Vex(0.215, 0.013, 0.0501);
    glTexCoord2f(1,1);Vex(0.233, 0.013, 0.0501);
    glTexCoord2f(0,1);Vex(0.233, 0.035, 0.0501);
    glEnd();
    
    glBegin(GL_QUADS);
    
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0);Vex(0.265, 0.035, 0.0501);
    glTexCoord2f(1,0);Vex(0.265, 0.013, 0.0501);
    glTexCoord2f(1,1);Vex(0.283, 0.013, 0.0501);
    glTexCoord2f(0,1);Vex(0.283, 0.035, 0.0501);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    

}
void DrawCircle(int k, int r, int h)
{
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[12]);
    
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 180; i++)
    {
        float x = r * cos(i) - h;
        float y = r * sin(i) + k;
        glNormal3f(0, 0, 1);
        x = r * cos(i + 0.1) - h;
        y = r * sin(i + 0.1) + k;
        glTexCoord2f(x, y);
        Vex(x + k,y - h,0);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void Draw_lake_rock(void)
{
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    DrawCircle(20, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslated(10, 0, 0);
    glColor3f(1.0, 1.0, 1.0);
    DrawCircle(20, 10, 10);
    glPopMatrix();
    
    //draw rocks
    for(int i = 0 ; i < 3 ; i++){
        glColor3f(1.0, 1.0, 1.0);
        glPushMatrix();
        glRotated(-5*i, 1, 0, 1);
        glTranslated(3+i*3, 1, -i+2);
        glScaled(10, 10, 1.5);
        draw_ball(10, 10, 8, 0);
        glPopMatrix();
    }
    for(int i = 0 ; i < 4 ; i++){
        glColor3f(0.45, 0.5, 0.45);
        glPushMatrix();
        glRotated(15*i, 0, 0, 1);
        glTranslated(35-i*3, 1.5*5, 0);
        glScaled(10, 10, 1.5);
        draw_ball(10, 10, 8, 0);
        glPopMatrix();
    }
}
void Wall(void)
{
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*0,1.0};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname[6]);
    glBegin(GL_QUADS);
    
    glNormal3f(0.0f,1.0f,0.0f);//crossproduct find normal.
    glTexCoord2f(0,0);Vex( 1.0f, 1.0f, -1.0f);
    glTexCoord2f(1,0);Vex(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1,1);Vex(-1.0f, 1.0f,  1.0f);
    glTexCoord2f(0,1);Vex( 1.0f, 1.0f,  1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
}
void Draw_snow_house(void)
{
    glColor3f(0.45, 0.76, 1);
    glPushMatrix();
    glRotated(-90, 1, 0, 0);
    draw_cone(25, 30, 4);
    glPopMatrix();
    
    glColor3f (0.7, 0.7, 0.5);
    glPushMatrix();
    glTranslated(0, -15, 0);
    glScaled(15, 20, 15);
    Draw_cube(3);
    glPopMatrix();
    
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glTranslated(-12, -21, 0);
    glScaled(7, 5, 5);
    Wall();
    glPopMatrix();

}

void idle1(void)
{
    int t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    
    if(t % 5 == 0 && t != previous)
    {
        
        if(zh <360)
        zh+=30;
        else
        zh =0;
        previous = t;

    }
    if(t%2 == 0 )
    {
        if(humanpos > humanpos2 && humanpos2 > 0)
        {
            humanpos = 0.0;
            humanpos2 = -25.0;
        }
        else if(humanpos< humanpos2 && humanpos2 < 0)
        {
            humanpos = 0.0;
            humanpos2 = 25.0;
        }
        
        if(humanpos2 > 0)
            humanpos+=0.1;
        else
            humanpos-=0.1;

    }
    
    glutPostRedisplay();
    
}


static void ball(double x,double y,double z,double r)
{
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  White ball
    glColor3f(1,1,1);
    glutSolidSphere(1.0,16,16);
    //  Undo transofrmations
    glPopMatrix();
}
void Display (void)
{
    
    
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();
    
    gluPerspective ( 60.0,  1.0,  1.0, 10.0);
    
    glMatrixMode   (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt(0, 1, -xcr*0.02, 0, 1, -1+-xcr*0.02, 0, 1, 0);
    
    float position[] = {distance*cosf(zh), distance*sinf(zh) , -20.0 ,1.0};
   // float position[] = {0, 10 , -30.0 ,1.0};
    glColor3f(1,1,1);
    ball(position[0],position[1],position[2] , 0.1);
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glEnable(GL_LIGHT0);
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,lit1_amb);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,lit1_dfs);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lit1_spc);
    glLightfv(GL_LIGHT0,GL_POSITION,position);
    

//===================================

    glTranslatef (xtm, ytm, -4.0);       // Place objects between near and far clipping planes.
    glRotatef    (tipp,  1, 0, 0);       // Rotate around screen horizontal axis. purpose: Debug
    glRotatef    (turn,  0, 1, 0);       // Rotate around world    vertical axis. Purpose: Debug

    glScalef     (scl, scl, scl );       // Scale world relative to its origin.
    glScalef     (0.02, 0.02, 0.02);
    
    
   // park scene
    
  { glPushMatrix();
    glRotated(90, 1, 0, 0);
    //glRotated(45, 0, 0, 1);
    glScaled(3, 2, 2);
    glTranslated(20, -210, 11);
    Draw_lake_rock();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-120, -25, -450);
    Draw_land();
    glPopMatrix();
    glPushMatrix();
    glTranslated(120, -25, -450);
    Draw_land();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.7, 0.7, 0.7);
    glTranslated(-100, 8, -60+humanpos);
    Draw_human();
    glPopMatrix();
    
      for(int i = 0 ; i < 2 ; i++)
      {
          glPushMatrix();
          glScaled(0.7, 0.7, 0.7);
          glRotated(15+i*15, 0, 1, 0);
          glTranslated(250+50*i, 8, -400-humanpos);
          Draw_human();
          glPopMatrix();
      
      }
    
    for(int i = 0; i >= -200; i-=50 ){
        glPushMatrix();
        glTranslated(-30, 8, i);
        Draw_tree();
        glPopMatrix();
    }
    for(int i = 0; i >= -200; i-=50 ){
        glPushMatrix();
        glTranslated(30, 8, i);
        Draw_tree();
        glPopMatrix();
    }
    glPushMatrix();
    glTranslated(-300, -25, -150);
    glScaled(800, 800, 800);
    Draw_house();
    glPopMatrix();
    glPushMatrix();
    glRotated(90, 0, 1, 0);
    glTranslated(100, -25, -150);
    glScaled(800, 800, 800);
    Draw_house();
    glPopMatrix();
    
    for(int i = 0 ; i < 10 ; i++){
    glPushMatrix();
    glColor3f(0.25, 0.2, 0.1);
        float x = 20*sinf(i*10);
        float z = 20*cosf(i*10);
    glTranslated(100+x, -22, -60+z);
        glRotated(5*i, 0, 1, 0);
    glScaled(10, 95, 60);
    Draw_frame();
    glPopMatrix();
    }
    glPushMatrix();
    glTranslated(100, -22, -65);
    Draw_tree2();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 0, 0);
    glScaled(10, 10, 10);
    glTranslated(0, 0, -80);
    Draw_tunnel(10, 30,0);
    glPopMatrix();
  }
    //another scene
    glPushMatrix();
    glTranslated(0, 0, -1000);
    {
    for(int i = 0 ; i < 10 ; i++){
    glPushMatrix();
        if(i % 2 == 0 ){
            glTranslated(-150+i*8.56, -20, -100+i*-10);
        }
        else glTranslated(76+i*11, -20, -185+ i*13);
        
   
    Draw_ChristmasTree();
        glPopMatrix();
    }
    
    for(int i = 0 ; i < 10 ; i++){
        glPushMatrix();
        if(i % 2 == 0 ){
            glTranslated(-150+i*8.56, -20, -300+i*14);
        }
        else glTranslated(60+i*12.45, -20, -340+ i*13);
        
        Draw_ChristmasTree();
        glPopMatrix();
    }
    
    
    
    glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(3, 2, 2);
    glTranslated(20, -210, 11);
    Draw_lake_rock();
    glPopMatrix();
    
    for(int i = 0 ; i < 5 ; i ++){
    glPushMatrix();
        
        if(i%2 ==0)
    glTranslated(-100+i*20, 11, -50-i*15);
        else
    glTranslated(100+i*20, 11, -150-i*15);
        
    glRotated(15*i, 0, 1, 0);
    Draw_snowMan();
    glPopMatrix();
    }
    
    //render snow every where.
    {
    glPushMatrix();
    glScaled(8, 5.8, 4.5);
    glTranslated(-26, -4, -5);
    Draw_snow();
    glPopMatrix();
    
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glScaled(5.3, 3.8, 3.13);
    glTranslated(-35, -1, -18.5);
    Draw_snow();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(9, 6.8, 6.5);
    glTranslated(20, -4, -115);
    Draw_snow();
    glPopMatrix();
    
    glPushMatrix();
    glRotated(-20, 1, 0, 0);
    glScaled(7.3, 5.8, 5.13);
    glTranslated(29, -1, -128.5);
    Draw_snow();
    glPopMatrix();}
    
    //render snow house
    {
        for(int i = 0 ; i < 4 ; i++ ){
        glPushMatrix();
        glTranslated(-100, 10, -350-50*i);
        
        Draw_snow_house();
        glPopMatrix();
        }
    }
    {
        for(int i = 0 ; i < 3 ; i++ ){
            glPushMatrix();
            
            glTranslated(-170+130*i, 10, -650);
            glRotated(15+5*i, 1, 0, 0);
            Draw_snow_house();
            glPopMatrix();
        }
    }
    glPushMatrix();
    glTranslated(-120, -25, -450);
    Draw_land();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(120, -25, -450);
    Draw_land();
    glPopMatrix();
        
        //tunnel
    glPushMatrix();
    glColor3f(0, 0, 0);
    glScaled(10, 10, 10);

    glTranslated(0, 0, -120);
    Draw_tunnel(10, 30,0);
    glPopMatrix();
        
    }
    glPopMatrix();

    glRotated(-90, 0, 1, 0);
    
    Draw_Road ();


    glTranslated(-xcr, 0, 0);
    Draw_Car  ();
    
    

   
    
    glFlush();
    
    glutSwapBuffers();
}

//-----------------------------------------   Init_GL   --------------------------------------------

void Init_GL (void)
{
  //  Init_Lights ();
    
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING  );
    glEnable (GL_COLOR_MATERIAL);
    
  //  glColorMaterial (GL_AMBIENT_AND_DIFFUSE, GL_FRONT);
  //  glColorMaterial (GL_SPECULAR           , GL_FRONT);
    
    // glClearColor (0.2, 0.3, 0.3, 1.0);
}

//----------------------------------------   Init_Glut   -------------------------------------------

void Init_Glut (void)
{
    glutInitDisplayMode    (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition (800, 200);
    glutInitWindowSize     (800, 800);
    glutCreateWindow       ("Car Tour");
    
    glutKeyboardFunc (Keybord);
    glutDisplayFunc  (Display);
    glutMouseFunc    (Mouse  );
    glutMotionFunc   (Motion );
    glutIdleFunc     (idle1);

}

//------------------------------------------   main   ----------------------------------------------

int main (int argc, char **argv)
{
    glutInit (&argc, argv);
    
    Init_Glut ();
    Init_GL   ();
    {//read bitmap
    texname[0]=LoadTexBMP("bmp/Stonewall.bmp");
    texname[1]=LoadTexBMP("bmp/Bark01.bmp");
    texname[2]=LoadTexBMP("bmp/Grass01.bmp");
    texname[3]=LoadTexBMP("bmp/metal2.bmp");
    texname[4]=LoadTexBMP("bmp/tree.bmp");
    texname[5]=LoadTexBMP("bmp/scree1.bmp");
    texname[6]=LoadTexBMP("bmp/window.bmp");
    texname[7]=LoadTexBMP("bmp/redbrick.bmp");
    texname[8]=LoadTexBMP("bmp/door.bmp");
    texname[9]=LoadTexBMP("bmp/wood.bmp");
    texname[10]=LoadTexBMP("bmp/cloth.bmp");
    texname[11]=LoadTexBMP("bmp/img4.bmp");
    texname[12]=LoadTexBMP("bmp/water.bmp");
    }
    glutMainLoop ();
        
    
    return (1);
}

// -------------------------------------------------------------------------------------------------
