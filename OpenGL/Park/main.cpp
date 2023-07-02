
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
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
float distance = 15.0;//light distance
int previous = -1;

int tp = -1;

unsigned int texname[13];
unsigned int sky[6];
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
GLfloat lit1_spc[] = {1.0, 1.0, 1.0, 1.0},
lit1_dfs[] = {1, 1,  1, 1.00},
lit1_amb[] = {.5, 0.2,  0.2, 1.00};

GLfloat lit2_amb[] = { 0.0, 0.2, 1, 1.0 },
lit2_dfs[] = { 0.0, 1.0, 1.0, 1.0 },
lit2_spc[] = { 0.0, 1.0, 1.0, 1.0 },
spot_dir[] = { 1.0, -1.0, 0.0 };


/*
 *  Load texture from BMP file
 */
unsigned int LoadTexBMP(const char* file)
{
   unsigned int   texture;    // Texture name
   FILE*          f;          // File pointer
   unsigned short magic;      // Image magic
   unsigned int   dx,dy,size; // Image dimensions
   unsigned short nbp,bpp;    // Planes and bits per pixel
   unsigned char* image;      // Image data
   unsigned int   off;        // Image offset
   unsigned int   k;          // Counter
   int            max;        // Maximum texture dimensions

   //  Open file
   f = fopen(file,"rb");
   if (!f) printf("Cannot open file");
   //  Check image magic
   if (fread(&magic,2,1,f)!=1) printf("Cannot read magic");
   if (magic!=0x4D42 && magic!=0x424D) printf("Image magic not BMP");
   //  Read header
   if (fseek(f,8,SEEK_CUR) || fread(&off,4,1,f)!=1 ||
       fseek(f,4,SEEK_CUR) || fread(&dx,4,1,f)!=1 || fread(&dy,4,1,f)!=1 ||
       fread(&nbp,2,1,f)!=1 || fread(&bpp,2,1,f)!=1 || fread(&k,4,1,f)!=1)
     exit(1);

   //  Check image parameters
   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
   if (dx<1 || dx>max) printf("image width out of range");
   if (dy<1 || dy>max) printf("image height out of range");
   if (nbp!=1)  printf(" bit planes is not 1");
   if (bpp!=24) printf("bits per pixel is not 24");
   if (k!=0)    printf("compressed files not supported");
#ifndef GL_VERSION_2_0
   //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
   for (k=1;k<dx;k*=2);
   if (k!=dx) {printf("image width not a power of two");exit(1);}
   for (k=1;k<dy;k*=2);
   if (k!=dy) {printf("image height not a power of two");exit(1);}
#endif

   //  Allocate image memory
   size = 3*dx*dy;
   image = (unsigned char*) malloc(size);
   if (!image) {printf("Cannot allocate memory for image");exit(1);}
   //  Seek to and read image
   if (fseek(f,off,SEEK_SET) || fread(image,size,1,f)!=1) {printf("Texture failed to load at path");exit(1);}
   fclose(f);
   //  Reverse colors (BGR -> RGB)
   for (k=0;k<size;k+=3)
   {
      unsigned char temp = image[k];
      image[k]   = image[k+2];
      image[k+2] = temp;
   }


   //  Generate 2D texture
   glGenTextures(1,&texture);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Copy image
   glTexImage2D(GL_TEXTURE_2D,0,3,dx,dy,0,GL_RGB,GL_UNSIGNED_BYTE,image);
   if (glGetError()) fprintf(stderr,"ERROR: %s\n",gluErrorString(glGetError()));
   //  Scale linearly when image size doesn't match
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

   //  Free image memory
   free(image);
   //  Return texture name
   return texture;
}

