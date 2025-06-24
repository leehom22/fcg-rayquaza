#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "imageloader.h"

using namespace std;

#define NUM_SEGMENTS 6
#define M_PI 3.14159265359
#define NUM_CLOUDS 50

const int SEGMENTS = 20;       // More segments = smoother S
const float SEG_LENGTH = 1.0f; // Length of each segment
const float SEG_RADIUS = 0.5f; // Radius of each segment
const float ROTATE_LENGTH = 0.01f;
static int segment1 = 0, segment2 = 0, segment3 = 0, segment4 = 0, segment5 = 0, segment6 = 0, segment7 = 0;
static int head = 0, tail = 0;

float angle = 0.0f;

float lx = 0.0f, lz = -1.0f, ly = 0.1f;

float x = 0.0f, z = 5.0f, y = 0.0f;


float deltaANGLE = 0.0f;
float deltaMove = 0;
float deltaAngleLR = 0.0f;
float deltaY = 0.0f;

float pitch = 0.0f;
float deltaPitch = 0.0f;

int xOrigin = -1;
int yOrigin = -1;

//Translation whrn body move
float moveX[NUM_SEGMENTS];
float moveY[NUM_SEGMENTS];
float moveZ[NUM_SEGMENTS];

float segments[NUM_SEGMENTS] = { 0, 0, 0, 0 }; // from tail (0) to head (3)
const float segmentLength = 0.01f; // adjust this based on your model

//Rotation when body move
float yawSegments[NUM_SEGMENTS];
float pitchSegments[NUM_SEGMENTS];

bool moveUp = false;
bool moveDown = false;
bool moveLeft = false;
bool moveRight = false;

bool forwardFront = false;
bool forwardBack = false;
bool forwardLeft = false;
bool forwardRight = false;
bool forwardUp = false;
bool forwardDown = false;


bool resetBody = false;
bool resetLocation = false;

//darshini
float energyOffset = 0.0f;
float ballZ = 0.0f;
float ballScale = 0.1;
float explosionTime = 0.0f;
float ball = 0.0f;

// Manual control for energy ball
float energyBallX = 0.0f;
float energyBallY = 0.0f;
float energyBallZ = 0.6f;  // default in-hand position
bool controlBall = false;  // toggle manual control

//darshini
bool shooting = false; // to make the energy ball shoot
bool exploding = false;

//cloud
double cloudPosition[NUM_CLOUDS][3];
double cloudOffSet = 0.0;

// Light 0 setup (white directional light from top)
GLfloat light_pos[] = { 1.0f, 8.0f, 5.0f, 1.0f }; // positional light //directional light w=0 //for sun
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // white light
GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // low ambient light
GLfloat light1_pos[] = { 1.0f, 3.0f,7.0f, 0.0f }; //for main model

//texture mapping
GLuint loadTexture(Image* image)
{
    GLuint textureId;
    glGenTextures(1, &textureId); // Make room for our texture
    glBindTexture(GL_TEXTURE_2D, textureId); // Tell OpenGL which texture to edit
    // Map the image to the texture
    glTexImage2D(GL_TEXTURE_2D,
        0,                // Always GL_TEXTURE_2D
        GL_RGB,           // Format OpenGL uses for image
        image->width, image->height, // Width and height
        0,                // The border of the image
        GL_RGB,           // GL_RGB, because pixels are stored in RGB format
        GL_UNSIGNED_BYTE, // GL_UNSIGNED_BYTE, because pixels are stored as unsigned numbers
        image->pixels);   // The actual pixel data

    return textureId; // Returns the id of the texture
}

GLuint _textureId1;



void lukisPaksi()
{
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    glVertex3i(-100, 0, 0);
    glVertex3i(100, 0, 0);
    glVertex3i(0, -100, 0);
    glVertex3i(0, 100, 0);
    glVertex3i(0, 0, -100);
    glVertex3i(0, 0, 100);
    glEnd();

}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}


void drawHorizontalStripe(float radiusX, float radiusY, float zPos, float stripeWidth) {
    int segments = 100;
    float angleStep = 360.0f / segments;

    glColor3f(0.9098f, 0.8980f, 0.1020f); // dark yellow stripe
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep * M_PI / 180.0f;

        float cosA = cos(angle);
        float sinA = sin(angle);

        // Base oval radii
        float x1 = radiusX * cosA;
        float y1 = radiusY * sinA;

        float x2 = (radiusX + stripeWidth) * cosA;
        float y2 = (radiusY + stripeWidth) * sinA;

        glVertex3f(x1, zPos, y1); // inner edge
        glVertex3f(x2, zPos, y2); // outer edge
    }
    glEnd();
}

void drawStraightStripe(float radius, float length, float stripeWidth, float angleDeg) {
    // Convert the angle to radians
    float angleRad = angleDeg * M_PI / 180.0f;

    // Fixed position on the circular body
    float x = radius * cos(angleRad);
    float y = radius * sin(angleRad);

    glColor3f(0.9098f, 0.8980f, 0.1020f);// dark yellow stripe
    glBegin(GL_QUADS);

    // Draw a rectangle along the z-axis at fixed (x, y)
    glVertex3f(x, y, 0.0f);               // Bottom start
    glVertex3f(x, y, length);             // Top end
    glVertex3f(x + stripeWidth, y, length); // Top end offset
    glVertex3f(x + stripeWidth, y, 0.0f);   // Bottom start offset

    glEnd();
}



