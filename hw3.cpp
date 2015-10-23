// HW3 for CS 637
// Shangqi Wu

#include "Angel.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>

using namespace std;

#define PI 3.14159265

typedef vec4 color4;

// Height and width of main window. 
const int h = 500;
const int w = 500;

// Numbers of points for different objects. 
const int TriPoints = 3;
const int QuadPoints = 4;
const int CirPoints = 500;
const int EllPoints = 500;

// Stores position & color for left click circles. 
vector<vec2> main_ver;
vector<color4> main_col;

// RGBA color for background of main window.
float Red = 0;
float Green = 0;
float Blue = 0;
float Alpha = 1;

// RGB color for squares in main window. 
float RedSquare = 1;
float GreenSquare = 1;
float BlueSquare = 1;
float AlphaSquare = 1;

// RGBA color for background of window2.
float Red2 = 0;
float Green2 = 0;
float Blue2 = 0;
float Alpha2 = 1;

// RGBA color for background of subwindow. 
float subRed = 1;
float subGreen = 1;
float subBlue = 1;
float subAlpha = 1;

// Angle for all objects.
float t = 0;
// Increment of angle at a time. 
const float dt = 0.05;
// Radius for circles and ellipses. 
float radv = 0.25;
float rrange = 0.1;

// IDs for 3 windows. 
int MainWindow, MainWindow2, SubWindow;

//--------------------------------------------------------------------------

void
init( void )
{
    // Reserve vertices & colors for squares. 
    vec2 quad_ver[6][QuadPoints];
    color4 quad_col[6][QuadPoints];
    
    // Create a vertex array object.
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    cout<<"glGenVertexArrays(), glBindVertexArray() for main window initialization."<<endl;

    // Create and initialize a buffer object.
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    cout<<"glGenBuffer(), glBindBuffer() for main window initialization."<<endl;
    glBufferData( GL_ARRAY_BUFFER, sizeof(quad_ver)+sizeof(quad_col)+main_ver.size()*CirPoints*sizeof(vec2)+main_col.size()*CirPoints*sizeof(color4), NULL, GL_STATIC_DRAW );
    cout<<"glBufferData(), glBufferSubData() for main window initialization."<<endl;

    // Load shaders and use the resulting shader program.
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    cout<<"InitShader(), glUseProgram() for main window initialization."<<endl;

    // Initialize the vertex position attribute from the vertex shader.
    GLuint loc_ver = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc_ver );
    glVertexAttribPointer( loc_ver, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint loc_col = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( loc_col );
    glVertexAttribPointer( loc_col, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(quad_ver)+main_ver.size()*CirPoints*sizeof(vec2)) );
    cout<<"glEnableVertexAttribArray(), glVertexAttribPointer() for main window initialization."<<endl;
}

//----------------------------------------------------------------------------

void
recal( void )
{
    // Specify vertices & colors for squares in the main window. 
    vec2 quad_ver[6][QuadPoints];
    color4 quad_col[6][QuadPoints];
    for (int i = 0; i < 6; i++) {
        quad_ver[i][0] = vec2( (0.6-0.1*i)*cos(t+PI/4),   -0.3+(0.6-0.1*i)*sin(t+PI/4)   );
        quad_ver[i][1] = vec2( (0.6-0.1*i)*cos(t+3*PI/4), -0.3+(0.6-0.1*i)*sin(t+3*PI/4) );
        quad_ver[i][2] = vec2( (0.6-0.1*i)*cos(t+7*PI/4), -0.3+(0.6-0.1*i)*sin(t+7*PI/4) );
        quad_ver[i][3] = vec2( (0.6-0.1*i)*cos(t+5*PI/4), -0.3+(0.6-0.1*i)*sin(t+5*PI/4) );
        if (i % 2 == 0) 
            for (int j = 0; j < QuadPoints; j++)
                quad_col[i][j] = color4( RedSquare, GreenSquare, BlueSquare, AlphaSquare ); 
        else
            for (int j = 0; j < QuadPoints; j++)
                quad_col[i][j] = color4( 0, 0, 0, 1 ); 
    }

    // Specify vertices & colors for circles in the main window. 
    vec2 (*cir_ver_main)[CirPoints] = new vec2[main_ver.size()][CirPoints];
    color4 (*cir_col_main)[CirPoints] = new color4[main_col.size()][CirPoints];
    for (int i = 0; i < (int)main_ver.size(); i++)
        for (int j = 0; j < CirPoints; j++) {
            cir_ver_main[i][j] = vec2( main_ver[i].x+(0.1+0.05*cos(t))*cos(j*2*PI/CirPoints), main_ver[i].y+(0.1+0.05*cos(t))*sin(j*2*PI/CirPoints) );
            cir_col_main[i][j] = color4( main_col[i].x*(float)j/CirPoints, main_col[i].y*(float)j/CirPoints, main_col[i].z*(float)j/CirPoints, 1 );
        }

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(quad_ver), quad_ver );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(quad_ver), main_ver.size()*CirPoints*sizeof(vec2), cir_ver_main );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(quad_ver)+main_ver.size()*CirPoints*sizeof(vec2), sizeof(quad_col), quad_col );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(quad_ver)+main_ver.size()*CirPoints*sizeof(vec2)+sizeof(quad_col), main_col.size()*CirPoints*sizeof(color4), cir_col_main );

    delete []cir_ver_main;
    delete []cir_col_main;
}