void Keybord (unsigned char ch, int x, int y)
{
    switch (ch)  {
            
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
    Vex(0, 0, height);
    for (int angle = 0; angle < 360; angle++) {
        glNormal3f(-sin(angle)+sin(angle+5), -cos(angle)+cos(angle+5), cos(angle)*sin(angle+5)-sin(angle)*cos(angle+5));
        glTexCoord2f(0.5,1);
        Vex(sin(angle) * radius, cos(angle) * radius, 0);
    }
    glEnd();
    
    // draw the base of the cone
    glBegin(GL_TRIANGLE_FAN);
    Vex(0, 0, 0);
    for (int angle = 0; angle < 360; angle++) {
        // normal is just pointing down
        Vex(sin(angle) * radius, cos(angle) * radius, 0);
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
    if(opt!=-1){//opt -1 no texture apply
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
            Vex(x, y, z0);
            
            glNormal3f(x / radius, y / radius, 0.0);
            glTexCoord2f(j / (GLfloat) 20, (i + 1) / (GLfloat) 50);
            Vex(x, y, z1);
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
    glTranslated(-200, -23.5, 0);
    glScalef(400.0, 2,20);
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



void drawSkybox(float size)
{
        glDisable(GL_LIGHTING); //turn off lighting, when making the skybox
        glDisable(GL_DEPTH_TEST);       //turn off depth testing
        glEnable(GL_TEXTURE_2D);        //and turn on texturing
        glBindTexture(GL_TEXTURE_2D,sky[0]);  //sky texture 
        glBegin(GL_QUADS);      //and draw a face
                //back face
                glTexCoord2f(0,0);      
                Vex(size,size,size);       
                glTexCoord2f(1,0);      
                Vex(-size,size,size);
                glTexCoord2f(1,1);
                Vex(-size,-size,size);
                glTexCoord2f(0,1);
                Vex(size,-size,size);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,sky[5]);
        glBegin(GL_QUADS);     
                //left face
                glTexCoord2f(0,0);
                Vex(-size,size,size);
                glTexCoord2f(1,0);
                Vex(-size,size,-size);
                glTexCoord2f(1,1);
                Vex(-size,-size/2,-size/2);
                glTexCoord2f(0,1);
                Vex(-size,-size,size);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,sky[2]);
        glBegin(GL_QUADS);     
                //front face
                glTexCoord2f(1,0);
                Vex(size,size,-size);
                glTexCoord2f(0,0);
                Vex(-size,size,-size);
                glTexCoord2f(0,1);
                Vex(-size,-size,-size);
                glTexCoord2f(1,1);
                Vex(size,-size,-size);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,sky[3]);
        glBegin(GL_QUADS);     
                //right face
                glTexCoord2f(0,0);
                Vex(size,size,-size);
                glTexCoord2f(1,0);
                Vex(size,size,size);
                glTexCoord2f(1,1);
                Vex(size,-size,size);
                glTexCoord2f(0,1);
                Vex(size,-size,-size);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,sky[1]);          
        glBegin(GL_QUADS);                      //top face
                glTexCoord2f(1,0);
                Vex(size,size,size);
                glTexCoord2f(0,0);
                Vex(-size,size,size);
                glTexCoord2f(0,1);
                Vex(-size,size,-size);
                glTexCoord2f(1,1);
                Vex(size,size,-size);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,sky[4]);               
        glBegin(GL_QUADS);     
                //bottom face
                glTexCoord2f(1,1);
                Vex(size,-size,size);
                glTexCoord2f(0,1);
                Vex(-size,-size,size);
                glTexCoord2f(0,0);
                Vex(-size,-size,-size);
                glTexCoord2f(1,0);
                Vex(size,-size,-size);
        glEnd();
        glEnable(GL_LIGHTING);  //turn everything on
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
}