void drawStripeOnSegment(float radius, float length) {
    int segments = 100;  // smoother stripe
    float angleStep = 360.0f / segments;
    float stripeHeight = 0.05f; // vertical thickness of stripe

    glColor3f(0.9098f, 0.8980f, 0.1020f);// dark yellow stripe
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep * M_PI / 180.0f;

        float x = radius * cos(angle);
        float y = radius * sin(angle);
        float z1 = 0.0f;
        float z2 = length;

        // Horizontal stripe near the middle of the body
        float stripeY1 = y;

        glVertex3f(x, stripeY1, z1);  // lower edge of stripe at z1
        glVertex3f(x, stripeY1, z2);  // lower edge of stripe at z2
    }
    glEnd();
}

void drawCylinderSegment(float radius_, float length_) {

    float seg_radius = radius_;
    float seg_length = length_;

    if (radius_ == 0 || length_ == 0) {
        seg_radius = SEG_RADIUS;
        seg_length = SEG_LENGTH;
    }

    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, seg_radius, seg_radius, seg_length, 20, 5);

    // Draw end caps
    gluDisk(quad, 0, seg_radius, 20, 1);  // base cap
    glTranslatef(0, 0, seg_length);
    gluDisk(quad, 0, seg_radius, 20, 1);  // top cap

    gluDeleteQuadric(quad);
}


void drawHead() { //z exchange with y

    glColor3d(0.0078, 0.6000, 0.1412); //same color with body 
    // diamter = 0.44
    // minus x and y with 0.55
    //glColor3d(1, 0, 0); //red
    glBegin(GL_QUADS); //base rectangle
    glVertex3d(0, 0.5, 0); //E
    glVertex3d(0, 1, 0); //A
    glVertex3d(-0.55, 1, 0); //B
    glVertex3d(-0.55, 0.5, 0); //D
    glEnd();

    //glColor3d(0, 1, 0); //green
    glBegin(GL_TRIANGLES);  //left triangles
    glVertex3d(0, 1, 0); // A
    glVertex3d(-0.17, 0.77, 0.6); //C
    glVertex3d(0, 0.5, 0); // E
    glEnd();

    //testing to draw an eye on right 

    //x =-0.49 left eye
    //x =-0.06  right eye
    glColor3d(0, 0, 0);

    glBegin(GL_TRIANGLES);
    glVertex3d(-0.4978, 0.8321, 0.18585); //I  //left eye
    glVertex3d(-0.5, 0.7, 0.2); //J 
    glVertex3d(-0.48038, 0.76614, 0.3243); //K  

    glVertex3d(-0.06, 0.83, 0.19); //M //right eye
    glVertex3d(-0.06, 0.7, 0.2); //L 
    glVertex3d(-0.06, 0.77, 0.32); //N 
    glEnd();

    glColor3d(1, 1, 0);
    glBegin(GL_QUADS); //left eye ball
    glVertex3d(-0.4970, 0.7725, 0.24); //left
    glVertex3d(-0.49903, 0.78169, 0.26319);  //up
    glVertex3d(-0.4970, 0.77, 0.28); //right
    glVertex3d(-0.49349, 0.74825, 0.26); //down

    glVertex3d(-0.05, 0.74, 0.26); //down
    glVertex3d(-0.05, 0.77, 0.28009); //right
    glVertex3d(-0.05, 0.78, 0.26); //up
    glVertex3d(-0.05, 0.76888, 0.23791); //left
    glEnd();

    //draw right eye end here 

    glColor3d(0.0078, 0.6000, 0.1412); //same color with body 

    //glColor3d(0, 0, 1); //blue
    glBegin(GL_TRIANGLES); //right triangles
    glVertex3d(-0.55, 1, 0); //B
    glVertex3d(-0.37, 0.77, 0.6); //F
    glVertex3d(-0.55, 0.5, 0); //D
    glEnd();

    //glColor3d(1, 1, 0); //yellow
    glBegin(GL_QUADS); //top triangles
    glVertex3d(-0.55, 1, 0); //B
    glVertex3d(0, 1, 0); //A
    glVertex3d(-0.17, 0.77, 0.6); //C
    glVertex3d(-0.37, 0.77, 0.6); //F
    glVertex3d(-0.55, 1, 0); //B
    glEnd();

    //glColor3d(1, 0, 1); //magenta
    glBegin(GL_QUADS);//botom triangles
    glVertex3d(-0.17, 0.77, 0.6); //C
    //glVertex3d(0, 0.77, 0);  // G
    //glVertex3d(-1, 0.5, 0);  //H
    glVertex3d(-0.37, 0.77, 0.6); //F
    glVertex3d(-0.17, 0.77, 0.6); //C
    glEnd();

    // fins //justin
    glColor3d(0.0078, 0.6000, 0.1412); //color

    // top fin
    glBegin(GL_POLYGON);
    glVertex3d(-0.48, 0.55, 0.11);
    glVertex3d(-1.25, 0.01, -0.02);
    glVertex3d(-0.66, 0.42, -0.17);
    glVertex3d(-0.66, 0.42, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3d(-0.04, 0.55, 0.11);
    glVertex3d(0.73, 0.01, -0.02);
    glVertex3d(0.14, 0.42, -0.17);
    glVertex3d(0.14, 0.42, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3d(-0.48, 0.95, 0.11);
    glVertex3d(-1.25, 1.49, -0.02);
    glVertex3d(-0.66, 1.08, -0.17);
    glVertex3d(-0.66, 1.08, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3d(-0.04, 0.95, 0.11);
    glVertex3d(0.73, 1.49, -0.02);
    glVertex3d(0.14, 1.08, -0.17);
    glVertex3d(0.14, 1.08, 0.0);
    glEnd();
}

void drawEnergyBall(float radius) { //darshini
    glPushMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.2f, 0.8f, 1.0f, 0.7f); //cyan

    for (int i = 0; i < 3; ++i) {
        glutSolidSphere(radius - i * 0.02f, 20, 20);
    }

    glDisable(GL_BLEND);
    glPopMatrix();
}

void drawClaw() { //darshini
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.1f); // Yellow claw color

    // Draw three claw fingers
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 30.0f - 30.0f, 0.0f, 1.0f, 0.0f); // Spread fingers
        glTranslatef(0.0f, 0.0f, 0.1f); // Move forward

        // Cone-shaped claw
        GLUquadric* quad = gluNewQuadric();
        gluCylinder(quad, 0.03f, 0.01f, 0.15f, 6, 1);
        glTranslatef(0.0f, 0.0f, 0.15f);
        gluDisk(quad, 0, 0.01f, 6, 1);
        gluDeleteQuadric(quad);

        glPopMatrix();
    }
    glPopMatrix();
}

