#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "core.h"
#include "../ge/GEVisual.h"

UniqueKeyTree Core::tree;
int Core::width, Core::height, Core::view_angle;
CoreVisual *Core::visual_root;
int Core::visual_update = 0;
int Core::selected_id = 0;

struct event_t {
  int x, y;
  int key, type;
};

event_t events[100];
int n_events = 0;

void idle_func()
{
  event_t event;
  if (n_events > 0) {
    //for (int i = 0; i < n_events; i++) {
      event = events[n_events - 1];
      if (event.key != 0 || event.type != 0)
	Core::select(event.x, event.y, event.key, event.type);
      //}
    n_events = 0;
    //return;
  }

  if (!Core::visual_update)
    return;
  Core::visual_update = 0;
  //printf("idle\n");
  Core::draw();
  glutSwapBuffers();
}

void display_func()
{ 
  Core::visual_update = 1;
}

void passive_motion_func(int x, int y)
{
  event_t event;
  printf("mouse: over %d %d\n", x, y);
  //Core::select(x, y, KEV_MOUSE_NO_BUTTON, KEV_MOUSE_OVER);
  event.x = x;
  event.y = y;
  event.key = KEV_MOUSE_NO_BUTTON;
  event.type = KEV_MOUSE_OVER;
  events[n_events ++] = event;
}

void mouse_func(int button, int state, int x, int y)
{
  printf("mouse: %d %d\n", button, state);
  Core::select(x, y, button, state ==  GLUT_DOWN ? KEV_MOUSE_DOWN  : KEV_MOUSE_UP); 
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
  //printf("XY %d %d %d %d %d %d %f\n",x, y, view[0], view[1], view[2], view[3], (float)width/height);
  gluPickMatrix(x, view[3] - y, 1, 1, view);
  gluPerspective(45, (float)width/height, .1, 100);

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
    return 0;
}

void Core::select(int x, int y, int key_event, int type)
{
  CoreVisual *visual;
  //if (type != KEV_MOUSE_UP)
    selected_id = getSelectedId(x, y);
  //printf("selected %d\n", selected_id);
  if (!selected_id)
    return;
  visual = (CoreVisual *) tree.search(selected_id);
  if (visual == NULL)
    return;
  visual->touch(key_event, type);
  //if (type != KEV_MOUSE_DOWN)
    selected_id = 0;
  //printf("touch\n");
  visual_update = 1;
}

int Core::registerVisual(CoreVisual *visual)
{
  tree.add(visual);
}

int Core::visualCount()
{
  return tree.get_count();
}

void Core::draw()
{
  vector_t unit_size;
  unit_size.x = 0.1;
  unit_size.y = 0.1;
  printf("Draw\n");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Load identity matrix
  glLoadIdentity();
  // Multiply in translation matrix
  glTranslatef(0, 0, -3);
  // Multiply in rotation matrix
  glRotatef(-45, 1, 0, 0);
  if (visual_root != NULL)
    visual_root->draw(unit_size);
}

// Initialze OpenGL perspective matrix
void GL_Setup(int width, int height)
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glEnable(GL_DEPTH_TEST);
  gluPerspective(45, (float)width/height, .1, 100);
  glMatrixMode(GL_MODELVIEW);
}

void Core::init(int width, int height, int counter, char **args)
{
  glutInit(&counter, args);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  Core::width = width;
  Core::height = height;
  glutInitWindowSize(Core::width, Core::height);
  glutCreateWindow("The boredom of Gelfar");
  glutIdleFunc(idle_func);
  glutMouseFunc(mouse_func);
  glutDisplayFunc(display_func);
  glutPassiveMotionFunc(passive_motion_func);
  glutMotionFunc(passive_motion_func);
  GL_Setup(Core::width, Core::height);
  glutMainLoop();
}

void tree_test(int size)
{
  int * test = (int *) malloc(sizeof(int) * size);
  UniqueKeyTree tree;
  int tests = 0, success = 0, ret;
  for (int i = 0; i < size; i++)
    test[i] = tree.add((void *) (i + 1));
  for (int i = 0; i < size; i++) {
    ret = (int) tree.search(test[i]);
    if (ret == i + 1)
      success ++;
    /*else
      printf("Expected %d got %d (key %d)\n", i + 1, ret, test[i]);*/
    tests ++;
  }
  free(test);
  printf("Tree test: %d/%d (%d inserted)\n", success, tests, tree.get_count());
  exit(0);
}

int main(int argc, char **argv)
{
  cell_vector_t dimensions;
  unit_vector_t cell_size;
  //tree_test(3 * MAX_KEY); //1000000);
  dimensions.col = 30;
  dimensions.row = 30;
  cell_size.x = 3;
  cell_size.y = 3;
  printf("Core: nr objects: %d\n", Core::visualCount());
  Core::visual_root = new GEMapGrid(dimensions, cell_size);
  printf("Core: nr objects: %d\n", Core::visualCount());
  Core::init(800, 600, argc, argv);
}
