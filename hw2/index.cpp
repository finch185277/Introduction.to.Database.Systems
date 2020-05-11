// ref: https://www.tutorialspoint.com/cplusplus-program-to-implement-bplus-tree

#include <iostream>

#define ORDER 6

struct BPT_node {
  int *data;
  BPT_node **child_ptr;
  bool is_leaf;
  int nleaf;
} *root = NULL, *parent = NULL, *cur = NULL;

BPT_node *init() {
  parent = new BPT_node;
  parent->data = new int[ORDER];
  parent->child_ptr = new BPT_node *[ORDER];
  parent->is_leaf = true;
  parent->nleaf = 0;
  for (int i = 0; i < ORDER; i++) {
    parent->child_ptr[i] = NULL;
  }
  return parent;
}

void traverse(BPT_node *node) {
  std::cout << std::endl;
  int i;
  for (i = 0; i < node->nleaf; i++) {
    if (node->is_leaf == false) {
      traverse(node->child_ptr[i]);
    }
    std::cout << " " << node->data[i];
  }
  if (node->is_leaf == false) {
    traverse(node->child_ptr[i]);
  }
  std::cout << std::endl;
}

void sort(int *node, int nleaf) {
  for (int i = 0; i < nleaf; i++) {
    for (int j = i; j <= nleaf; j++) {
      if (node[i] > node[j]) {
        int t = node[i];
        node[i] = node[j];
        node[j] = t;
      }
    }
  }
}

int split_child(BPT_node *cur, int i) {
  int j, mid;
  BPT_node *parent1, *parent3, *y;
  parent3 = init();
  parent3->is_leaf = true;
  if (i == -1) {
    mid = cur->data[ORDER / 2 - 1];
    cur->data[ORDER / 2 - 1] = 0;
    cur->nleaf--;
    parent1 = init();
    parent1->is_leaf = false;
    cur->is_leaf = true;
    for (j = ORDER / 2; j < ORDER; j++) {
      parent3->data[j - ORDER / 2] = cur->data[j];
      parent3->child_ptr[j - ORDER / 2] = cur->child_ptr[j];
      parent3->nleaf++;
      cur->data[j] = 0;
      cur->nleaf--;
    }
    for (j = 0; j < ORDER; j++) {
      cur->child_ptr[j] = NULL;
    }
    parent1->data[0] = mid;
    parent1->child_ptr[parent1->nleaf] = cur;
    parent1->child_ptr[parent1->nleaf + 1] = parent3;
    parent1->nleaf++;
    root = parent1;
  } else {
    y = cur->child_ptr[i];
    mid = y->data[ORDER / 2 - 1];
    y->data[ORDER / 2 - 1] = 0;
    y->nleaf--;
    for (j = ORDER / 2; j < ORDER; j++) {
      parent3->data[j - ORDER / 2] = y->data[j];
      parent3->nleaf++;
      y->data[j] = 0;
      y->nleaf--;
    }
    cur->child_ptr[i + 1] = y;
    cur->child_ptr[i + 1] = parent3;
  }
  return mid;
}

void insert(int key) {
  int i, t;
  cur = root;
  if (cur == NULL) {
    root = init();
    cur = root;
  } else {
    if (cur->is_leaf == true && cur->nleaf == ORDER) {
      t = split_child(cur, -1);
      cur = root;
      for (i = 0; i < (cur->nleaf); i++) {
        if ((key > cur->data[i]) && (key < cur->data[i + 1])) {
          i++;
          break;
        } else if (key < cur->data[0]) {
          break;
        } else {
          continue;
        }
      }
      cur = cur->child_ptr[i];
    } else {
      while (cur->is_leaf == false) {
        for (i = 0; i < (cur->nleaf); i++) {
          if ((key > cur->data[i]) && (key < cur->data[i + 1])) {
            i++;
            break;
          } else if (key < cur->data[0]) {
            break;
          } else {
            continue;
          }
        }
        if ((cur->child_ptr[i])->nleaf == ORDER) {
          t = split_child(cur, i);
          cur->data[cur->nleaf] = t;
          cur->nleaf++;
          continue;
        } else {
          cur = cur->child_ptr[i];
        }
      }
    }
  }
  cur->data[cur->nleaf] = key;
  sort(cur->data, cur->nleaf);
  cur->nleaf++;
}

int main() {
  int n;
  std::cout << "enter the no of elements to be inserted\n";
  std::cin >> n;
  for (int i = 0; i < n; i++) {
    int t;
    std::cout << "enter the element\n";
    std::cin >> t;
    insert(t);
  }
  std::cout << "traversal of constructed B tree\n";
  traverse(root);
}