void Display (void)
{
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 800, 600);
    glMatrixMode   (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective ( 60.0,  8/6,  1.0, 100.0);
    glMatrixMode   (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt(0, 1, -xcr*0.02, 0, 1, -1+-xcr*0.02, 0, 1, 0);
    
    float position[] = {distance, distance , -20.0 ,1.0};
    float position2[] = {-1, 3.0, -5, 1.0};

    glColor3f(1,1,1);
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
   
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,lit1_amb);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,lit1_dfs);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lit1_spc);
    glLightfv(GL_LIGHT0,GL_POSITION,position);
    glEnable(GL_LIGHT0);

  
   glLightfv(GL_LIGHT1, GL_AMBIENT, lit2_amb);
   glLightfv(GL_LIGHT1, GL_DIFFUSE, lit2_dfs);
   glLightfv(GL_LIGHT1, GL_SPECULAR, lit2_spc);
   glLightfv(GL_LIGHT1, GL_POSITION, position2);
   glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, .5);
   glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
   glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1);

   glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
   glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
   glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);
   glEnable(GL_LIGHT1);

   //fog
   glEnable(GL_FOG);
   
      GLfloat fogColor[4] = {0.2, 0.2, 0.2, 1.0};

      static GLint fogMode = GL_EXP;
      glFogi (GL_FOG_MODE, fogMode);
      glFogfv (GL_FOG_COLOR, fogColor);
      glFogf (GL_FOG_DENSITY, 0.3);
      glHint (GL_FOG_HINT, GL_DONT_CARE);
      glFogf (GL_FOG_START, 1.0);
      glFogf (GL_FOG_END, 5.0);
   
   glClearColor(0.2, 0.2, 0.2, 1.0);  /* fog color */

//===================================

    glTranslatef (xtm, ytm, -4.0);       // Place objects between near and far clipping planes.
    glRotatef    (tipp,  1, 0, 0);       // Rotate around screen horizontal axis. purpose: Debug
    glRotatef    (turn,  0, 1, 0);       // Rotate around world    vertical axis. Purpose: Debug

    glScalef     (scl, scl, scl );       // Scale world relative to its origin.
    glScalef     (0.02, 0.02, 0.02);
    
    drawSkybox(250);
   // park scene
    
  { glPushMatrix();
    glRotated(90, 1, 0, 0);
    glScaled(3, 2, 2);
    glTranslated(20, 0, 11);
    Draw_lake_rock();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(-100, -25, -300);
    Draw_land();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(100, -25, -300);
    Draw_land();
    glPopMatrix();
    
    glPushMatrix();
    glScaled(0.7, 0.7, 0.7);
    glTranslated(-100, 8, -60);
    Draw_human();
    glPopMatrix();
    

    

   glPushMatrix();
   glTranslated(-30, 8, 0);
   Draw_tree();
   glPopMatrix();
    

    glPushMatrix();
    glTranslated(-300, -25, -150);
    glScaled(800, 800, 800);
    Draw_house();
    glPopMatrix();

    glPushMatrix();
    glTranslated(100, 15, -120);
    Draw_snow_house();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-120, 13, -25);
    glRotated(15, 0, 1, 0);
    Draw_snowMan();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(40, -17, -100);
    Draw_ChristmasTree();
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
    glScaled(20, 10, 10);
    glTranslated(0, 0, -40);
    Draw_tunnel(10, 30,0);
    glPopMatrix();

  }
   glPushMatrix();
    glRotated(-90, 0, 1, 0);
    Draw_Road ();
    Draw_Car  ();
    glPopMatrix();
    
    glFlush();
    
    glutSwapBuffers();
}

//-----------------------------------------   Init_GL   --------------------------------------------

void Init_GL (void)
{
    
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING  );
    glEnable (GL_COLOR_MATERIAL);

}

//----------------------------------------   Init_Glut   -------------------------------------------

void Init_Glut (void)
{
    glutInitDisplayMode    (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition (800, 200);
    glutInitWindowSize     (800,600);
    glutCreateWindow       ("Final");
    
    glutKeyboardFunc (Keybord);
    glutDisplayFunc  (Display);
    glutMouseFunc    (Mouse);
    glutMotionFunc   (Motion);

}

//------------------------------------------   main   ----------------------------------------------

int main (int argc, char **argv)
{
    glutInit (&argc, argv);
    
    Init_Glut ();
    Init_GL   ();
    {//read bitmap
    sky[3]=LoadTexBMP("skybox/negx.bmp");
    sky[4]=LoadTexBMP("skybox/negy.bmp");
    sky[5]=LoadTexBMP("skybox/negz.bmp");
    sky[0]=LoadTexBMP("skybox/posx.bmp");
    sky[1]=LoadTexBMP("skybox/posy.bmp");
    sky[2]=LoadTexBMP("skybox/posz.bmp");

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


