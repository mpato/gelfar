#include "core.h"
#include "../ge/GEVisual.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

UniqueKeyTree Core::tree;
int Core::width, Core::height, Core::view_angle;
CoreVisual *Core::visual_root;

void config()
{
}

void idle_func()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Load identity matrix
  glLoadIdentity();
  // Multiply in translation matrix
  glTranslatef(0,0, -3);
  // Multiply in rotation matrix
  glRotatef(-60, 1, 0, 0);
  Core::draw();

  /*int i,j;

    for(i=0; i < 100; i++){
      for(j=0; j < 100; j++){
        glBegin(GL_QUADS);
        glColor3ub(255, 255, 255);
        glVertex2f(0.01,  -0.01);
        glVertex2f( 0.01,  -0.09);
        glVertex2f( 0.09, -0.09);
        glVertex2f(0.09, -0.01);
        glEnd();
        // Swap buffers (color buffers, makes previous render visible)

        glTranslatef(0.1,0, 0);
      }
      glTranslatef(-100*0.1,0, 0);
      glTranslatef(0,-0.1,0);
    }*/

  glutSwapBuffers();
}

void display_func()
{
  Core::draw();
  glutSwapBuffers();
}

void mouse_func(int button, int state, int x, int y)
{

  Core::select(x, y, button, state ==  GLUT_DOWN);
}

int Core::getSelectedId(int x, int y)
{
  GLuint buff[64] = {0};
  GLint hits, view[4];
  int id;
  
  //This chooses the buffer where to store the values for the selection data
  glSelectBuffer(64, buff);
  
  //This retrieves info about the viewport
  glGetIntegerv(GL_VIEWPORT, view);

  //Switching in selecton mode
  glRenderMode(GL_SELECT);
  
  // Clearing the names' stack
  // This stack contains all the info about the objects
  glInitNames();
  
  // Now fill the stack with one element (or glLoadName will generate an error)
  glPushName(0);
  
  // Now modify the viewing volume, restricting selection area around the cursor
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // Restrict the draw to an area around the cursor
  gluPickMatrix(x, y, 1, 1, view);
  gluPerspective(view_angle, (float)width/height, .1, 100 );

  // Draw the objects onto the screen
  glMatrixMode(GL_MODELVIEW);
  
  // Draw only the names in the stack, and fill the array
  draw();
  
  // Do you remeber? We do pushMatrix in PROJECTION mode
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // Get number of objects drawed in that area
  // and return to render mode
  hits = glRenderMode(GL_RENDER);
  
  // Print a list of the objects
  //list_hits(hits, buff);
  
  // uncomment this to show the whole buffer
  //gl_selall(hits, buff);
  
  glMatrixMode(GL_MODELVIEW);
  
  if(hits > 0)
    return buff[3];
  else
    return -1;
}

void Core::select(int x, int y, int key_event, int down)
{
  int id;
  CoreVisual *visual;
  id = getSelectedId(x, y);
  visual = (CoreVisual *) tree.search(id);
  if (visual != NULL)
    visual->touch(key_event, down);
}

void Core::draw()
{
  vector_t unit_size;
  unit_size.x = 0.1;
  unit_size.y = 0.1;
  if (visual_root != NULL)
    visual_root->draw(unit_size);
}

// Initialze OpenGL perspective matrix
void GL_Setup(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glEnable(GL_DEPTH_TEST);
  gluPerspective(30, (float)width/height, .1, 100);
  glMatrixMode(GL_MODELVIEW);
}

void Core::init(int counter, char **args)
{
  glutInit(&counter, args);
  //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  Core::width = 640;
  Core::height = 480;
  glutInitWindowSize(Core::width, Core::height);
  glutCreateWindow("StratGame");
  glutIdleFunc(idle_func);
  //glutDisplayFunc(display_func);
  config();
  GL_Setup(Core::width, Core::height);
  glutMainLoop();
}

int main(int argc, char **argv)
{
  cell_vector_t dimensions;
  unit_vector_t cell_size;
  dimensions.col = 100;
  dimensions.row = 100;
  cell_size.x = 1;
  cell_size.y = 1;
  Core::visual_root = new GEMapGrid(dimensions, cell_size);
  Core::init(argc, argv);
}
