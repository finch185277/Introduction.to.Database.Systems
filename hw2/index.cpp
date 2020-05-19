#include "index.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Node.cpp
Node::Node(int aOrder) : fOrder{aOrder}, fParent{nullptr} {}

Node::Node(int aOrder, Node *aParent) : fOrder{aOrder}, fParent{aParent} {}

Node::~Node() {}

int Node::order() const { return fOrder; }

Node *Node::parent() const { return fParent; }

void Node::setParent(Node *aParent) { fParent = aParent; }

bool Node::isRoot() const { return !fParent; }

// Record.cpp
Record::Record(ValueType aValue) : fValue(aValue) {}

ValueType Record::value() const { return fValue; }

void Record::setValue(ValueType aValue) { fValue = aValue; }

std::string Record::toString() const {
  std::ostringstream oss;
  oss << fValue;
  return oss.str();
}

// InternalNode.cpp
InternalNode::InternalNode(int aOrder) : Node(aOrder) {}

InternalNode::InternalNode(int aOrder, Node *aParent) : Node(aOrder, aParent) {}

InternalNode::~InternalNode() {
  for (auto mapping : fMappings) {
    delete mapping.second;
  }
}

bool InternalNode::isLeaf() const { return false; }

int InternalNode::size() const { return static_cast<int>(fMappings.size()); }

int InternalNode::minSize() const { return (order() + 1) / 2; }

int InternalNode::maxSize() const {
  // Includes the first entry, which
  // has key DUMMY_KEY and a value that
  // points to the left of the first positive key k1
  // (i.e., a node whose keys are all < k1).
  return order();
}

KeyType InternalNode::keyAt(int aIndex) const {
  return fMappings[aIndex].first;
}

void InternalNode::setKeyAt(int aIndex, KeyType aKey) {
  fMappings[aIndex].first = aKey;
}

Node *InternalNode::firstChild() const { return fMappings.front().second; }

void InternalNode::populateNewRoot(Node *aOldNode, KeyType aNewKey,
                                   Node *aNewNode) {
  fMappings.push_back(std::make_pair(DUMMY_KEY, aOldNode));
  fMappings.push_back(std::make_pair(aNewKey, aNewNode));
}

int InternalNode::insertNodeAfter(Node *aOldNode, KeyType aNewKey,
                                  Node *aNewNode) {
  auto iter = fMappings.begin();
  for (; iter != fMappings.end() && iter->second != aOldNode; ++iter)
    ;
  fMappings.insert(iter + 1, std::make_pair(aNewKey, aNewNode));
  return size();
}

void InternalNode::remove(int aIndex) {
  fMappings.erase(fMappings.begin() + aIndex);
}

Node *InternalNode::removeAndReturnOnlyChild() {
  Node *firstChild = fMappings.front().second;
  fMappings.pop_back();
  return firstChild;
}

KeyType InternalNode::replaceAndReturnFirstKey() {
  KeyType newKey = fMappings[0].first;
  fMappings[0].first = DUMMY_KEY;
  return newKey;
}

void InternalNode::moveHalfTo(InternalNode *aRecipient) {
  aRecipient->copyHalfFrom(fMappings);
  size_t size = fMappings.size();
  for (size_t i = minSize(); i < size; ++i) {
    fMappings.pop_back();
  }
}

void InternalNode::copyHalfFrom(std::vector<MappingType> &aMappings) {
  for (size_t i = minSize(); i < aMappings.size(); ++i) {
    aMappings[i].second->setParent(this);
    fMappings.push_back(aMappings[i]);
  }
}

void InternalNode::moveAllTo(InternalNode *aRecipient, int aIndexInParent) {
  fMappings[0].first =
      static_cast<InternalNode *>(parent())->keyAt(aIndexInParent);
  aRecipient->copyAllFrom(fMappings);
  fMappings.clear();
}

void InternalNode::copyAllFrom(std::vector<MappingType> &aMappings) {
  for (auto mapping : aMappings) {
    mapping.second->setParent(this);
    fMappings.push_back(mapping);
  }
}

