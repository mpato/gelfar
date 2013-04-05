#ifndef CORE_H
#define CORE_H
#include "utils/uniquekeytree.h"
#include "corevisual.h"

class Core
{
private:
  static UniqueKeyTree tree;
  static int width, height, view_angle;
  static CoreVisual *visual_root;
public:
  static void init(int counter, char **args);
  static int getSelectedId(int x, int y);
  static void select(int x, int y, int key_event, int down);
  static void draw();
};

#endif