void drawForearm(float length, float radius) { //darshini
    GLUquadric* quad = gluNewQuadric();
    glColor3f(0.0f, 0.8f, 0.2f); // green
    gluCylinder(quad, radius, radius * 0.6f, length, 10, 5);
    glTranslatef(0.0f, 0.0f, length);
    drawClaw();
    gluDeleteQuadric(quad);
}


void drawUpperArm(float length, float radius, bool isLeft) { //darshini
    GLUquadric* quad = gluNewQuadric();
    glColor3f(0.0f, 0.8f, 0.2f); // green
    gluCylinder(quad, radius, radius * 0.8f, length, 10, 5);
    glTranslatef(0.0f, 0.0f, length);

    if (isLeft) glRotatef(30, 0, 1, 0);  // left

    else
        glRotatef(-30, 0, 1, 0); // right

    drawForearm(0.3f, radius * 0.8f);
    gluDeleteQuadric(quad);
}

void drawArm(bool isLeft) { //darshini
    glPushMatrix();

    // Position left/right
    float sideOffset = isLeft ? -0.22f : 0.22f;
    glTranslatef(sideOffset, -0.1f, 0.0f); // offset from body

    // Shoulder sphere
    glColor3f(0.0f, 0.6f, 0.0f);
    glutSolidSphere(0.1f, 20, 20);

    // Arm rotation
    glPushMatrix();
    float shoulderTilt = isLeft ? 30.0f : -30.0f;
    float forwardAngle = isLeft ? -25.0f : 25.0f;
    glRotatef(shoulderTilt, 0, 0, 1);   // tilt down
    glRotatef(forwardAngle, 0, 1, 0);   // rotate forward
    drawUpperArm(0.1f, 0.06f, isLeft);
    glPopMatrix();

    glPopMatrix();
}

