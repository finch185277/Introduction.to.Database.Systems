// ref: https://www.tutorialspoint.com/cplusplus-program-to-implement-bplus-tree

#include <iostream>

#define ORDER 6

struct BPT_node {
  int *d;
  BPT_node **child_ptr;
  bool l;
  int n;
} *r = NULL, *np = NULL, *x = NULL;

BPT_node *init() {
  int i;
  np = new BPT_node;
  np->d = new int[ORDER];
  np->child_ptr = new BPT_node *[ORDER];
  np->l = true;
  np->n = 0;
  for (i = 0; i < ORDER; i++) {
    np->child_ptr[i] = NULL;
  }
  return np;
}

void traverse(BPT_node *p) {
  std::cout << std::endl;
  int i;
  for (i = 0; i < p->n; i++) {
    if (p->l == false) {
      traverse(p->child_ptr[i]);
    }
    std::cout << " " << p->d[i];
  }
  if (p->l == false) {
    traverse(p->child_ptr[i]);
  }
  std::cout << std::endl;
}

void sort(int *p, int n) {
  int i, j, t;
  for (i = 0; i < n; i++) {
    for (j = i; j <= n; j++) {
      if (p[i] > p[j]) {
        t = p[i];
        p[i] = p[j];
        p[j] = t;
      }
    }
  }
}

int split_child(BPT_node *x, int i) {
  int j, mid;
  BPT_node *np1, *np3, *y;
  np3 = init();
  np3->l = true;
  if (i == -1) {
    mid = x->d[ORDER / 2 - 1];
    x->d[ORDER / 2 - 1] = 0;
    x->n--;
    np1 = init();
    np1->l = false;
    x->l = true;
    for (j = ORDER / 2; j < ORDER; j++) {
      np3->d[j - ORDER / 2] = x->d[j];
      np3->child_ptr[j - ORDER / 2] = x->child_ptr[j];
      np3->n++;
      x->d[j] = 0;
      x->n--;
    }
    for (j = 0; j < ORDER; j++) {
      x->child_ptr[j] = NULL;
    }
    np1->d[0] = mid;
    np1->child_ptr[np1->n] = x;
    np1->child_ptr[np1->n + 1] = np3;
    np1->n++;
    r = np1;
  } else {
    y = x->child_ptr[i];
    mid = y->d[ORDER / 2 - 1];
    y->d[ORDER / 2 - 1] = 0;
    y->n--;
    for (j = ORDER / 2; j < ORDER; j++) {
      np3->d[j - ORDER / 2] = y->d[j];
      np3->n++;
      y->d[j] = 0;
      y->n--;
    }
    x->child_ptr[i + 1] = y;
    x->child_ptr[i + 1] = np3;
  }
  return mid;
}

void insert(int a) {
  int i, t;
  x = r;
  if (x == NULL) {
    r = init();
    x = r;
  } else {
    if (x->l == true && x->n == ORDER) {
      t = split_child(x, -1);
      x = r;
      for (i = 0; i < (x->n); i++) {
        if ((a > x->d[i]) && (a < x->d[i + 1])) {
          i++;
          break;
        } else if (a < x->d[0]) {
          break;
        } else {
          continue;
        }
      }
      x = x->child_ptr[i];
    } else {
      while (x->l == false) {
        for (i = 0; i < (x->n); i++) {
          if ((a > x->d[i]) && (a < x->d[i + 1])) {
            i++;
            break;
          } else if (a < x->d[0]) {
            break;
          } else {
            continue;
          }
        }
        if ((x->child_ptr[i])->n == ORDER) {
          t = split_child(x, i);
          x->d[x->n] = t;
          x->n++;
          continue;
        } else {
          x = x->child_ptr[i];
        }
      }
    }
  }
  x->d[x->n] = a;
  sort(x->d, x->n);
  x->n++;
}

int main() {
  int i, n, t;
  std::cout << "enter the no of elements to be inserted\n";
  std::cin >> n;
  for (i = 0; i < n; i++) {
    std::cout << "enter the element\n";
    std::cin >> t;
    insert(t);
  }
  std::cout << "traversal of constructed B tree\n";
  traverse(r);
}
