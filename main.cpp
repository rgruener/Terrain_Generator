// main.cpp
// Robert Gruener
// 3/18/13

#include "Angel.h"
#include "terrain.h"
#include <unistd.h>
#include <cstdlib>

using namespace std;

Terrain *terrain;

color4 *colors;
point4 *points;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLfloat scale = 1.0;

// Array for command line arguments
int method = TRIANGLES;
int terrain_order = 5;
float roughness_constant = 0.7;
float range = 0.8;
float init_height = 0.0;

GLuint  model_view;  // The location of the "model_view" shader uniform variable
GLuint buffer;

double camera_angle_h = 0;
double camera_angle_v = 0;

int numVertices;
int drag_x_origin;
int drag_y_origin;
int dragging = 0;

//----------------------------------------------------------------------------

// OpenGL initialization
void init(){

    terrain = new Terrain(method, terrain_order, roughness_constant, range, init_height);

    points = terrain->getPoints();
    colors = terrain->getColors();

    numVertices = terrain->getNumPoints();
    //terrain->dumpHeightMap();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    glBufferData( GL_ARRAY_BUFFER, numVertices*(sizeof(point4) + sizeof(color4)),
		  NULL, GL_STREAM_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*numVertices, points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*numVertices, sizeof(color4)*numVertices, colors );

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
               BUFFER_OFFSET(sizeof(point4)*numVertices) );

    // Initialize shader lighting parameters
    point4 light_position( 0.0, 0.0, -1.0, 0.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product );
	
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		  1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );
		 
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH_TEST );

    glShadeModel(GL_FLAT);
    glClearColor( .1, .1, .1, 1.0 ); 
}

//----------------------------------------------------------------------------

void display( void ){
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    mat4 mv = Scale(scale, scale, scale) * RotateY(Theta[Yaxis]) * RotateX(Theta[Xaxis]);
    mat4 rot;

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );

    switch(method){
        case TRIANGLES:
            glDrawArrays( GL_TRIANGLES, 0, numVertices );
            break;
        case LINES:
            glDrawArrays( GL_LINES, 0, numVertices );
            break;
    }

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
    float scale_factor = .01;
    switch( key ) {
        case GLUT_KEY_LEFT:
            Theta[Yaxis] += angle_factor;
            break;
        case GLUT_KEY_RIGHT:
            Theta[Yaxis] -= angle_factor;
            break;
        case GLUT_KEY_UP:
            scale += scale_factor;
            break;
        case GLUT_KEY_DOWN:
            scale -= scale_factor;
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

void parseCommandLineArguments(int argc, char** argv){
    int c;

    while ((c=getopt(argc, argv, "lto:h:r:i:")) != -1){
        switch(c){
            case 'l':
                method = LINES;
                break;
            case 't':
                method = TRIANGLES;
                break;
            case 'o':
                terrain_order = atoi(optarg);
                break;
            case 'h':
                roughness_constant = atof(optarg);
                break;
            case 'r':
                range = atof(optarg);
                break;
            case 'i':
                init_height = atof(optarg);
                break;
        }
    }
}

//----------------------------------------------------------------------------


int main( int argc, char **argv ){

    parseCommandLineArguments(argc, argv);

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
