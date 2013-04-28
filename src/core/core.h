#ifndef CORE_H
#define CORE_H
#include "utils/uniquekeytree.h"
#include "corevisual.h"

class Core
{
private:
  static int width, height, view_angle;
  static UniqueKeyTree tree;
  static int visual_update;
  static int selected_id;
public:
  static CoreVisual *visual_root;
  static void init(int width, int height, int counter, char **args);
  static int getSelectedId(int x, int y);
  static void select(int x, int y, int key_event, int down);
  static int registerVisual(CoreVisual *visual);
  static int visualCount();
  static void draw();
  friend void idle_func();
  friend void display_func();
  friend void passive_motion_func(int width, int height);
};

#endif