void InternalNode::moveFirstToEndOf(InternalNode *aRecipient) {
  aRecipient->copyLastFrom(fMappings.front());
  fMappings.erase(fMappings.begin());
  static_cast<InternalNode *>(parent())->setKeyAt(1, fMappings.front().first);
}

void InternalNode::copyLastFrom(MappingType aPair) {
  fMappings.push_back(aPair);
  // fMappings.back().first = fMappings.back().second->fMappings.front().first;
  fMappings.back().first = fMappings.back().second->firstKey();
  fMappings.back().second->setParent(this);
}

void InternalNode::moveLastToFrontOf(InternalNode *aRecipient,
                                     int aParentIndex) {
  aRecipient->copyFirstFrom(fMappings.back(), aParentIndex);
  fMappings.pop_back();
}

void InternalNode::copyFirstFrom(MappingType aPair, int aParentIndex) {
  fMappings.front().first =
      static_cast<InternalNode *>(parent())->keyAt(aParentIndex);
  fMappings.insert(fMappings.begin(), aPair);
  fMappings.front().first = DUMMY_KEY;
  fMappings.front().second->setParent(this);
  static_cast<InternalNode *>(parent())->setKeyAt(aParentIndex,
                                                  fMappings.front().first);
}

Node *InternalNode::lookup(KeyType aKey) const {
  auto locator = fMappings.begin();
  auto end = fMappings.end();
  while (locator != end && aKey >= locator->first) {
    ++locator;
  }
  // locator->first is now the least key k such that aKey < k.
  // One before is the greatest key k such that aKey >= k.
  --locator;
  return locator->second;
}

int InternalNode::nodeIndex(Node *aNode) const {
  for (size_t i = 0; i < size(); ++i) {
    if (fMappings[i].second == aNode) {
      return static_cast<int>(i);
    }
  }
  throw NodeNotFoundException(aNode->toString(), toString());
}

Node *InternalNode::neighbor(int aIndex) const {
  return fMappings[aIndex].second;
}

std::string InternalNode::toString(bool aVerbose) const {
  if (fMappings.empty()) {
    return "";
  }
  std::ostringstream keyToTextConverter;
  if (aVerbose) {
    keyToTextConverter << "[" << std::hex << this << std::dec << "]<"
                       << fMappings.size() << "> ";
  }
  auto entry = aVerbose ? fMappings.begin() : fMappings.begin() + 1;
  auto end = fMappings.end();
  bool first = true;
  while (entry != end) {
    if (first) {
      first = false;
    } else {
      keyToTextConverter << " ";
    }
    keyToTextConverter << std::dec << entry->first;
    if (aVerbose) {
      keyToTextConverter << "(" << std::hex << entry->second << std::dec << ")";
    }
    ++entry;
  }
  return keyToTextConverter.str();
}

void InternalNode::queueUpChildren(std::queue<Node *> *aQueue) {
  for (auto mapping : fMappings) {
    aQueue->push(mapping.second);
  }
}

const KeyType InternalNode::firstKey() const { return fMappings[1].first; }

// LeafNode.cpp
LeafNode::LeafNode(int aOrder) : fNext{nullptr}, Node(aOrder) {}

LeafNode::LeafNode(int aOrder, Node *aParent)
    : fNext{nullptr}, Node(aOrder, aParent) {}

LeafNode::~LeafNode() {
  for (auto mapping : fMappings) {
    delete mapping.second;
  }
}

bool LeafNode::isLeaf() const { return true; }

LeafNode *LeafNode::next() const { return fNext; }

void LeafNode::setNext(LeafNode *aNext) { fNext = aNext; }

int LeafNode::size() const { return static_cast<int>(fMappings.size()); }

int LeafNode::minSize() const { return order() / 2; }

int LeafNode::maxSize() const { return order() - 1; }

std::string LeafNode::toString(bool aVerbose) const {
  std::ostringstream keyToTextConverter;
  if (aVerbose) {
    keyToTextConverter << "[" << std::hex << this << std::dec << "]<"
                       << fMappings.size() << "> ";
  }
  bool first = true;
  for (auto mapping : fMappings) {
    if (first) {
      first = false;
    } else {
      keyToTextConverter << " ";
    }
    keyToTextConverter << mapping.first;
  }
  if (aVerbose) {
    keyToTextConverter << "[" << std::hex << fNext << ">";
  }
  return keyToTextConverter.str();
}

