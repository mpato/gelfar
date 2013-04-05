#ifndef UNIQUEKEYTREE_H
#define UNIQUEKEYTREE_H
#include <stdlib.h>

class UniqueKeyTree {
 private:
  UniqueKeyTree *left, *right, *parent;
  int key;
  void *obj;
  int count;
  int add(void *obj, int min, int max);
  UniqueKeyTree(int k, void *o) {
    obj = o;
    key = k;
  }
 public:
  UniqueKeyTree() {
    left = NULL;
    right = NULL;
    parent = NULL;
    key = 0;
    obj = NULL;
    count = 0;
  }

  ~UniqueKeyTree() {
    if (left)
      delete left;
    if (right)
      delete right;
    if (parent)
      delete parent;
  }

  int add(void *obj);
  void *search(int key);
  int get_count();
};

#endif