//----------------------------------------------------------------------------
void
init2( void )
{
    // Reserve vertices & colors for the triangle.
    vec2 tri_ver[TriPoints];
    color4 tri_col[TriPoints];

    // Reserve the vertices & colors for the circle.
    vec2 cir_ver[CirPoints];
    color4 cir_col[CirPoints]; 

    // Create vertex array object.
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    cout<<"glGenVertexArrays(), glBindVertexArray() initializatin."<<endl;

    // Create and initialize a buffer object.
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    cout<<"glGenBuffer(), glBindBuffer() initialization."<<endl;
    glBufferData( GL_ARRAY_BUFFER, sizeof(tri_ver)+sizeof(cir_ver)+sizeof(tri_col)+sizeof(cir_col), NULL, GL_STATIC_DRAW );
    cout<<"glBufferData(), glBufferSubData() initialization."<<endl;

    // Load shaders and use the resulting shader program.
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    cout<<"InitShader(), glUseProgram() for window2 initialization."<<endl;

    // Initialize the vertex position attribute from the vertex shader.
    GLuint loc_ver = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc_ver );
    glVertexAttribPointer( loc_ver, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint loc_col = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( loc_col );
    glVertexAttribPointer( loc_col, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(tri_ver)+sizeof(cir_ver)) );
    cout<<"glEnableVertexAttribArray(), glVertexAttribPointer() for window2 initialization."<<endl;
}

//----------------------------------------------------------------------------
void
recal2( void )
{
    // Specify the vertices for the triangle.
    vec2 tri_ver[TriPoints] = {
        vec2( 0.3*sin(t+4*PI/3), 0.6+0.3*cos(t+4*PI/3) ), vec2( 0.3*sin(t), 0.6+0.3*cos(t) ), vec2( 0.3*sin(t+2*PI/3), 0.6+0.3*cos(t+2*PI/3) )
    };

    // Specify colors for vertices of triangle.
    color4 tri_col[TriPoints] = {
        color4( 0, 1, 0, 1 ), color4( 1, 0, 0, 1 ), color4( 0, 0, 1, 1 )
    };

    // Specify the vertices & colors for the circle.
    vec2 cir_ver[CirPoints];
    color4 cir_col[CirPoints];
    for (int i = 0; i < CirPoints; i++) {
        cir_ver[i] = vec2( (radv+rrange*cos(t))*cos(i*2*PI/EllPoints), -0.3+(radv+rrange*cos(t))*sin(i*2*PI/EllPoints) );
        cir_col[i] = color4( (float)i/(float)CirPoints, 0, 0, 1 );
    }

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(tri_ver), tri_ver );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(tri_ver), sizeof(cir_ver), cir_ver );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(tri_ver)+sizeof(cir_ver), sizeof(tri_col), tri_col );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(tri_ver)+sizeof(cir_ver)+sizeof(tri_col), sizeof(cir_col), cir_col );
}

//----------------------------------------------------------------------------
void
initsub( void )
{
    // Reserve the verices & colors for ellipse. 
    vec2 ell_ver[EllPoints];
    color4 ell_col[EllPoints];

    // Create a vertex array object.
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    cout<<"glGenVertexArrays(), glBindVertexArray() for subwindow initialization."<<endl;

    // Create and initialize a buffer object.
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    cout<<"glGenBuffer(), glBindBuffer() for subwindow initialization."<<endl;
    glBufferData( GL_ARRAY_BUFFER, sizeof(ell_ver)+sizeof(ell_col), NULL, GL_STATIC_DRAW );
    cout<<"glBufferData(), glBufferSubData() for subwindow initialization."<<endl;

    // Load shaders and use the resulting shader program.
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );
    cout<<"InitShader(), glUseProgram() for subwindow initialization."<<endl;

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc_ver = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc_ver );
    glVertexAttribPointer( loc_ver, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint loc_col = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( loc_col );
    glVertexAttribPointer( loc_col, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(ell_ver)) );
    cout<<"glEnableVertexAttribArray(), glVertexAttribPointer() for subwindow initialization."<<endl;
}