void drawFin() {

    glBegin(GL_QUADS);
    glColor3d(0.0078, 0.6000, 0.1412);//green
    double width = 0.02;
    double height = 0.5;
    double length = 0;
    //inner
    // FRONT face (A-B-C-D)
    glVertex3d(0, 0, 0);     // A
    glVertex3d(0, 0.5, 0.3); // B
    glVertex3d(0, height, 0.6);   // C
    glVertex3d(0, 0, 0.9);   // D

    // BACK face (E-F-G-H)
    glVertex3d(width, 0, 0);     // E
    glVertex3d(width, height, 0.3); // F
    glVertex3d(width, height, 0.6);   // G
    glVertex3d(width, 0, 0.9);   // H

    // LEFT face (A-B-F-E)
    glVertex3d(0, 0, 0);     // A
    glVertex3d(0, height, 0.3); // B
    glVertex3d(width, height, 0.3); // F
    glVertex3d(width, 0, 0);   // E

    // RIGHT face (C-D-H-G)
    glVertex3d(0, height, 0.6);   // C
    glVertex3d(0, 0, 0.9);   // D
    glVertex3d(width, 0, 0.9); // H
    glVertex3d(width, height, 0.6); // G

    // TOP face (B-C-G-F)
    glVertex3d(0, height, 0.3); // B
    glVertex3d(0, height, 0.6);   // C
    glVertex3d(width, height, 0.6); // G
    glVertex3d(width, height, 0.3); // F

    // BOTTOM face (A-D-H-E)
    glVertex3d(0, 0, 0);     // A
    glVertex3d(0, 0, 0.9);   // D
    glVertex3d(width, 0, 0.9); // H
    glVertex3d(width, 0, 0);   // E

    //outer
    glEnd();


    //outline - left-top
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(-0.001, 0.47, 0.3);   // J
    glVertex3d(-0.001, 0.47, 0.6);   // K
    glVertex3d(-0.001, height, 0.6);   // C
    glVertex3d(-0.001, 0.5, 0.3); // B

    glEnd();

    //outline - left-right
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(-0.001, height, 0.6);   // C
    glVertex3d(-0.001, 0.50, 0.54);   // K
    glVertex3d(0.0, 0.0, 0.8);   // L
    glVertex3d(0, 0, 0.9);   // D

    glEnd();

    //outline - left-left
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(-0.001, 0.47, 0.35);   // J
    glVertex3d(0.0, 0.0, 0.1);   // I
    glVertex3d(0, 0, 0);     // A
    glVertex3d(0, height, 0.3); // B

    glEnd();

    //outline - right-top
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(width, height, 0.6); // G
    glVertex3d(0.07, 0.45, 0.6);   // O
    glVertex3d(0.07, 0.45, 0.3);   // N
    glVertex3d(width, height, 0.3); // F

    glEnd();

    //outline - right-right
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(width, height, 0.3); // F
    glVertex3d(width, 0, 0);   // E
    glVertex3d(0.07, 0.0, 0.1);   // M
    glVertex3d(0.07, 0.45, 0.35);   // N

    glEnd();

    //outline - right-left
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red

    glVertex3d(width, height, 0.6); // G
    glVertex3d(0.07, 0.45, 0.55);   // O
    glVertex3d(0.07, 0.0, 0.8);   // P
    glVertex3d(width, 0, 0.9); // H

    glEnd();

    //outline - top
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red
    glVertex3d(0, height + 0.01, 0.3); // B
    glVertex3d(width, height + 0.01, 0.3); // F
    glVertex3d(width, height + 0.01, 0.6); // G
    glVertex3d(-0.001, height + 0.01, 0.6);   // C
    glEnd();

    //outline - top-right
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red
    glVertex3d(0, height + 0.01, 0.3); // B
    glVertex3d(width, height + 0.01, 0.3); // F
    glVertex3d(width, 0.01, 0);   // E
    glVertex3d(0, 0.01, 0);     // A
    glEnd();

    //outline - top-left
    glBegin(GL_POLYGON);
    glColor3d(0.91, 0, 0);//dark red
    glVertex3d(-0.001, height + 0.01, 0.6);   // C
    glVertex3d(width, height + 0.01, 0.6); // G
    glVertex3d(width, 0, 0.9); // H
    glVertex3d(0, 0.01, 0.9);   // D
    glEnd();
}

void drawFinTail() { //y=0.3 z=1.0 x=0.03
    double height = 0.3;
    double length = 1.0;
    double width = 0.03;

    glBegin(GL_POLYGON);  //front
    glColor3d(0.0078, 0.6000, 0.1412); //body color
    glVertex3d(0, 0, 0);   // A
    glVertex3d(0, height, 0);   // B
    glVertex3d(0, 0, length);   // C
    glEnd();

    glBegin(GL_POLYGON);  //back
    glColor3d(0.0078, 0.6000, 0.1412); //body color
    glVertex3d(width, 0, 0);   // D
    glVertex3d(width, height, 0);   // E
    glVertex3d(width, 0, length);   // F
    glEnd();

    glBegin(GL_POLYGON);  //top BEFC
    glColor3d(0.91, 0, 0);//dark red 
    glVertex3d(0, height, 0);   // B
    glVertex3d(width, height, 0);   // E
    glVertex3d(width, 0, length);   // F
    glVertex3d(0, 0, length);   // C
    glEnd();

    glBegin(GL_POLYGON);  //right BEDA
    glColor3d(0.91, 0, 0);//dark red 
    glVertex3d(0, height, 0);   // B
    glVertex3d(width, height, 0);   // E
    glVertex3d(width, 0, 0);   // D
    glVertex3d(0, 0, 0);   // A
    glEnd();
}

