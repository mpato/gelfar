#include "uniquekeytree.h"
#define MAX_KEY 2147483648U
#define MIN_KEY -MAX_KEY

int UniqueKeyTree::add(void *obj, int min, int max)
{
  int ret;
  if (left == NULL) {
    ret = (min + key) / 2;
    left = new UniqueKeyTree(ret, obj);
  } else if (right == NULL) {
    ret = (max + key) / 2;
    right = new UniqueKeyTree(ret, obj);
  } else if (left->get_count() >= right->get_count()) {
    right->add(obj, key, max);
  } else {
    left->add(obj, min, key);
  }

  count++;
  return ret;
}

int UniqueKeyTree::add(void *obj)
{
  add(obj, MIN_KEY, MAX_KEY);
}

void *UniqueKeyTree::search(int k)
{
  if (key == k)
    return obj;
  else if (key < k)
    return right ? right->search(k) : NULL;
  else
    return left ? left->search(k) : NULL;
}

int UniqueKeyTree::get_count()
{
  return count;
}
