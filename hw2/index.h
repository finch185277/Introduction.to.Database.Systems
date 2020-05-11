#ifndef INDEX_H_
#define INDEX_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Node;
class Index;

void Index::search(int x);
void Index::insert(int x);
void Index::insertInternal(int x, Node *cursor, Node *child);
Node *Index::findParent(Node *cursor, Node *child);
void Index::remove(int x);
void Index::removeInternal(int x, Node *cursor, Node *child);
void Index::display(Node *cursor);
Node *Index::getRoot();
void Index::cleanUp(Node *cursor);

#endif