int LeafNode::createAndInsertRecord(KeyType aKey, ValueType aValue) {
  Record *existingRecord = lookup(aKey);
  if (!existingRecord) {
    Record *newRecord = new Record(aValue);
    insert(aKey, newRecord);
  }
  return static_cast<int>(fMappings.size());
}

void LeafNode::insert(KeyType aKey, Record *aRecord) {
  auto insertionPoint = fMappings.begin();
  auto end = fMappings.end();
  while (insertionPoint != end && insertionPoint->first < aKey) {
    ++insertionPoint;
  }
  fMappings.insert(insertionPoint, MappingType(aKey, aRecord));
}

Record *LeafNode::lookup(KeyType aKey) const {
  for (auto mapping : fMappings) {
    if (mapping.first == aKey) {
      return mapping.second;
    }
  }
  return nullptr;
}

void LeafNode::copyRangeStartingFrom(KeyType aKey,
                                     std::vector<EntryType> &aVector) {
  bool found = false;
  for (auto mapping : fMappings) {
    if (mapping.first == aKey) {
      found = true;
    }
    if (found) {
      aVector.push_back(
          std::make_tuple(mapping.first, mapping.second->value(), this));
    }
  }
}

void LeafNode::copyRangeUntil(KeyType aKey, std::vector<EntryType> &aVector) {
  bool found = false;
  for (auto mapping : fMappings) {
    if (!found) {
      aVector.push_back(
          std::make_tuple(mapping.first, mapping.second->value(), this));
    }
    if (mapping.first == aKey) {
      found = true;
    }
  }
}

void LeafNode::copyRange(std::vector<EntryType> &aVector) {
  for (auto mapping : fMappings) {
    aVector.push_back(
        std::make_tuple(mapping.first, mapping.second->value(), this));
  }
}

int LeafNode::removeAndDeleteRecord(KeyType aKey) {
  auto removalPoint = fMappings.begin();
  auto end = fMappings.end();
  while (removalPoint != end && removalPoint->first != aKey) {
    ++removalPoint;
  }
  if (removalPoint == end) {
    throw RecordNotFoundException(aKey);
  }
  auto record = *removalPoint;
  fMappings.erase(removalPoint);
  delete record.second;
  return static_cast<int>(fMappings.size());
}

const KeyType LeafNode::firstKey() const { return fMappings[0].first; }

void LeafNode::moveHalfTo(LeafNode *aRecipient) {
  aRecipient->copyHalfFrom(fMappings);
  size_t size = fMappings.size();
  for (size_t i = minSize(); i < size; ++i) {
    fMappings.pop_back();
  }
}

void LeafNode::copyHalfFrom(
    std::vector<std::pair<KeyType, Record *>> &aMappings) {
  for (size_t i = minSize(); i < aMappings.size(); ++i) {
    fMappings.push_back(aMappings[i]);
  }
}

void LeafNode::moveAllTo(LeafNode *aRecipient, int) {
  aRecipient->copyAllFrom(fMappings);
  fMappings.clear();
  aRecipient->setNext(next());
}

void LeafNode::copyAllFrom(
    std::vector<std::pair<KeyType, Record *>> &aMappings) {
  for (auto mapping : aMappings) {
    fMappings.push_back(mapping);
  }
}

void LeafNode::moveFirstToEndOf(LeafNode *aRecipient) {
  aRecipient->copyLastFrom(fMappings.front());
  fMappings.erase(fMappings.begin());
  static_cast<InternalNode *>(parent())->setKeyAt(1, fMappings.front().first);
}

void LeafNode::copyLastFrom(MappingType aPair) { fMappings.push_back(aPair); }

void LeafNode::moveLastToFrontOf(LeafNode *aRecipient, int aParentIndex) {
  aRecipient->copyFirstFrom(fMappings.back(), aParentIndex);
  fMappings.pop_back();
}

void LeafNode::copyFirstFrom(MappingType aPair, int aParentIndex) {
  fMappings.insert(fMappings.begin(), aPair);
  static_cast<InternalNode *>(parent())->setKeyAt(aParentIndex,
                                                  fMappings.front().first);
}