//----------------------------------------------------------------------------
void
recalsub( void )
{
    // Specify the verices & colors for ellipse. 
    vec2 ell_ver[EllPoints];
    color4 ell_col[EllPoints];
    for (int i = 0; i < EllPoints; i++) {
        ell_ver[i] = vec2( (0.7+rrange*cos(t))*cos(i*2*PI/EllPoints), (0.7+rrange*cos(t))*0.6*sin(i*2*PI/EllPoints) );
        ell_col[i] = color4( 0, (float)i/(float)EllPoints, (float)i/(float)EllPoints, 1 );
    }

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(ell_ver), ell_ver );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(ell_ver), sizeof(ell_col), ell_col );
}

//----------------------------------------------------------------------------

void
display( void )
{
    recal(); // Calculates vertices & colors for objects in main window. 
    glClearColor( Red, Green, Blue, Alpha ); // Set background color of main window.
    glClear( GL_COLOR_BUFFER_BIT );     // Clear main window.
    glDrawArrays( GL_TRIANGLE_STRIP, 0, QuadPoints );    // Draw the points by one square.
    glDrawArrays( GL_TRIANGLE_STRIP, QuadPoints, QuadPoints );
    glDrawArrays( GL_TRIANGLE_STRIP, 2*QuadPoints, QuadPoints );
    glDrawArrays( GL_TRIANGLE_STRIP, 3*QuadPoints, QuadPoints );
    glDrawArrays( GL_TRIANGLE_STRIP, 4*QuadPoints, QuadPoints );
    glDrawArrays( GL_TRIANGLE_STRIP, 5*QuadPoints, QuadPoints );
    for (int i = 0; i < (int)main_ver.size(); i++)
        glDrawArrays( GL_TRIANGLE_FAN, 6*QuadPoints+i*CirPoints, CirPoints );
    glutSwapBuffers(); // Double buffer swapping. 
    glFlush(); // Flush. 
    cout<<"glClearColor(), glClear(), glDrawArrays(), glutSwapBuffers(), glFlush() for main window display function."<<endl;
}

//----------------------------------------------------------------------------

void
display2( void )
{
    recal2(); // Calculates vertices & colors for objects in main window. 
    glClearColor( Red2, Green2, Blue2, Alpha2 ); // Set background color of window2.
    glClear( GL_COLOR_BUFFER_BIT ); // Clear window2. 
    glDrawArrays( GL_TRIANGLES, 0, TriPoints ); // Draw the points of the triangle.
    glDrawArrays( GL_TRIANGLE_FAN, TriPoints, CirPoints ); // Draw the points of the circle.
    glutSwapBuffers(); // Double buffer swapping.
    glFlush(); // Flush.
    cout<<"glClearColor(), glClear(), glDrawArrays(), glutSwapBuffers(), glFlush() for window2 display function."<<endl;
}

//----------------------------------------------------------------------------

void
subdisplay( void )
{
    recalsub(); // Calculates vertices & colors for objects in main window.
    glClearColor( subRed, subGreen, subBlue, subAlpha ); // Set background color of subwindow.
    glClear( GL_COLOR_BUFFER_BIT );  // Clear subwindow.
    glDrawArrays( GL_TRIANGLE_FAN, 0, EllPoints ); // Draw the points of the ellipse.
    glutSwapBuffers(); // Double buffer swapping.
    glFlush(); // Flush.
    cout<<"glClearColor(), glClear(), glDrawArrays(), glutSwapBuffers(), glFlush() for subwindow display function."<<endl;
}

//----------------------------------------------------------------------------

void
rotation( void )
{
    t += dt; // Increase t, and redraw modified objects. 
    glutSetWindow( MainWindow );
    glutPostRedisplay();
    glutSetWindow( MainWindow2 );
    glutPostRedisplay();
    glutSetWindow( SubWindow );
    glutPostRedisplay();
    cout<<"glutSetWindow(), glutPostRedisplay() for main window, subwindow, window2, for idle function."<<endl;
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033: // "Esc": exit the program.
        exit( EXIT_SUCCESS );
        break;
    
    }
}

