#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
  NIL->color = RBTREE_BLACK;
  p->root = NIL;
  p->nil = NIL;
  return p;
}
void left_rotate(rbtree *t, node_t *p)
{
  node_t *y = p->right;
  p->right = y->left;
  if (y->left != t->nil)
  {
    y->left->parent = p;
  }
  y->parent = p->parent;
  if (p->parent == t->nil)
  {
    t->root = y;
  }
  else if (p == p->parent->left)
  {
    p->parent->left = y;
  }
  else
  {
    p->parent->right = y;
  }
  y->left = p;
  p->parent = y;
}
void right_rotate(rbtree *t, node_t *p)
{
  node_t *y = p->left;
  p->left = y->right;
  if (y->right != t->nil)
  {
    y->right->parent = p;
  }
  y->parent = p->parent;
  if (p->parent == t->nil)
  {
    t->root = y;
  }
  else if (p == p->parent->right)
  {
    p->parent->right = y;
  }
  else
  {
    p->parent->left = y;
  }
  y->right = p;
  p->parent = y;
}
void delete_node(rbtree *t, node_t *p)
{
  if (p != t->nil)
  {
    delete_node(t, p->left);
    delete_node(t, p->right);
    free(p);
    p = NULL;
  }
}
void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  delete_node(t, t->root);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  z->key = key;
  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }
  z->parent = y;
  if (y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  // fix up
  while (z->parent->color == RBTREE_RED)
  {
    node_t *gp = z->parent->parent;
    // uncle node
    y = gp->right;

    if (z->parent == gp->left)
    {

      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        z = gp;
      }
      else
      {

        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }

        z->parent->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        right_rotate(t, gp);
      }
    }
    else
    {
      y = gp->left;
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        z = gp;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        gp->color = RBTREE_RED;
        left_rotate(t, gp);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *s = t->root;
  while (s != t->nil)
  {
    if (s->key == key)
    {
      return s;
    }
    else if (s->key > key)
    {
      s = s->left;
    }
    else
    {
      s = s->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *n = t->root;
  while (n->left != t->nil)
  {
    n = n->left;
  }
  return n;
}
node_t *rbtree_max(const rbtree *t)
{
  node_t *n = t->root;
  while (n->right != t->nil)
  {
    n = n->right;
  }
  return n;
}
node_t *tree_minimum(const rbtree *t, node_t *x)
{
  while (x->left != t->nil)
  {
    x = x->left;
  }
  return x;
}

void transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

int rbtree_erase(rbtree *t, node_t *z)
{
  // 삭제하려는 노드 z를 우선 y에 저장.
  // y가 z를 기준으로 시작하지만 중간에 바뀔 수 있다.
  node_t *y = z;
  node_t *x;
  color_t y_original_color = y->color;

  // 노드 z에게 유효한 값을 가진 자식이 하나 있는데
  // 그 자식이 오른쪽에 있는 경우
  if (z->left == t->nil)
  {
    x = z->right;               // 오른쪽 자식을 x에 담아두고
    transplant(t, z, z->right); // z의 오른쪽 자식을 p에 위치에 이식(transplant)하면서 z는 제거

    // 유효한 값을 가진 자식이 왼쪽에만 하나 있는 경우
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    // z의 왼쪽 자식을 z에 위치에 이식하면서 p는 제거됨
    transplant(t, z, z->left);
  }
  else
  {
    // 유효한 자식이 둘인 경우
    y = tree_minimum(t, z->right);
    y_original_color = y->color;
    x = y->right;
    if (y->parent == z)
    {
      x->parent = y;
    }
    else
    {
      transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  // 삭제되는 색이 BLACK인 경우에만 속성 위반 여부 확인
  // fix up
  if (y_original_color == RBTREE_BLACK)
  {
    node_t *w;
    while (x != t->root && x->color == RBTREE_BLACK)
    {
      if (x == x->parent->left)
      {
        w = x->parent->right;
        // x 의 형제 w가 RED 인 경우
        if (w->color == RBTREE_RED)
        {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          left_rotate(t, x->parent);
          w = x->parent->right;
        }
        // X의 형제 W는 BLACK이고 W의 두 자식이 모두 BLACK인 경우
        if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
        {
          w->color = RBTREE_RED;
          x = x->parent;
          // X의 형제 W는 BLACK, W의 왼쪽 자식은 RED, W의 오른쪽 자식은 BLACK인 경우
        }
        else
        {
          if (w->right->color == RBTREE_BLACK)
          {
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            right_rotate(t, w);
            w = x->parent->right;
          }
          // X의 형제 W는 BLACK이고 W의 오른쪽 자식은 RED인 경우
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->right->color = RBTREE_BLACK;
          left_rotate(t, x->parent);
          x = t->root;
        }
      }
      else
      {
        // 오른쪽 케이스 ( 왼쪽, 오른쪽만 반대고 동일함)
        w = x->parent->left;
        if (w->color == RBTREE_RED)
        {
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          right_rotate(t, x->parent);
          w = x->parent->left;
        }
        if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
        {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else
        {
          if (w->left->color == RBTREE_BLACK)
          {
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotate(t, w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->left->color = RBTREE_BLACK;
          right_rotate(t, x->parent);
          x = t->root;
        }
      }
    }
    x->color = RBTREE_BLACK;
  }
  free(z);
  return 0;
}
int inorder(const rbtree *t, node_t *p, key_t *arr, const size_t n, int i)
{
  if (i < n)
  {
    if (p == t->nil)
    {
      return i;
    }
    i = inorder(t, p->left, arr, n, i);
    arr[i++] = p->key;
    i = inorder(t, p->right, arr, n, i);
    return i;
  }
  return 0;
}
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  inorder(t, t->root, arr, n, 0);
  return 0;
}