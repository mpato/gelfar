#ifndef COREVISUAL_H
#define COREVISUAL_H
#include <GL/glut.h>
#include "utils/vector.h"

#define  KEV_MOUSE_LEFT       GLUT_LEFT_BUTTON
#define  KEV_MOUSE_RIGHT      GLUT_RIGHT_BUTTON
#define  KEV_MOUSE_NO_BUTTON  -1
#define  KEV_MOUSE_DOWN       1
#define  KEV_MOUSE_UP         2
#define  KEV_MOUSE_OVER       3

class CoreVisual {
 protected:
  int key;
  void registerVisual();
 public:
  CoreVisual();
  int getKey();
  virtual void touch(int key_event, int type) = 0;
  virtual ~CoreVisual() {}
  virtual void draw(vector_t unit_size) = 0;
};
#endif