//----------------------------------------------------------------------------

void
keyboard2( unsigned char key, int x, int y )
{
    switch( key ) {
        case 033: exit( EXIT_SUCCESS ); break;
        case (int)'r': Red2 = 1; Green2 = 0; Blue2 = 0; Alpha2 = 1; break; // Set window2 background color to red.
        case (int)'g': Red2 = 0; Green2 = 1; Blue2 = 0; Alpha2 = 1; break; // Set window2 background color to green.
        case (int)'b': Red2 = 0; Green2 = 0; Blue2 = 1; Alpha2 = 1; break; // Set window2 background color to blue.
        case (int)'y': Red2 = 1; Green2 = 1; Blue2 = 0; Alpha2 = 1; break; // Set window2 background color to yellow.
        case (int)'o': Red2 = 1; Green2 = 0.5; Blue2 = 0; Alpha2 = 1; break; // Set window2 background color to orange.
        case (int)'p': Red2 = 1; Green2 = 0; Blue2 = 1; Alpha2 = 1; break; // Set window2 background color to purple.
        case (int)'w': Red2 = 1; Green2 = 1; Blue2 = 1; Alpha2 = 1; break; // Set window2 background color to white. 
    }
    glutPostRedisplay(); // Redraw immediately. 
    cout<<"glutPostRedisplay() for window2 keyboard input."<<endl;
}

//----------------------------------------------------------------------------

void
MainWindowMenu( int id )
{
    switch ( id ) {
        case 1: glutIdleFunc( NULL ); break; // Stop animation by removing idle function.
        case 2: glutIdleFunc( rotation ); break; // Start animatino by inserting idle function.
    }
    cout<<"glutIdleFunc() for main window menu."<<endl;
}

//----------------------------------------------------------------------------

void
MainWindowSubmenu( int id )
{
    switch ( id ) { // Change colors of squares. 
        case 1: RedSquare = 1; GreenSquare = 1; BlueSquare = 1; AlphaSquare = 1; break;
        case 2: RedSquare = 1; GreenSquare = 0; BlueSquare = 0; AlphaSquare = 1; break;
        case 3: RedSquare = 0; GreenSquare = 1; BlueSquare = 0; AlphaSquare = 1; break;
        case 4: RedSquare = 0; GreenSquare = 0; BlueSquare = 1; AlphaSquare = 1; break;
    }
    glutPostRedisplay();
    cout<<"glutPostRedisplay() for main window submenu."<<endl;
}

//----------------------------------------------------------------------------

void
setMainWinMenu( void )
{
    int menu_id, submenu_id;
    submenu_id = glutCreateMenu( MainWindowSubmenu ); // Set submenu in main window.
    cout<<"glutCreateMenu() for main window submenu."<<endl;
    glutAddMenuEntry( "White square", 1 );
    glutAddMenuEntry( "Red square", 2 );
    glutAddMenuEntry( "Green square", 3 );
    glutAddMenuEntry( "Blue square", 4 );
    cout<<"glutAddMenuEntry() for main window submenu."<<endl;

    menu_id = glutCreateMenu( MainWindowMenu ); // Set menu in main window. 
    cout<<"glutCreateMenu() for main window menu."<<endl;
    glutAddMenuEntry( "Stop animation", 1 );
    glutAddMenuEntry( "Start animation", 2 );
    cout<<"glutAddMenuEntry() for main window menu."<<endl;
    glutAddSubMenu( "Square color", submenu_id );
    cout<<"glutAddSubMenu() for main window menu & submenu."<<endl;
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    cout<<"glutAttachMenu() for main window menu."<<endl;
}

//----------------------------------------------------------------------------

void
subWindowMenu( int id )
{
    switch ( id ) { // Change colors of subwindow background.
        case 1: subRed = 1.0; subGreen = 0; subBlue = 0; subAlpha = 1.0; break;
        case 2: subRed = 0; subGreen = 1.0; subBlue = 0; subAlpha = 1.0; break;
        case 3: subRed = 0; subGreen = 0; subBlue = 1.0; subAlpha = 1.0; break;
        case 4: subRed = 1.0; subGreen = 1.0; subBlue = 0; subAlpha = 1.0; break;
        case 5: subRed = 1.0; subGreen = 0; subBlue = 1.0; subAlpha = 1.0; break;
        case 6: subRed = 0; subGreen = 1.0; subBlue = 1.0; subAlpha = 1.0; break;
        case 7: subRed = 1.0; subGreen = 1.0; subBlue = 1.0; subAlpha = 1.0; break;
        case 8: subRed = 0; subGreen = 0; subBlue = 0; subAlpha = 1.0; break;
    }
    glutPostRedisplay();
    cout<<"glutPostRedisplay() for sub window menu."<<endl;
}