// Printer.cpp
Printer::Printer() : fVerbose(false) {}

bool Printer::verbose() const { return fVerbose; }

void Printer::setVerbose(bool aVerbose) { fVerbose = aVerbose; }

void Printer::printTree(Node *aRoot) const {
  if (!aRoot) {
    printEmptyTree();
  } else {
    printNonEmptyTree(aRoot);
  }
}

void Printer::printEmptyTree() const {
  std::cout << "Empty tree." << std::endl;
}

void Printer::printNonEmptyTree(Node *aRoot) const {
  std::queue<Node *> queue0;
  std::queue<Node *> queue1;
  auto currentRank = &queue0;
  auto nextRank = &queue1;
  currentRank->push(aRoot);
  while (!currentRank->empty()) {
    printCurrentRank(currentRank, nextRank);
    auto tmp = currentRank;
    currentRank = nextRank;
    nextRank = tmp;
  }
}

void Printer::printCurrentRank(std::queue<Node *> *aCurrentRank,
                               std::queue<Node *> *aNextRank) const {
  std::cout << "|";
  while (!aCurrentRank->empty()) {
    Node *currentNode = aCurrentRank->front();
    std::cout << " " << currentNode->toString(verbose());
    std::cout << " |";
    if (!currentNode->isLeaf()) {
      auto internalNode = static_cast<InternalNode *>(currentNode);
      internalNode->queueUpChildren(aNextRank);
    }
    aCurrentRank->pop();
  }
  std::cout << std::endl;
}

void Printer::printLeaves(Node *aRoot) {
  if (!aRoot) {
    printEmptyTree();
    return;
  }
  auto node = aRoot;
  while (!node->isLeaf()) {
    node = static_cast<InternalNode *>(node)->firstChild();
  }
  auto leafNode = static_cast<LeafNode *>(node);
  while (leafNode) {
    std::cout << "| ";
    std::cout << leafNode->toString(fVerbose);
    leafNode = leafNode->next();
  }
  std::cout << " |" << std::endl;
}

// Exceptions.cpp
LeafNotFoundException::LeafNotFoundException(KeyType aKey) : fKey{aKey} {}

const char *LeafNotFoundException::what() const noexcept {
  std::ostringstream ss;
  ss << "Key not found in any leaf node:  ";
  ss << fKey;
  static std::string message;
  message = ss.str();
  return message.c_str();
}

NodeNotFoundException::NodeNotFoundException(std::string aSearchedNode,
                                             std::string aContainingNode)
    : fSearchedNode{aSearchedNode}, fContainingNode{aContainingNode} {}

const char *NodeNotFoundException::what() const noexcept {
  std::ostringstream ss;
  ss << "Node |" << fSearchedNode << "| not found";
  ss << " as a child of node ";
  ss << fContainingNode;
  static std::string message;
  message = ss.str();
  return message.c_str();
}

RecordNotFoundException::RecordNotFoundException(KeyType aKey) : fKey{aKey} {}

const char *RecordNotFoundException::what() const noexcept {
  std::ostringstream ss;
  ss << "Record not found with key:  " << fKey;
  static std::string message;
  message = ss.str();
  return message.c_str();
}

// BPlusTree.cpp
BPlusTree::BPlusTree(int aOrder) : fOrder{aOrder}, fRoot{nullptr} {}

bool BPlusTree::isEmpty() const { return !fRoot; }

// INSERTION

void BPlusTree::insert(KeyType aKey, ValueType aValue) {
  if (isEmpty()) {
    startNewTree(aKey, aValue);
  } else {
    insertIntoLeaf(aKey, aValue);
  }
}

void BPlusTree::startNewTree(KeyType aKey, ValueType aValue) {
  LeafNode *newLeafNode = new LeafNode(fOrder);
  newLeafNode->createAndInsertRecord(aKey, aValue);
  fRoot = newLeafNode;
}

