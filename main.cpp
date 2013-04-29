// main.cpp
// Robert Gruener
// 3/18/13

#include "Angel.h"
#include "terrain.h"

using namespace std;

Terrain *terrain;

color4 *colors;
point4 *points;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLuint  model_view;  // The location of the "model_view" shader uniform variable
GLuint buffer;

double camera_angle_h = 0;
double camera_angle_v = 0;

int drag_x_origin;
int drag_y_origin;
int dragging = 0;

//----------------------------------------------------------------------------

// OpenGL initialization
void init(){
    terrain = new Terrain(2);

    points = terrain->getPoints();
    colors = terrain->getColors();

    int num_points = terrain->getNumPoints();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    cout << "in init after terrain generated" << endl;

    glBufferData( GL_ARRAY_BUFFER, num_points*(sizeof(point4) + sizeof(color4)),
		  NULL, GL_STREAM_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*num_points, points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*num_points, sizeof(color4)*num_points, colors );

    //Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    //set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET(sizeof(point4)*num_points) );


    model_view = glGetUniformLocation( program, "model_view" );

    glEnable( GL_DEPTH_TEST );
    glClearColor( .1, .1, .1, 1.0 ); 
}

//----------------------------------------------------------------------------

void display( void ){
    cout << "in display" << endl;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    mat4 mv = RotateY(Theta[Yaxis]) * RotateX(Theta[Xaxis]);
    mat4 rot;

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

    int num_points = terrain->getNumPoints();
    
    points = terrain->getPoints();
    colors = terrain->getColors();

    cout << "num points: " << num_points << endl;

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*num_points, points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*num_points, sizeof(color4)*num_points, colors );

    glDrawArrays( GL_LINES, 0, num_points );

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y ){
    switch( key ) {
        case 033:  // Escape key
            exit( EXIT_SUCCESS );
            break;
    }
}

//----------------------------------------------------------------------------

void special_keyboard( int key, int x, int y ){
    int angle_factor = 4;
    switch( key ) {
        case GLUT_KEY_LEFT:
            Theta[Yaxis] += angle_factor;
            break;
        case GLUT_KEY_RIGHT:
            Theta[Yaxis] -= angle_factor;
            break;
        case GLUT_KEY_UP:
            Theta[Xaxis] += angle_factor;
            break;
        case GLUT_KEY_DOWN:
            Theta[Xaxis] -= angle_factor;
            break;
    }
    if (Theta[Xaxis] > 360.0)
        Theta[Xaxis] -= 360.0;
    if (Theta[Yaxis] > 360.0)
        Theta[Yaxis] -= 360.0;
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void mouse( int button, int state, int x, int y ){
    switch (button){
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN) {
                dragging = 1;
                drag_x_origin = x;
                drag_y_origin = y;
            }
            else
                dragging = 0;
            break;
    }
}

//----------------------------------------------------------------------------

void mouse_move(int x, int y){
    if(dragging) {
        Theta[Xaxis] += (y - drag_y_origin)*0.3;
        Theta[Yaxis] += (x - drag_x_origin)*0.3;
        drag_x_origin = x;
        drag_y_origin = y;
        glutPostRedisplay();
    }
}

//----------------------------------------------------------------------------

int main( int argc, char **argv ){

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 600, 600 );
    glutCreateWindow( "Terrain Generator" );

    glewInit();

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( special_keyboard );
    glutMouseFunc( mouse );
    glutMotionFunc( mouse_move);
    //glutIdleFunc( idle );

    glutMainLoop();
    return 0;
}
