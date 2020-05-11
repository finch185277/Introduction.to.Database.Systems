#ifndef INDEX_H_
#define INDEX_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Index; // self explanatory classes

class Node {
private:
  bool IS_LEAF;
  int *key, size;
  Node **ptr;
  friend class Index;

public:
  Node();
};

class Index {
private:
  Node *root;
  void insertInternal(int, Node *, Node *);
  void removeInternal(int, Node *, Node *);
  Node *findParent(Node *, Node *);

public:
  Index();
  Index(int &num_rows, vector<int> &key, vector<int> &value);
  void search(int);
  void insert(int);
  void remove(int);
  void display(Node *);
  Node *getRoot();
  void cleanUp(Node *);
  void clear_index();
  ~Index();
};

#endif