void BPlusTree::insertIntoLeaf(KeyType aKey, ValueType aValue) {
  LeafNode *leafNode = findLeafNode(aKey);
  if (!leafNode) {
    throw LeafNotFoundException(aKey);
  }
  Record *record = leafNode->lookup(aKey);
  if (record) {
    record->setValue(aValue);
    return;
  }
  int newSize = leafNode->createAndInsertRecord(aKey, aValue);
  if (newSize > leafNode->maxSize()) {
    LeafNode *newLeaf = split(leafNode);
    newLeaf->setNext(leafNode->next());
    leafNode->setNext(newLeaf);
    KeyType newKey = newLeaf->firstKey();
    insertIntoParent(leafNode, newKey, newLeaf);
  }
}

void BPlusTree::insertIntoParent(Node *aOldNode, KeyType aKey, Node *aNewNode) {
  InternalNode *parent = static_cast<InternalNode *>(aOldNode->parent());
  if (parent == nullptr) {
    fRoot = new InternalNode(fOrder);
    parent = static_cast<InternalNode *>(fRoot);
    aOldNode->setParent(parent);
    aNewNode->setParent(parent);
    parent->populateNewRoot(aOldNode, aKey, aNewNode);
  } else {
    int newSize = parent->insertNodeAfter(aOldNode, aKey, aNewNode);
    if (newSize > parent->maxSize()) {
      InternalNode *newNode = split(parent);
      KeyType newKey = newNode->replaceAndReturnFirstKey();
      insertIntoParent(parent, newKey, newNode);
    }
  }
}

template <typename T> T *BPlusTree::split(T *aNode) {
  T *newNode = new T(fOrder, aNode->parent());
  aNode->moveHalfTo(newNode);
  return newNode;
}

// REMOVAL

void BPlusTree::remove(KeyType aKey) {
  if (isEmpty()) {
    return;
  } else {
    removeFromLeaf(aKey);
  }
}

void BPlusTree::removeFromLeaf(KeyType aKey) {
  LeafNode *leafNode = findLeafNode(aKey);
  if (!leafNode) {
    return;
  }
  if (!leafNode->lookup(aKey)) {
    return;
  }
  int newSize = leafNode->removeAndDeleteRecord(aKey);
  if (newSize < leafNode->minSize()) {
    coalesceOrRedistribute(leafNode);
  }
}

template <typename N> void BPlusTree::coalesceOrRedistribute(N *aNode) {
  if (aNode->isRoot()) {
    adjustRoot();
    return;
  }
  auto parent = static_cast<InternalNode *>(aNode->parent());
  int indexOfNodeInParent = parent->nodeIndex(aNode);
  int neighborIndex = (indexOfNodeInParent == 0) ? 1 : indexOfNodeInParent - 1;
  N *neighborNode = static_cast<N *>(parent->neighbor(neighborIndex));
  if (aNode->size() + neighborNode->size() <= neighborNode->maxSize()) {
    coalesce(neighborNode, aNode, parent, indexOfNodeInParent);
  } else {
    redistribute(neighborNode, aNode, parent, indexOfNodeInParent);
  }
}

template <typename N>
void BPlusTree::coalesce(N *aNeighborNode, N *aNode, InternalNode *aParent,
                         int aIndex) {
  if (aIndex == 0) {
    std::swap(aNode, aNeighborNode);
    aIndex = 1;
  }
  aNode->moveAllTo(aNeighborNode, aIndex);
  aParent->remove(aIndex);
  if (aParent->size() < aParent->minSize()) {
    coalesceOrRedistribute(aParent);
  }
  delete aNode;
}

template <typename N>
void BPlusTree::redistribute(N *aNeighborNode, N *aNode, InternalNode *aParent,
                             int aIndex) {
  if (aIndex == 0) {
    aNeighborNode->moveFirstToEndOf(aNode);
  } else {
    aNeighborNode->moveLastToFrontOf(aNode, aIndex);
  }
}

void BPlusTree::adjustRoot() {
  if (!fRoot->isLeaf() && fRoot->size() == 1) {
    auto discardedNode = static_cast<InternalNode *>(fRoot);
    fRoot = static_cast<InternalNode *>(fRoot)->removeAndReturnOnlyChild();
    fRoot->setParent(nullptr);
    delete discardedNode;
  } else if (!fRoot->size()) {
    delete fRoot;
    fRoot = nullptr;
  }
}