//----------------------------------------------------------------------------

void
setSubWinMenu( void )
{
    int menu_id;
    menu_id = glutCreateMenu( subWindowMenu ); // Set menu in subwindow. 
    cout<<"glutCreateMenu() for subwindow menu."<<endl;
    glutAddMenuEntry( "Set background red", 1 );
    glutAddMenuEntry( "Set background green", 2 );
    glutAddMenuEntry( "Set background blue", 3 );
    glutAddMenuEntry( "Set background yellow", 4 );
    glutAddMenuEntry( "Set background purple", 5 );
    glutAddMenuEntry( "Set background cyan", 6 );
    glutAddMenuEntry( "Set background white", 7 );
    glutAddMenuEntry( "Set background black", 8);
    cout<<"glutAddMenuEntry() for subwindow menu."<<endl;
    glutAttachMenu( GLUT_RIGHT_BUTTON );
    cout<<"glutAttachMenu() for subwindow menu."<<endl;
}

//----------------------------------------------------------------------------
void
MainMouse( GLint button, GLint state, GLint x, GLint y )
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        y = h - y;
        time_t t;
        time(&t);
        srand(t);
        float red = (float)(rand()%10) / (float)10;
        float green = (float)(rand()%10) / (float)10;
        float blue = (float)(rand()%10) / (float)10; 
        main_ver.push_back( vec2( 2*(float)(x-250)/(float)w, 2*(float)(y-250)/(float)h ) );
        main_col.push_back( color4( red, green, blue, 1 ) );
        init();
        glutPostRedisplay();
        cout<<"glutPostRedisplay() for mouse left click."<<endl;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{   
    glutInit( &argc, argv ); // Initializing environment.
    cout<<"glutInit(&argc,argv) called."<<endl;
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );
    cout<<"glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA) called."<<endl;
    glutInitWindowSize( w, h );
    cout<<"glutInitWindowSize(500,500) called."<<endl;
    glutInitWindowPosition( 100, 100 );
    cout<<"glutInitWindowPosition() called."<<endl;

    MainWindow = glutCreateWindow( "ICG_hw3" ); // Initializing & setting main window.
    cout<<"glutCreateWindow() for main window."<<endl;
    glewExperimental=GL_TRUE; 
    glewInit();    
    init();
    cout<<"glewInit() for main window."<<endl;
    glutDisplayFunc( display );
    cout<<"glutDisplayFunc() for main window."<<endl;
    glutKeyboardFunc( keyboard );
    cout<<"glutKeyboardFunc() for main window."<<endl;
    glutMouseFunc( MainMouse );
    cout<<"glutMouseFunc() for main window."<<endl;
    setMainWinMenu();

    glutInitWindowPosition( 600, 100 ); // Initializing & setting main window.
    cout<<"glutInitWindowPosition() for window2."<<endl;
    glutInitWindowSize( 500, 500 );
    cout<<"glutInitWindowSize() for window2."<<endl;
    MainWindow2 = glutCreateWindow( "window2" );
    cout<<"glutCreateWindow() for window2"<<endl;
    glewExperimental=GL_TRUE;
    glewInit();
    cout<<"glutInit() for window2"<<endl;
    init2();
    glutDisplayFunc( display2 );
    cout<<"glutDisplayFunc() for window2."<<endl;
    glutKeyboardFunc( keyboard2 );
    cout<<"glutKeyboardFunc() for window2."<<endl;

    SubWindow = glutCreateSubWindow( MainWindow, 0, 0, 200, 200 ); // Initializing & setting subwindow.
    cout<<"glutCreateSubWindow() for subwindow."<<endl;
    initsub();
    glutDisplayFunc( subdisplay );
    cout<<"glutDisplayFunc() for subwindow."<<endl;
    setSubWinMenu();
    glutKeyboardFunc( keyboard );
    cout<<"glutKeyboardFunc() for subwindow."<<endl;

    glutIdleFunc( rotation ); // Set idle function for animation. 
    cout<<"glutIdleFunc() called."<<endl;
    glutMainLoop(); // Start main loop. 
    cout<<"glutMainLoop() called."<<endl;
    return 0;
}
