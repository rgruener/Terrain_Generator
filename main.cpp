// main.cpp
// Robert Gruener
// 3/18/13

#include "Angel.h"
#include "terrain.h"
#include <unistd.h>
#include <cstdlib>
#include <cmath>
//#include "tga_loader.cpp"
#include <string>

using namespace std;

Terrain *terrain;

color4 *colors;
point4 *points;
vec3 *normals;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 90.0, 0.0, 0.0 };
GLfloat scale = 1.0;
GLfloat translate[NumAxes] = {0.0, 10.0, -5.0};

// Array for command line arguments
int method = TRIANGLES;
int terrain_order = 5;
float roughness_constant = 0.9;
float range = 0.5;
float init_height = 0.0;

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
GLuint buffer;

int width, height;
unsigned char* grass;
char grass_fname[] = "grass.tga";
GLuint textures[2];

double camera_angle_h = 0;
double camera_angle_v = 0;

int numVertices;
int drag_x_origin;
int drag_y_origin;
int dragging = 0;
int scaling = 0;


//----------------------------------------------------------------------------

// OpenGL initialization
void init(){
    //grass = rgb_tga(grass_fname, &height, &width); 
    //glGenTextures( 2, textures );

    terrain = new Terrain(method, terrain_order, roughness_constant, range, init_height);

    points = terrain->getPoints();
    colors = terrain->getColors();
    normals = terrain->getNormals();

    numVertices = terrain->getNumPoints();
    //terrain->dumpHeightMap();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    
    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    glBufferData( GL_ARRAY_BUFFER, numVertices*(sizeof(point4) + sizeof(color4) + sizeof(vec3)),
          NULL, GL_STREAM_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*numVertices, points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*numVertices, sizeof(color4)*numVertices, colors );
    glBufferSubData( GL_ARRAY_BUFFER, (sizeof(color4)+sizeof(point4))*numVertices, sizeof(vec3)*numVertices, normals );

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

    GLuint vNormal = glGetAttribLocation( program, "vNormal" ); 
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
               BUFFER_OFFSET((sizeof(point4)+sizeof(color4))*numVertices) );

    //glActiveTexture( GL_TEXTURE0);
    //glBindTexture( GL_TEXTURE_2D, textures[0]);

    //glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, grass);
    //glUniform1i( glGetUniformLocation(program, "texGrass"), 0);

    // Initialize shader lighting parameters
    point4 light_position( 10.0, 0.0, 0.0, 0.0 );
    color4 light_ambient( 0.6, 0.6, 0.6, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 1.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.8, 1.0 );
    color4 material_specular( 1.0, 0.8, 0.8, 1.0 );
    float  material_shininess = 20.0;

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

    glShadeModel(GL_SMOOTH);
    glClearColor( .1, .1, .1, 1.0 ); 
}

//----------------------------------------------------------------------------

void display( void ){
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    mat4 mv = Translate(-translate[Xaxis],0,-translate[Yaxis]) * Scale(scale,scale,scale) * 
                RotateX(Theta[Xaxis]) * RotateY(Theta[Yaxis]) * RotateZ(Theta[Zaxis]);
    mat4 rot;

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, mv );

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

void reshape( int width, int height ){
    glViewport( 0, 0, width, height );

    GLfloat aspect = GLfloat(width)/height;
    GLfloat center_factor = (terrain->getSideSize()-1.0f) / 2.0f;
    mat4  projection = Ortho(-center_factor,center_factor,-center_factor,center_factor,
                                -sqrt(terrain->getSideSize()-1)*10,sqrt(terrain->getSideSize()-1)*10);
    projection = Perspective(45.0,aspect,0.5,-3000);
    //projection = mat4(1.0f);
    cout << projection << endl;
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection);
}

//----------------------------------------------------------------------------

void keyboard( unsigned char key, int x, int y ){
    float translate_factor = .4;
    switch( key ) {
        case 'w':
            translate[Yaxis] += translate_factor;
            break;
        case 's':
            translate[Yaxis] -= translate_factor;
            break;
        case 'a':
            translate[Xaxis] -= translate_factor;
            break;
        case 'd':
            translate[Xaxis] += translate_factor;
            break;
        case 033:  // Escape key
            exit( EXIT_SUCCESS );
            break;
    }
    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void special_keyboard( int key, int x, int y ){
    int angle_factor = 4;
    float scale_factor = .01;
    switch( key ) {
        case GLUT_KEY_LEFT:
            //Theta[Yaxis] += angle_factor;
            translate[Xaxis] -= scale_factor;
            break;
        case GLUT_KEY_RIGHT:
            //Theta[Yaxis] -= angle_factor;
            translate[Xaxis] += scale_factor;
            break;
        case GLUT_KEY_UP:
            //scale += scale_factor;
            translate[Yaxis] += scale_factor;
            break;
        case GLUT_KEY_DOWN:
            //scale -= scale_factor;
            translate[Yaxis] -= scale_factor;
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
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN) {
                scaling = 1;
                drag_x_origin = x;
                drag_y_origin = y;
            }
            else
                scaling = 0;
            break;
    }
}

//----------------------------------------------------------------------------

void mouse_move(int x, int y){
    if(dragging) {
        Theta[Xaxis] += (y - drag_y_origin)*0.2;
        Theta[Zaxis] += (x - drag_x_origin)*0.2;
        drag_x_origin = x;
        drag_y_origin = y;
        glutPostRedisplay();
    } else if (scaling){
        scale += (y - drag_y_origin)*0.0005;
        drag_x_origin = x;
        drag_y_origin = y;
        glutPostRedisplay();
    }
}

//----------------------------------------------------------------------------

void mouse_wheel(int button, int dir, int x, int y){
    float scale_factor = .01;
    if (dir > 0){
        scale += scale_factor;
    } else {
        scale -= scale_factor;
    }
    glutPostRedisplay();
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
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( special_keyboard );
    glutMouseFunc( mouse );
    glutMotionFunc( mouse_move );
    glutMouseWheelFunc( mouse_wheel );
    //glutIdleFunc( idle );

    glutMainLoop();
    return 0;
}
