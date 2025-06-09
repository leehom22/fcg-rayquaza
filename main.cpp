#include <GL/glut.h>
#include <cmath>
#include "drawStripeOnSegment.h"
#include "drawHorizontalStripe.h"

#define NUM_SEGMENTS 6
#define M_PI 3.14159265359

const int SEGMENTS = 20;       // More segments = smoother S
const float SEG_LENGTH = 1.0f; // Length of each segment
const float SEG_RADIUS = 0.5f; // Radius of each segment
const float ROTATE_LENGTH = 0.01f;
static int segment1 = 0, segment2 = 0, segment3 = 0, segment4 = 0, segment5 = 0, segment6 = 0, segment7 = 0;
static int head = 0, tail = 0;

float angle = 0.0f;

float lx = 0.0f, lz = -1.0f, ly = 0.1f;

float x = 0.0f, z = 5.0f, y = 0.0f;

float deltaANGLE = 0.0F;
float deltaMove = 0;
float deltaAngleLR = 0.0f;
float deltaY = 0.0f;

float pitch = 0.0f;
float deltaPitch = 0.0f;

int xOrigin = -1;
int yOrigin = -1;

float segments[NUM_SEGMENTS] = { 0, 0, 0, 0 }; // from tail (0) to head (3)
const float segmentLength = 0.01f; // adjust this based on your model

float yawSegments[NUM_SEGMENTS];
float pitchSegments[NUM_SEGMENTS];

bool moveUp = false;
bool moveDown = false;
bool moveLeft = false;
bool moveRight = false;
bool resetBody = false;

//darshini
float energyOffset = 0.0f;
float ballZ = 0.0f;
float ballScale = 0.1;
float explosionTime = 0.0f;
float ball = 0.0f;

//darshini
bool shooting = false; // to make the energy ball shoot
bool exploding = false;

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



void drawRayquaza() {
    glPushMatrix();
    glTranslated(0, 2.0, 0);

    for (int i = 0; i < NUM_SEGMENTS; i++) {
        glColor3d(0.0078, 0.6000, 0.1412);//green
        glRotatef(yawSegments[i], 0.0f, 1.0f, 0.0f);
        glRotatef(pitchSegments[i], 1.0f, 0.0f, 0.0f); // rotate segment

        if (i == 0) {

            drawCylinderSegment(0.1f, 1.0f);
        }
        else if (i == 1) {

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

            // Fins (left and right) //azim
            glPushMatrix();
            glTranslated(0.0, 0.0, 0.3);  // adjust position along the segment
            glColor3d(1.0, 0.0, 0.0);     // red fins

            // Right fin //azim
            glPushMatrix();
            glTranslated(0.1, 0.0, 0.0);
            glRotated(45, 0, 1, 0);
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.2f, 0.1f, 0.0f);
            glVertex3f(0.2f, -0.1f, 0.0f);
            glEnd();
            glPopMatrix();

            // Left fin //azim
            glPushMatrix();
            glTranslated(-0.1, 0.0, 0.0);
            glRotated(-45, 0, 1, 0);
            glBegin(GL_TRIANGLES);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(-0.2f, 0.1f, 0.0f);
            glVertex3f(-0.2f, -0.1f, 0.0f);
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

        else if (i == NUM_SEGMENTS - 1) { //head
            
              glPushMatrix();
            glTranslated(0.3, -0.75, 0.0);
            drawHead();
            glPopMatrix();
            

            //drawHead();

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

            glColor3d(0.0078, 0.6000, 0.1412); //body
            drawCylinderSegment(0.22f, 1.2);

        }

        glTranslatef(0.0f, 0.0f, segmentLength);       // move forward to next segment
    }

    glPopMatrix();
}


void computePos(float deltaMove) {
    x += deltaMove * lx * 0.1f;
    z += deltaMove * lz * 0.1f;
}

void computeUpDown(float deltaY) {
    y += deltaY * ly * 0.6f;
}


void pressKey(int key, int xx, int yy) { //control camera view
    switch (key) {
    case GLUT_KEY_UP:
        deltaMove = 0.1f;
        break;
    case GLUT_KEY_DOWN:
        deltaMove = -0.1f;
        break;
    case GLUT_KEY_LEFT:
        deltaAngleLR = -0.005f;
        break;
    case GLUT_KEY_RIGHT:
        deltaAngleLR = 0.005f;
        break;
    case GLUT_KEY_PAGE_UP:
        deltaY -= 0.03f;
        break;
    case GLUT_KEY_PAGE_DOWN:
        deltaY += 0.03f;
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

        // Clamp pitch to avoid flipping (e.g., -89° to +89°)
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

void keyboard(unsigned char key, int x, int y) 
{
    switch (key) {
    case 'e': moveUp = true; break;
    case 'd': moveDown = true; break;
    case 's': moveLeft = true; break;
    case 'f': moveRight = true; break;
    case 'r': resetBody = true; break;
    case 'z': //darshini
        if (!shooting) {
            shooting = true;
            ballZ = 0.3f;
            ballScale = 0.15f;
        }
        break;
    case 27: exit(0); break;
    }
}

/*
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'e': moveUp = true; break;
    case 'd': moveDown = true; break;
    case 's': moveLeft = true; break;
    case 'f': moveRight = true; break;
    case 'r': resetBody = true; break;
    case 27: exit(0); break;
    }
}
*/


void keyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
    case 'e': moveUp = false; break;
    case 'd': moveDown = false; break;
    case 's': moveLeft = false; break;
    case 'f': moveRight = false; break;
    case'r':resetBody = false; break;
    }
}

void updateBodyWave(float angleDelta, char direction) {
    // Adjust head rotation based on direction
    if (direction == 's') // turn left
        yawSegments[NUM_SEGMENTS - 1] -= angleDelta;
    else if (direction == 'f') // turn right
        yawSegments[NUM_SEGMENTS - 1] += angleDelta;
    else if (direction == 'e') // look up
        pitchSegments[NUM_SEGMENTS - 1] -= angleDelta;
    else if (direction == 'd') // look down
        pitchSegments[NUM_SEGMENTS - 1] += angleDelta;
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

void keyboardRayquaza(unsigned char key, int x, int y)
{
    switch (key) {
    case 'e': //up
        updateBodyWave(5.0f, 'e');
        glutPostRedisplay();
        break;

    case 'd': //down
        updateBodyWave(5.0f, 'd');
        glutPostRedisplay();
        break;

    case 's': //left
        updateBodyWave(5.0f, 's');
        glutPostRedisplay();
        break;

    case 'f': //right
        updateBodyWave(5.0f, 'f');
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
        }
    }

    //origin
    glutPostRedisplay(); // redraw the screen
    glutTimerFunc(16, update, 0); // roughly 60 FPS
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


    //Draw ground plane
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();

    drawRayquaza();

    glutSwapBuffers();
}


void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45, 1.33, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Rayquaza Mdeol");

    init();
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