// UTILITIES AND PRINTING

LeafNode *BPlusTree::findLeafNode(KeyType aKey, bool aPrinting, bool aVerbose) {
  if (isEmpty()) {
    if (aPrinting) {
      std::cout << "Not found: empty tree." << std::endl;
    }
    return nullptr;
  }
  auto node = fRoot;
  if (aPrinting) {
    std::cout << "Root: ";
    if (fRoot->isLeaf()) {
      std::cout << "\t" << static_cast<LeafNode *>(fRoot)->toString(aVerbose);
    } else {
      std::cout << "\t"
                << static_cast<InternalNode *>(fRoot)->toString(aVerbose);
    }
    std::cout << std::endl;
  }
  while (!node->isLeaf()) {
    auto internalNode = static_cast<InternalNode *>(node);
    if (aPrinting && node != fRoot) {
      std::cout << "\tNode: " << internalNode->toString(aVerbose) << std::endl;
    }
    node = internalNode->lookup(aKey);
  }
  return static_cast<LeafNode *>(node);
}

void BPlusTree::readInputFromFile(std::string aFileName) {
  int key;
  std::ifstream input(aFileName);
  while (input) {
    input >> key;
    insert(key, key);
  }
}

void BPlusTree::print(bool aVerbose) {
  fPrinter.setVerbose(aVerbose);
  fPrinter.printTree(fRoot);
}

void BPlusTree::printLeaves(bool aVerbose) {
  fPrinter.setVerbose(aVerbose);
  fPrinter.printLeaves(fRoot);
}

void BPlusTree::destroyTree() {
  if (fRoot->isLeaf()) {
    delete static_cast<LeafNode *>(fRoot);
  } else {
    delete static_cast<InternalNode *>(fRoot);
  }
  fRoot = nullptr;
}

void BPlusTree::printValue(KeyType aKey, bool aVerbose) {
  printValue(aKey, false, aVerbose);
}

void BPlusTree::printValue(KeyType aKey, bool aPrintPath, bool aVerbose) {
  LeafNode *leaf = findLeafNode(aKey, aPrintPath, aVerbose);
  if (!leaf) {
    std::cout << "Leaf not found with key " << aKey << "." << std::endl;
    return;
  }
  if (aPrintPath) {
    std::cout << "\t";
  }
  std::cout << "Leaf: " << leaf->toString(aVerbose) << std::endl;
  Record *record = leaf->lookup(aKey);
  if (!record) {
    std::cout << "Record not found with key " << aKey << "." << std::endl;
    return;
  }
  if (aPrintPath) {
    std::cout << "\t";
  }
  std::cout << "Record found at location " << std::hex << record << std::dec
            << ":" << std::endl;
  std::cout << "\tKey: " << aKey << "   Value: " << record->value()
            << std::endl;
}

void BPlusTree::printPathTo(KeyType aKey, bool aVerbose) {
  printValue(aKey, true, aVerbose);
}

void BPlusTree::printRange(KeyType aStart, KeyType aEnd) {
  auto rangeVector = range(aStart, aEnd);
  for (auto entry : rangeVector) {
    std::cout << "Key: " << std::get<0>(entry);
    std::cout << "    Value: " << std::get<1>(entry);
    std::cout << "    Leaf: " << std::hex << std::get<2>(entry) << std::dec
              << std::endl;
  }
}

std::vector<BPlusTree::EntryType> BPlusTree::range(KeyType aStart,
                                                   KeyType aEnd) {
  auto startLeaf = findLeafNode(aStart);
  auto endLeaf = findLeafNode(aEnd);
  std::vector<std::tuple<KeyType, ValueType, LeafNode *>> entries;
  if (!startLeaf || !endLeaf) {
    return entries;
  }
  startLeaf->copyRangeStartingFrom(aStart, entries);
  startLeaf = startLeaf->next();
  while (startLeaf != endLeaf) {
    startLeaf->copyRange(entries);
    startLeaf = startLeaf->next();
  }
  startLeaf->copyRangeUntil(aEnd, entries);
  return entries;
}
