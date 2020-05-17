// ref: https://github.com/deepaktabraham/BPlus-Tree

#ifndef INDEX_H_
#define INDEX_H_

// uncomment for debug mode
//#define DEBUG

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

#define DEFAULT_ORDER 10
#define ERROR -1
#define NUM_ARGS 2
#define MIN_ORDER 3

using namespace std;

// generic node
class Node {
protected:
  bool isLeaf;
  vector<int> keys;

public:
  bool Get_IsLeaf();
  vector<int> Get_Keys();
  virtual void Insert(int key, int value) {}
  virtual void Insert(int key, Node *rightChild) {}
  virtual void Insert(int key, Node *leftChild, Node *rightChild) {}
  virtual void Search(int key) {}
  virtual void Search(int key1, int key2) {}
  virtual Node *Split(int *keyToParent) {}
  virtual vector<Node *> Get_Children() {}
  virtual vector<int> Get_Values() {}
  virtual Node *Get_Next() {}
};

// internal node
class InternalNode : public Node {
private:
  vector<Node *> children;

public:
  InternalNode();
  void Insert(int key, Node *rightChild);
  void Insert(int key, Node *leftChild, Node *rightChild);
  Node *Split(int *keyToParent);
  vector<Node *> Get_Children();
};

// leaf node
class LeafNode : public Node {
private:
  LeafNode *prev;
  LeafNode *next;
  vector<int> values;

public:
  LeafNode();
  void Insert(int key, int value);
  Node *Split(int *keyToParent);
  vector<int> Get_Values();
  Node *Get_Next();
};

// B+ tree
class BPlusTree {
private:
  int order;
  Node *root;
  void Search_Path(Node *node, int key, stack<Node *> *path);
  void Destroy(Node *node);

public:
  void Initialize(int m);
  void Insert(int key, int value);
  int Search(int key);
  vector<pair<int, int>> Search(int key1, int key2);
  ~BPlusTree();
};

class Index {
private:
  BPlusTree tree;

public:
  Index(int &num_rows, vector<int> &key, vector<int> &value);
  void key_query(vector<int> &query_keys);
  void range_query(vector<pair<int, int>> &query_pairs);
  void clear_index();
};

#endif