float angleRadMove = 0;
float dx = 0;
float dz = 0;
void drawRayquaza() {
    glPushMatrix();

    glTranslated(0, 2.0, 0);
    glTranslated(moveX[NUM_SEGMENTS - 1], moveY[NUM_SEGMENTS - 1], moveZ[NUM_SEGMENTS - 1]);



    for (int i = 0; i < NUM_SEGMENTS; i++) {

        // Apply movement based on segment orientation
        //angleRadMove = yawSegments[i] * M_PI / 180.0f;
        //dx = sin(angleRadMove) * moveZ[i] + moveX[i];
        //dz = cos(angleRadMove) * moveZ[i];

        //glTranslatef(moveX[i], moveY[i], moveZ[i]); // Move in rotated direction
        //cout << "angleRad= " << angleRadMove << " dx= " << dx << " dz= " << dz << endl;

        glColor3d(0.0078, 0.6000, 0.1412);//green
        glRotatef(yawSegments[i], 0.0f, 1.0f, 0.0f);
        glRotatef(pitchSegments[i], 1.0f, 0.0f, 0.0f); // rotate segment


        if (i == 0) {

            glPushMatrix(); //top
            drawFinTail();
            glPopMatrix();

            glPushMatrix(); //bottom
            glRotated(180, 0, 0, 1);
            drawFinTail();
            glPopMatrix();

            glPushMatrix(); //left
            glRotated(90, 0, 0, 1);
            drawFinTail();
            glPopMatrix();

            glPushMatrix(); //right
            glRotated(90, 0, 0, -1);
            drawFinTail();
            glPopMatrix();

            glColor3d(0.0078, 0.6000, 0.1412); //body 

            drawCylinderSegment(0.1f, 1.0f);

            //glPopMatrix();
        }
        else if (i == 1) {
            double scale = 0.7;

            glPushMatrix(); //straight line up
            glTranslated(0.0, -0.85, 0.58);
            drawStraightStripe(1.0f, 0.5f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval up
            glTranslated(0.0, 0.14, 0.42);
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //straaight line down
            glTranslated(0.0, -1.19, 0.61);
            drawStraightStripe(1.0f, 0.5f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval down
            glTranslated(0.0, -0.20, 0.42);
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //vertical circle 
            glTranslated(0.0, 0.0, 0.23);
            drawStripeOnSegment(0.15f, 0.03f);
            glPopMatrix();

            //Upper fin
            glPushMatrix();
            glScaled(scale, scale, scale);
            glRotated(20, 0, 0, 1);
            drawFin();
            glPopMatrix();

            //bottom fin
            glPushMatrix();
            glScaled(scale, scale, scale);
            glRotated(200, 0, 0, 1);
            drawFin();
            glPopMatrix();

            //left fin
            glPushMatrix();
            glScaled(scale, scale, scale);
            glRotated(110, 0, 0, 1);
            drawFin();
            glPopMatrix();

            //right fin
            glPushMatrix();
            glScaled(scale, scale, scale);
            glRotated(-110, 0, 0, 1);
            drawFin();
            glPopMatrix();

            glColor3d(0.0078, 0.6000, 0.1412); //body 
            drawCylinderSegment(0.15f, 1.05f);
        }
        else if (i == 2) {

            glPushMatrix(); //straight line up
            glTranslated(0.0, -0.78, 0.58); //+7
            drawStraightStripe(1.0f, 0.5f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval up
            glTranslated(0.0, 0.18, 0.42); //+4
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //straaight line down
            glTranslated(0.0, -1.26, 0.61);
            drawStraightStripe(1.0f, 0.5f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval down
            glTranslated(0.0, -0.24, 0.42);
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //vertical circle 
            glTranslated(0.0, 0.0, 0.23);
            drawStripeOnSegment(0.22f, 0.05f);
            glPopMatrix();

            glColor3d(0.0078, 0.6000, 0.1412); //body
            drawCylinderSegment(0.2f, 1.1f);

            // right arm
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.5f);
            drawArm(false);
            glPopMatrix();

            // lefft arm
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.5f);
            drawArm(true);
            glPopMatrix();

            // Fins (left and right) 
            glPushMatrix();
            glTranslated(0.0, 0.0, 0.3);  // adjust position along the segment
            glColor3d(1.0, 0.0, 0.0);     // red fins

            //Upper fin
            glPushMatrix();
            glTranslated(0, 0, -1);
            drawFin();
            glPopMatrix();

            //bottom fin
            glPushMatrix();
            glTranslated(0, 0, -1);
            glRotated(180, 0, 0, 1);
            drawFin();
            glPopMatrix();

            //left fin
            glPushMatrix();
            glTranslated(0, 0, -1);
            glRotated(90, 0, 0, 1);
            drawFin();
            glPopMatrix();

            //right fin
            glPushMatrix();
            glTranslated(0, 0, -1);
            glRotated(-90, 0, 0, 1);
            drawFin();
            glPopMatrix();

            // Left fin //azim
            glPushMatrix();
            glTranslated(-0.1, 0.0, 0.0);

            glEnd();
            glPopMatrix();

            glPopMatrix();

            // tail fin //azim
            glPushMatrix();
            glTranslated(0.0, 0.0, 1.0); // move to end of the tail segment
            glColor3d(1.0, 0.0, 0.0);    // red tail fin

            glBegin(GL_TRIANGLES);
            // vertical fin //azim
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.3f, 0.2f);
            glVertex3f(0.0f, -0.3f, 0.2f);
            glEnd();

            glBegin(GL_TRIANGLES);
            // horizontal fin //azim
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.3f, 0.0f, 0.2f);
            glVertex3f(-0.3f, 0.0f, 0.2f);
            glEnd();

            glPopMatrix();


        }

        else if (i == NUM_SEGMENTS - 2) {

            double scale = 1.2;

            glPushMatrix();
            glTranslated(0, 0, -0.2);
            glRotated(45, 0, 0, 1);
            glScaled(scale, scale, scale);
            drawFin();
            glPopMatrix();

            glPushMatrix();
            glTranslated(0, 0, -0.2);
            glRotated(-45, 0, 0, 1);
            glScaled(scale, scale, scale);
            drawFin();
            glPopMatrix();

            glColor3d(0.0078, 0.6000, 0.1412); //body
            drawCylinderSegment(0.22f, 1.2);
        }

        else if (i == NUM_SEGMENTS - 1) { //head

            glPushMatrix();
            glTranslated(0.3, -0.75, 0.0);
            drawHead();
            glPopMatrix();


            //shhooting ball
            if (shooting) { //darshini
                glPushMatrix();
                glTranslatef(0.0f, 0.0f, 0.6f + ballZ);  // forward from mouth
                drawEnergyBall(ballScale);
                glPopMatrix();
            }

            // explosion effect
            if (exploding) {
                glPushMatrix();
                glTranslatef(0.0f, 0.0f, 6.0f);
                glColor4f(1.0f, 0.5f, 0.0f, 0.7f);  // fiery orange ?
                glutSolidSphere(0.5f + 0.5f * sin(explosionTime * 10), 30, 30);
                glPopMatrix();
            }
            // Ball is held in hand and manually movable -ajim
             if (!shooting && !exploding) {
                glPushMatrix();
                glTranslatef(energyBallX, energyBallY, energyBallZ);
                drawEnergyBall(ballScale);
                glPopMatrix();
             }


        }
        else {
            glPushMatrix(); //straight line up
            glTranslated(0.0, -0.75, 0.58);
            drawStraightStripe(1.0f, 0.7f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval up
            glTranslated(0.0, 0.20, 0.42);
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //straaight line down
            glTranslated(0.0, -1.26, 0.58);
            drawStraightStripe(1.0f, 0.6f, 0.03f, 90.0f);//radius,length,stripeWidth,angleDegree
            glPopMatrix();

            glPushMatrix(); //horizontal oval down
            glTranslated(0.0, -0.27, 0.42);
            drawHorizontalStripe(0.06f, 0.17f, 0.03f, 0.03f); //x,y,z,stripeWidth
            glPopMatrix();

            glPushMatrix(); //verticle circle 
            glTranslated(0.0, 0.0, 0.23);
            drawStripeOnSegment(0.22f, 0.05f);
            glPopMatrix();

            glPushMatrix();
            glTranslated(0, 0, -0.5);
            glPopMatrix();

            glColor3d(0.0078, 0.6000, 0.1412); //body
            drawCylinderSegment(0.22f, 1.2);

        }

        glTranslatef(0.0f, 0.0f, segmentLength);       // move forward to next segment
    }

    glPopMatrix();
}

void drawSun() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureId1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glColor3f(1.0f, 0.84f, 0.0f);
    glPushMatrix();
    glTranslatef(9.0f, 10.0f, 5.0f);
    glRotatef(30, 0.0f, 1.0f, 0.0f);
    // Draw sphere 
    glutSolidSphere(1.0f, 20, 20);
    glPopMatrix();
}

