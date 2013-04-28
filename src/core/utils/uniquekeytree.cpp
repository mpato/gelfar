#include <stdio.h>

#include "uniquekeytree.h"

#define MIDDLE_POINT(min, max) ((min)  + ((max) - (min)) / 2)
#define CAN_INSERT_LEFT(left, right, min, middle, max)  (get_free_keys(left, min, middle - 1) >= get_free_keys(right, middle + 1, max))

uint32_t UniqueKeyTree::get_free_keys(UniqueKeyTree *tree, int min, int max)
{
  uint32_t count, total_keys;
  count = tree ? tree->get_count() : 0;
  if (tree != NULL && tree->key == 0)
    total_keys = 2 * (uint32_t) MAX_KEY;
  else
    total_keys = max - min + 1;
  return total_keys - count;
}

UniqueKeyTree * UniqueKeyTree::add(UniqueKeyTree * tree, void *obj, int min, int max, int &ret)
{
  // printf("Min %d Max %d %d %d %d\n", min, max, obj, tree ? tree->get_count() : 0, get_free_keys(tree, min, max));
  if (get_free_keys(tree, min, max) <= 0) {
    ret = 0;
    return tree;
  } else if (tree == NULL) {
    ret = MIDDLE_POINT(min, max);
    return new UniqueKeyTree(ret, obj);
  }
  if (tree->key != 0 && tree->obj == NULL) {
    ret = tree->key;
    tree->obj = obj;
  } // an object can be inserted on the left if there are at least as 
   // many free keys available on the left as on the right
  else if (CAN_INSERT_LEFT(tree->left, tree->right, min, tree->key, max)) {
    tree->left = add(tree->left, obj, min, tree->key - 1, ret);
  } else {
    tree->right = add(tree->right, obj, tree->key + 1, max, ret);
  }
  if (ret)
    tree->count++;
  return tree;
}

int UniqueKeyTree::add(void *obj)
{
  //printf("\n");
  int ret = 0;
  add(this, obj, MIN_KEY, MAX_KEY, ret);
  return ret;
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

int UniqueKeyTree::remove(int k)
{
  int ret;
  if (key == k) {
    ret = (obj != NULL);
    obj = NULL;
  } else if (key < k)
    ret = right ? right->remove(k) : 0;
  else
    ret =  left ? left->remove(k) : 0;
  
  if (ret)
    count --;
  return ret;
}

uint32_t UniqueKeyTree::get_count()
{
  return count;
}

/*uint32_t UniqueKeyTree::get_count()
{
  return count;
}

int UniqueKeyTree::is_balanced()
{
  return get_count(left)
}
*/
