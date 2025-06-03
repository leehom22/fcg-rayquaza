#include <GL/glut.h>
#include <cmath>
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
        float stripeY2 = y + stripeHeight;

        glVertex3f(x, stripeY1, z1);  // lower edge of stripe at z1
        glVertex3f(x, stripeY1, z2);  // lower edge of stripe at z2
    }
    glEnd();
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


void drawCylinderSegment(float radius_, float length_) {
     
    float seg_radius = radius_;
    float seg_length = length_;

    if (radius_ == 0||length_==0) {
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
/*
void drawHead() {//Segment: radius= 0.5f  ; length= 1.0f
    //Head: radius=0.5f ; length=1.3f

    glBegin(GL_QUADS);

    glColor3d(0.5, 0.241, 0.51);
    glVertex3f(0.25f,1.1f,1.0f); //top
    glVertex3f(0, 1.3f, 0);
    glVertex3f(0.5f, 1.3f, 0);

    glVertex3f(0.5f, 1.2f, 0); //right
    glVertex3f(0.25f, 0.55f, 1.0f);
    glVertex3f(0.25f, 1.1f, 1.0f);
    glVertex3f(0.5f, 1.3f, 0);
    glVertex3f(0.5f, 1.2f, 0);

    glVertex3f(0.25f, 0.55f, 1.0f); //left
    glVertex3f(0, 1.2f, 0);
    glVertex3f(0, 1.3f, 0);
    glVertex3f(0.25f, 1.1f, 1.0f);
    glVertex3f(0.25f, 0.55f, 1.0f);

    glVertex3f(0.25f, 0.55f, 1.0f); //bottom
    glVertex3f(0, 1.2f, 0);
    glVertex3f(0.5f, 1.2f, 0);
    glVertex3f(0.25f, 0.55f, 1.0f);

    glEnd();
}
*/

/*
void drawRayquaza() {
    lukisPaksi(); // draw axis
    //drawHead();

    glColor3d(0.0745, 0.6863, 0.2118); // green color
    glPushMatrix();
    glTranslatef(0, 2, -1.0); // start position

    // Segment 1
    glPushMatrix(); //tail
    glRotatef((GLfloat)segment1, 0.0, 0.0, 1.0);
    drawCylinderSegment(0.5f,1.0f);

    // Segment 2
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix();
    glRotatef((GLfloat)segment2, 0.0, 1.0, 0.0);
    drawCylinderSegment(0.5f,1.0f);

    // Segment 3
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix();
    glRotatef((GLfloat)segment3, 1.0, 0.0, 0.0);
    drawCylinderSegment(0.5f,1.0f);

    // Segment 4
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix();
    glRotatef((GLfloat)segment4, 1.0, 0.0, 0.0);
    drawCylinderSegment(0.5f,1.0f);

    // Segment 5
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix();
    glRotatef((GLfloat)segment5, 1.0, 0.0, 0.0);
    drawCylinderSegment(0.5f, 1.0f);

    // Segment 6
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix();
    glRotatef((GLfloat)segment6, 1.0, 0.0, 0.0);
    drawCylinderSegment(0.5f, 1.0f);

    // Segment 7
    glTranslatef(0, 0, ROTATE_LENGTH);
    glPushMatrix(); //head
    glRotatef((GLfloat)segment7, 1.0, 0.0, 0.0);
    drawCylinderSegment(0.5f, 1.0f);

    // Pop matrices in reverse order
    glPopMatrix(); // seg 7
    glPopMatrix(); // seg 6
    glPopMatrix(); // seg 5
    glPopMatrix(); // seg 4
    glPopMatrix(); // seg 3
    glPopMatrix(); // seg 2
    glPopMatrix(); // seg 1

    glPopMatrix(); // base translate
}

*/ //drawRayquaza v1

    void drawEye(float radiusX, float radiusZ, float height, float angleDeg, float verticalFactor) {
        float angleRad = angleDeg * M_PI / 180.0f;

        // Base center position of the eye
        float cx = radiusX * cos(angleRad);
        float cz = radiusZ * sin(angleRad);
        float cy = height * verticalFactor;

        // Normal vector in the tangent direction (perpendicular to radius vector)
        float nx = -sin(angleRad);  // points along surface tangent (around cone)
        float nz = cos(angleRad);

        // Eye size controls
        float width = 0.1f;   // horizontal base
        float heightEye = 0.1f; // vertical height

        // Calculate triangle vertices
        //Bottom center 
        float ex1 = cx; 
        float ey1 = cy;
        float ez1 = cz;

        //Upper right 
        float ex2 = cx + width * nx;
        float ey2 = cy + heightEye;
        float ez2 = cz + width * nz;

        //Upper left
        float ex3 = cx - width * nx;
        float ey3 = cy + heightEye;
        float ez3 = cz - width * nz;

        // Draw the triangle
        glColor3f(0.0, 0.0, 0.0); // black eye
        glBegin(GL_TRIANGLES);
        glVertex3f(ex1, ey1, ez1); // bottom point
        glVertex3f(ex2, ey2, ez2); // upper right
        glVertex3f(ex3, ey3, ez3); // upper left
        glEnd();
    }


void drawHead(double baseRadiusUp, double baseRadiusDown, double height, double slices) {

    double angleStep = M_PI / slices;  // PI = 180 degrees

    double angle = 0;
    double y = 0;
    double z = 0;

    double initAngle = 0;
    double initY = 0;
    double initZ = 0;

    double halfAngle = 0;
    double halfY = 0;
    double halfZ = 0;
   
    double radiusX = baseRadiusUp*0.6;
    double radiusZ = baseRadiusUp;

    drawEye(radiusX, radiusZ-0.3f, height, 63.0f, 0.3f); //change the position of the eyes (left/right, up/down)
    drawEye(radiusX, radiusZ-0.3f, height, 117.0f, 0.3f);

   
    glBegin(GL_TRIANGLES);
    glColor3d(0.0745, 0.6863, 0.2118); //green color same as body 
    for (int i = 0; i < slices; ++i) {
        double angle1 = i * angleStep;
        double angle2 = (i + 1) * angleStep;

        double x1 = radiusX * cos(angle1);
        double z1 = radiusZ * sin(angle1);
        double x2 = radiusX * cos(angle2);
        double z2 = radiusZ * sin(angle2);

        // Triangle from base edge to apex
        glVertex3d(x1, 0.0, z1);           // base point 1
        glVertex3d(x2, 0.0, z2);           // base point 2
        glVertex3d(0.0, height, 0.0);      // apex
    }
    glEnd();
   

    double radiusXDown = baseRadiusDown * 0.6;
    double radiusZDown = baseRadiusDown;

    glBegin(GL_TRIANGLES);
    //glColor3f(1.0, 0.0, 0.0); // red sides
    for (int i = 0; i < slices; ++i) {
        double angle1 = i * angleStep;
        double angle2 = (i + 1) * angleStep;

        double x1 = radiusXDown * cos(angle1);
        double z1 = radiusZDown * sin(angle1);
        double x2 = radiusXDown * cos(angle2);
        double z2 = radiusZDown * sin(angle2);

        // Triangle from base edge to apex
        glVertex3d(x1, 0.0, z1);           // base point 1
        glVertex3d(x2, 0.0, z2);           // base point 2
        glVertex3d(0.0, -height, 0.0);      // apex
    }
    glEnd();
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
            drawHorizontalStripe(0.06f,0.17f, 0.03f,0.03f); //x,y,z,stripeWidth
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

        }else if (i == NUM_SEGMENTS - 1) {

            drawHead(0.45, 0.40, 0.3, 50.0);
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
    case 27: exit(0); break;
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
        updateBodyWave(5.0f,'e');
        glutPostRedisplay();
        break;
   
    case 'd': //down
        updateBodyWave(5.0f,'d');
        glutPostRedisplay();
        break;
   
    case 's': //left
        updateBodyWave(5.0f,'s');
        glutPostRedisplay();
        break;
   
    case 'f': //right
        updateBodyWave(5.0f,'f');
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




/*
#include <GL/glut.h>
#include <cmath>

const int SEGMENTS = 20;
const float SEG_LENGTH = 0.5f;
const float SEG_RADIUS = 0.2f;

float timeAngle = 0.0f; // Time for animation

void drawCylinderSegment() {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, SEG_RADIUS, SEG_RADIUS, SEG_LENGTH, 20, 5);

    // Optional caps
    gluDisk(quad, 0, SEG_RADIUS, 20, 1);
    glTranslatef(0, 0, SEG_LENGTH);
    gluDisk(quad, 0, SEG_RADIUS, 20, 1);

    gluDeleteQuadric(quad);
    glTranslatef(0, 0, -SEG_LENGTH); // Reset position
}

void drawTrunk(float timeAngle) {
    for (int i = 0; i < SEGMENTS; i++) {
        glPushMatrix();

        // Animate bend using sine wave
        float angleY = sinf(timeAngle + i * 0.3f) * 15.0f;  // Horizontal wave
        float angleX = cosf(timeAngle + i * 0.3f) * 5.0f;   // Vertical wave

        glRotatef(angleX, 1, 0, 0);
        glRotatef(angleY, 0, 1, 0);

        drawCylinderSegment();

        // Move forward to next segment
        glTranslatef(0, 0, SEG_LENGTH);
        glPopMatrix();

        // Move base position for next segment
        glTranslatef(0, 0, SEG_LENGTH);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(5, 5, 15, 0, 0, 0, 0, 1, 0);

    glColor3f(0.0, 0.8, 0.2); // Rayquaza green

    glPushMatrix();
    drawTrunk(timeAngle);
    glPopMatrix();

    glutSwapBuffers();
}

void update(int value) {
    timeAngle += 0.05f;  // Increment time for wave animation

    glutPostRedisplay();            // Request redraw
    glutTimerFunc(16, update, 0);   // Schedule next update (60 FPS)
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(1, 1, 1, 1); // White background
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45, 1.33, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Animated Rayquaza Trunk");
    init();
    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0); // Start animation timer
    glutMainLoop();
    return 0;
}
*/