void drawCloud(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor4f(1.0f, 1.0f, 1.0f, 0.85f); // white, slightly transparent

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Main big sphere
    glPushMatrix();
    glutSolidSphere(0.5, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, 0.2f, -0.5f);
    glutSolidSphere(0.2, 30, 30);
    glPopMatrix();

    // Side spheres
    glPushMatrix();
    glTranslatef(0.8f, 0.2f, 0.0f);
    glutSolidSphere(0.4, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.8f, 0.2f, 0.0f);
    glutSolidSphere(0.4, 30, 30);
    glPopMatrix();

    // Top spheres
    glPushMatrix();
    glTranslatef(0.4f, 0.6f, 0.0f);
    glutSolidSphere(0.3, 30, 30);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.4f, 0.6f, 0.0f);
    glutSolidSphere(0.4, 30, 30);
    glPopMatrix();

    // Disable blending after use
    glDisable(GL_BLEND);

    glPopMatrix();
}



void computePos(float deltaMove) {
    x += deltaMove * lx * 0.1f;
    z += deltaMove * lz * 0.1f;
}

void computeMovingPos(float move) {

}

void computeUpDown(float deltaY) {
    y += deltaY * ly * 0.6f;
}


void pressKey(int key, int xx, int yy) { //control camera view
    switch (key) {
    case GLUT_KEY_UP:
        //deltaMove = 0.1f;
        deltaMove = 0.2f;
        break;
    case GLUT_KEY_DOWN:
        //deltaMove = -0.1f;
        deltaMove = -0.2f;
        break;
    case GLUT_KEY_LEFT:
        //deltaAngleLR = -0.005f;
        deltaAngleLR = -0.05f;
        break;
    case GLUT_KEY_RIGHT:
        //deltaAngleLR = 0.005f;
        deltaAngleLR = 0.05f;
        break;
    case GLUT_KEY_PAGE_UP:
        //deltaY -= 0.03f;
        deltaY -= 0.1f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        //deltaY += 0.03f;
        deltaY += 0.4f;
        break;
    }
}

void releaseKey(int key, int x, int y) { //control camera view
    switch (key) {
    case GLUT_KEY_UP:
    case GLUT_KEY_DOWN:
        deltaMove = 0;
        break;
    case GLUT_KEY_LEFT:
    case GLUT_KEY_RIGHT:
        deltaAngleLR = 0.0f;
        break;
    case GLUT_KEY_PAGE_UP:
    case GLUT_KEY_PAGE_DOWN:
        deltaY = 0.0f;
        break;
    }
}

void mouseMove(int x, int y) { //comtrol mouse
    if (xOrigin >= 0) {
        deltaANGLE = (x - xOrigin) * 0.001f;
        deltaPitch = (yOrigin - y) * 0.001f;  // Inverted so moving mouse up looks up

        float totalAngle = angle + deltaANGLE;
        float totalPitch = pitch + deltaPitch;

        // Clamp pitch to avoid flipping (e.g., -89� to +89�)
        if (totalPitch > 1.5f) totalPitch = 1.5f;
        if (totalPitch < -1.5f) totalPitch = -1.5f;

        lx = cos(totalPitch) * sin(totalAngle);
        ly = sin(totalPitch);
        lz = -cos(totalPitch) * cos(totalAngle);
    }
}

