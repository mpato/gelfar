#ifndef UNIQUEKEYTREE_H
#define UNIQUEKEYTREE_H
#include <stdint.h>

#define MAX_KEY 2147483647
#define MIN_KEY -MAX_KEY

class UniqueKeyTree {
 private:
  UniqueKeyTree *left, *right, *parent;
  int key;
  void *obj;
  uint32_t count;
  static UniqueKeyTree * add(UniqueKeyTree *tree, void *obj, int min, int max, int &ret);
  static uint32_t get_free_keys(UniqueKeyTree *tree, int min, int max);
  UniqueKeyTree(int k, void *o) {
    obj = o;
    key = k;
    count = 1;
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
  int remove(int key);
  uint32_t get_count();
};

#endif