void mouseButton(int button, int state, int x, int y) { //control mouse
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_UP) {
            angle += deltaANGLE;
            pitch += deltaPitch;

            // Clamp final pitch again
            if (pitch > 1.5f) pitch = 1.5f;
            if (pitch < -1.5f) pitch = -1.5f;

            xOrigin = -1;
            yOrigin = -1;
        }
        else {
            xOrigin = x;
            yOrigin = y;
        }
    }
}

void keyboard(unsigned char key, int x, int y) //control body move
{
    switch (key) {
    case 'e': moveUp = true; break;
    case 'd': moveDown = true; break;
    case 's': moveLeft = true; break;
    case 'f': moveRight = true; break;
    case 'r': resetBody = true; break;

    case 'i': forwardFront = true; break;
    case 'k': forwardBack = true; break;
    case 'j': forwardLeft = true; break;
    case 'l': forwardRight = true; break;
    case 'u': forwardUp = true; break;
    case 'h': forwardDown = true; break;
    case 'R': resetLocation = true; break;


    case 'z': //darshini
        if (!shooting) {
            shooting = true;
            ballZ = 0.3f;
            ballScale = 0.15f;
        }
        break;

        // ajim

    case 27: exit(0); break;
    case 'b':  // Toggle ball control mode
        controlBall = !controlBall;
        break;

    case 'w':  // Move ball forward (Z+)
        if (controlBall) energyBallZ += 0.1f;
        break;
    case 'W':  // Move ball backward (Z-)
        if (controlBall) energyBallZ -= 0.1f;
        break;
    case 'a':  // Move ball left (X-)
        if (controlBall) energyBallX -= 0.1f;
        break;
    case 'A':  // Move ball right (X+)
        if (controlBall) energyBallX += 0.1f;
        break;
    case 'q':  // Move ball up (Y+)
        if (controlBall) energyBallY += 0.1f;
        break;
    case 'Q':  // Move ball down (Y-)
        if (controlBall) energyBallY -= 0.1f;
        break;

    }
}


void keyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
    case 'e': moveUp = false; break;
    case 'd': moveDown = false; break;
    case 's': moveLeft = false; break;
    case 'f': moveRight = false; break;
    case'r':resetBody = false; break;

    case 'i': forwardFront = false; break;
    case 'k': forwardBack = false; break;
    case 'j': forwardLeft = false; break;
    case 'l': forwardRight = false; break;
    case 'u': forwardUp = false; break;
    case 'h': forwardDown = false; break;
    case 'R': resetLocation = false; break;
    }
}

void updateBodyWave(float angleDelta, char direction) { //e,s,d,f (for rotating) 
    // Adjust head rotation based on direction
    if (direction == 's') {// turn left
        if (yawSegments[NUM_SEGMENTS - 1] > -50) {
            yawSegments[NUM_SEGMENTS - 1] -= angleDelta;

        }

    }
    else if (direction == 'f') {// turn right
        if (yawSegments[NUM_SEGMENTS - 1] < 50) {
            yawSegments[NUM_SEGMENTS - 1] += angleDelta;
        }
    }
    else if (direction == 'e') // look up
    {
        if (pitchSegments[NUM_SEGMENTS - 1] > -50) {
            pitchSegments[NUM_SEGMENTS - 1] -= angleDelta;
        }
    }
    else if (direction == 'd') // look down
    {
        if (pitchSegments[NUM_SEGMENTS - 1] < 50) {
            pitchSegments[NUM_SEGMENTS - 1] += angleDelta;
            cout << pitchSegments[NUM_SEGMENTS - 1];
        }
    }
    else if (direction == 'r') {
        for (int i = NUM_SEGMENTS - 1; i >= 0; i--) {
            yawSegments[i] = 0;
            pitchSegments[i] = 0;
        }
        return;
    } //reset body position

    // Smoothly update rest of body
    for (int i = NUM_SEGMENTS - 2; i >= 0; i--) {
        yawSegments[i] += 0.08f * (yawSegments[i + 1] - yawSegments[i]);
        pitchSegments[i] += 0.08f * (pitchSegments[i + 1] - pitchSegments[i]);
    }
}

void updateBodyLocation(float moveLocation, char direction) { // i,j,k,,l (for translating)
    // Adjust head rotation based on direction
    if (direction == 'i') // move forward
        moveZ[NUM_SEGMENTS - 1] += moveLocation;
    else if (direction == 'k') // move backward
        moveZ[NUM_SEGMENTS - 1] -= moveLocation;
    else if (direction == 'j') // move left
        moveX[NUM_SEGMENTS - 1] -= moveLocation;
    else if (direction == 'l') // move right
        moveX[NUM_SEGMENTS - 1] += moveLocation;
    else if (direction == 'u') // move up
        moveY[NUM_SEGMENTS - 1] += moveLocation;
    else if (direction == 'h') // move down
        moveY[NUM_SEGMENTS - 1] -= moveLocation;
    else if (direction == 'R') {
        for (int i = NUM_SEGMENTS - 1; i >= 0; i--) {
            moveX[i] = 0;
            moveY[i] = 0;
            moveZ[i] = 0;
        }
        return;
    } //reset body position

     // Smoothly update the rest of the body position
    for (int i = NUM_SEGMENTS - 2; i >= 0; i--) {
        moveX[i] += 0.08f * (moveX[i + 1] - moveX[i]);
        moveY[i] += 0.08f * (moveY[i + 1] - moveY[i]);
        moveZ[i] += 0.08f * (moveZ[i + 1] - moveZ[i]);
    }
}


void keyboardRayquaza(unsigned char key, int x, int y)
{
    switch (key) {
    case 'e': //rotate up
        updateBodyWave(5.0f, 'e');
        glutPostRedisplay();
        break;

    case 'd': //rotate down
        updateBodyWave(5.0f, 'd');
        glutPostRedisplay();
        break;

    case 's': //rotate left
        updateBodyWave(5.0f, 's');
        glutPostRedisplay();
        break;

    case 'i': //move forward
        updateBodyLocation(3.0f, 'i');
        glutPostRedisplay();
        break;

    case 'k': //move backward
        updateBodyLocation(3.0f, 'k');
        glutPostRedisplay();
        break;

    case 'l': //move right
        updateBodyLocation(3.0f, 'l');
        glutPostRedisplay();
        break;
    case 'j': //move left
        updateBodyLocation(3.0f, 'j');
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }

    if (key == 27)
        exit(0);
}

void update(int value)
{
    if (moveUp)
        updateBodyWave(2.0f, 'e');
    if (moveDown)
        updateBodyWave(2.0f, 'd');
    if (moveLeft)
        updateBodyWave(2.0f, 's');
    if (moveRight)
        updateBodyWave(2.0f, 'f');

    if (forwardFront)
        updateBodyLocation(0.5f, 'i');
    if (forwardBack)
        updateBodyLocation(0.5f, 'k');
    if (forwardUp)
        updateBodyLocation(0.5f, 'u');
    if (forwardDown)
        updateBodyLocation(0.5f, 'h');
    if (forwardLeft)
        updateBodyLocation(0.5f, 'j');
    if (forwardRight)
        updateBodyLocation(0.5f, 'l');

    if (resetBody)
        updateBodyWave(0.0f, 'r');

    if (shooting) { //darshini
        ballZ += 0.1f; //speed of the ball
        ballScale += 0.005f;// make it grow as it explodes
        ball += 0.1f;
        glScalef(ballScale, ballScale, ball);
        glutSolidSphere(1.0f, 20, 20);

        if (ballZ > 6.0f) {
            shooting = false;
            exploding = true;
            explosionTime = 0.0f;
            ballZ = 0.0f;
            ballScale = 0.1f;
        }
    }

    if (exploding) { //darshini
        explosionTime += 0.05f;
        if (explosionTime > 1.0f) {
            exploding = false;
            energyBallX = 0.0f; //ajim
            energyBallY = 0.0f;
            energyBallZ = 0.6f;

        }
    }

    //update cloud  position
    cloudOffSet += 0.005f;
    if (cloudOffSet > 100.0f) cloudOffSet = -100.0f;


    //origin
    glutPostRedisplay(); // redraw the screen
    glutTimerFunc(16, update, 0); // roughly 60 FPS
}

void setupClouds() {
    for (int i = 0; i < NUM_CLOUDS; i++) {
        cloudPosition[i][0] = (rand() % 200 - 100) / 10;
        cloudPosition[i][1] = (rand() % 50 + 20) / 10;
        cloudPosition[i][2] = (rand() % 200 - 100) / 10;
    }
}

void display(void) { //equivalant to renderScene
    if (deltaMove != 0)
        computePos(deltaMove);
    computeUpDown(deltaY);

    if (deltaAngleLR != 0.0f) {
        angle += deltaAngleLR;
        lx = sin(angle);
        lz = -cos(angle);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(x, y, z,
        x + lx, y + ly, z + lz,
        0.0f, 1.0f, 0.0f);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos); // Make sure it's updated each frame

    //Draw ground plane
    //glColor3f(0.9f, 0.9f, 0.9f);
    glColor3d(0.56f, 0.93f, 0.56f); //dirt color
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, -5.0f, -100.0f);
    glVertex3f(-100.0f, -5.0f, 100.0f);
    glVertex3f(100.0f, -5.0f, 100.0f);
    glVertex3f(100.0f, -5.0f, -100.0f);
    glEnd();

    glPushMatrix();
    glTranslated(0, 0, -3);
    drawRayquaza();
    glPopMatrix();
    drawSun();
    for (int i = 0; i < NUM_CLOUDS; i++) {
        drawCloud(cloudPosition[i][0] + cloudOffSet + 5.0, cloudPosition[i][1] + 5.0, cloudPosition[i][2] + 5.0);
    }


    glutSwapBuffers();
}


void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);

    Image* image1 = loadBMP("C:\\audio\\bmp6.bmp");
    _textureId1 = loadTexture(image1);
    delete image1;

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    // Optional: Light 1 if you want a second light
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

    // Set clear color to sky blue
    glClearColor(0.584f, 0.961f, 1.0f, 1.0f);

    // Set perspective
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.33, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Rayquaza Mdeol");

    init();
    srand(time(NULL));
    setupClouds();
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0); //for body movement    
    glutReshapeFunc(reshape);
    glutIdleFunc(display); //added

    glutIgnoreKeyRepeat(1); //added
    //glutKeyboardFunc(keyboardRayquaza);
    glutSpecialFunc(pressKey); //added
    glutSpecialUpFunc(releaseKey); //added


    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);

    glutMouseFunc(mouseButton); //added
    glutMotionFunc(mouseMove); //added

    glEnable(GL_DEPTH_TEST);

    glutMainLoop();
    return 0;
}