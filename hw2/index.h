// ref: https://github.com/deepaktabraham/BPlus-Tree

#ifndef INDEX_H_
#define INDEX_H_

#include <cstdlib>
#include <exception>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#define VERSION "2.1.0"

const int DEFAULT_ORDER{4};

// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
const int MIN_ORDER{DEFAULT_ORDER - 1};
const int MAX_ORDER{20};

// Size of the buffer used to get the arguments (1 or 2) of the
// "i" interactive instruction.
const int BUFFER_SIZE{256};

using KeyType = int64_t;
using ValueType = int64_t;

// Node.hpp

// Key used where only the entry's pointer has meaning.
const KeyType DUMMY_KEY{-1};

// Abstract class.
class Node {
public:
  explicit Node(int aOrder);
  explicit Node(int aOrder, Node *aParent);
  virtual ~Node();
  int order() const;
  Node *parent() const;
  void setParent(Node *aParent);
  bool isRoot() const;
  virtual bool isLeaf() const = 0;
  virtual int size() const = 0;
  virtual int minSize() const = 0;
  virtual int maxSize() const = 0;
  virtual std::string toString(bool aVerbose = false) const = 0;
  virtual const KeyType firstKey() const = 0;

private:
  const int fOrder;
  Node *fParent;
};

// Record.hpp
class Record {
public:
  explicit Record(ValueType aValue);
  ValueType value() const;
  void setValue(ValueType aValue);
  std::string toString() const;

private:
  Record() : fValue(0) {}
  ValueType fValue;
};

// InternalNode.hpp
class InternalNode : public Node {
public:
  explicit InternalNode(int aOrder);
  explicit InternalNode(int aOrder, Node *aParent);
  ~InternalNode() override;
  using MappingType = std::pair<KeyType, Node *>;
  bool isLeaf() const override;
  int size() const override;
  int minSize() const override;
  int maxSize() const override;
  KeyType keyAt(int aIndex) const;
  void setKeyAt(int aIndex, KeyType aKey);
  Node *firstChild() const;
  void populateNewRoot(Node *aOldNode, KeyType aNewKey, Node *aNewNode);
  int insertNodeAfter(Node *aOldNode, KeyType aNewKey, Node *aNewNode);
  void remove(int aIndex);
  Node *removeAndReturnOnlyChild();
  KeyType replaceAndReturnFirstKey();
  void moveHalfTo(InternalNode *aRecipient);
  void moveAllTo(InternalNode *aRecipient, int aIndexInParent);
  void moveFirstToEndOf(InternalNode *aRecipient);
  void moveLastToFrontOf(InternalNode *aRecipient, int aParentIndex);
  Node *lookup(KeyType aKey) const;
  int nodeIndex(Node *aNode) const;
  Node *neighbor(int aIndex) const;
  std::string toString(bool aVerbose = false) const override;
  void queueUpChildren(std::queue<Node *> *aQueue);
  const KeyType firstKey() const override;

private:
  void copyHalfFrom(std::vector<MappingType> &aMappings);
  void copyAllFrom(std::vector<MappingType> &aMappings);
  void copyLastFrom(MappingType aPair);
  void copyFirstFrom(MappingType aPair, int aParentIndex);
  std::vector<MappingType> fMappings;
};

// LeafNode.hpp
class LeafNode : public Node {
public:
  explicit LeafNode(int aOrder);
  explicit LeafNode(int aOrder, Node *aParent);
  ~LeafNode() override;
  using MappingType = std::pair<KeyType, Record *>;
  using EntryType = std::tuple<KeyType, ValueType, LeafNode *>;
  bool isLeaf() const override;
  LeafNode *next() const;
  void setNext(LeafNode *aNext);
  int size() const override;
  int minSize() const override;
  int maxSize() const override;
  int createAndInsertRecord(KeyType aKey, ValueType aValue);
  void insert(KeyType aKey, Record *aRecord);
  Record *lookup(KeyType aKey) const;
  int removeAndDeleteRecord(KeyType aKey);
  const KeyType firstKey() const override;
  void moveHalfTo(LeafNode *aRecipient);
  void moveAllTo(LeafNode *aRecipient, int /* Unused */);
  void moveFirstToEndOf(LeafNode *aRecipient);
  void moveLastToFrontOf(LeafNode *aRecipient, int aParentIndex);
  void copyRangeStartingFrom(KeyType aKey, std::vector<EntryType> &aVector);
  void copyRangeUntil(KeyType aKey, std::vector<EntryType> &aVector);
  void copyRange(std::vector<EntryType> &aVector);
  std::string toString(bool aVerbose = false) const override;

private:
  void copyHalfFrom(std::vector<MappingType> &aMappings);
  void copyAllFrom(std::vector<MappingType> &aMappings);
  void copyLastFrom(MappingType aPair);
  void copyFirstFrom(MappingType aPair, int aParentIndex);
  std::vector<MappingType> fMappings;
  LeafNode *fNext;
};

// Printer.hpp
class Printer {
public:
  Printer();
  bool verbose() const;
  void setVerbose(bool aVerbose);
  void printTree(Node *aRoot) const;
  void printLeaves(Node *aRoot);

private:
  void printEmptyTree() const;
  void printNonEmptyTree(Node *aRoot) const;
  void printCurrentRank(std::queue<Node *> *aCurrentRank,
                        std::queue<Node *> *aNextRank) const;
  bool fVerbose;
};

// Exceptions.hpp
class LeafNotFoundException : public std::exception {
public:
  explicit LeafNotFoundException(KeyType);
  const char *what() const noexcept override;

private:
  KeyType fKey;
};

class NodeNotFoundException : public std::exception {
public:
  explicit NodeNotFoundException(std::string, std::string);
  const char *what() const noexcept override;

private:
  std::string fSearchedNode;
  std::string fContainingNode;
};

class RecordNotFoundException : public std::exception {
public:
  explicit RecordNotFoundException(KeyType);
  const char *what() const noexcept override;

private:
  KeyType fKey;
};

// BPlusTree.hpp
class BPlusTree {
public:
  /// Sole constructor.  Accepts an optional order for the B+ Tree.
  /// The default order will provide a reasonable demonstration of the
  /// data structure and its operations.
  explicit BPlusTree(int aOrder = DEFAULT_ORDER);

  /// The type used in the API for inserting a new key-value pair
  /// into the tree.  The third item is the type of the Node into
  /// which the key will be inserted.
  using EntryType = std::tuple<KeyType, ValueType, LeafNode *>;

  /// Returns true if this B+ tree has no keys or values.
  bool isEmpty() const;

  /// Insert a key-value pair into this B+ tree.
  void insert(KeyType aKey, ValueType aValue);

  /// Remove a key and its value from this B+ tree.
  void remove(KeyType aKey);

  /// Print this B+ tree to stdout using a simple command-line
  /// ASCII graphic scheme.
  /// @param[in] aVerbose Determins whether printing should include addresses.
  void print(bool aVerbose = false);

  /// Print the bottom rank of this B+ tree, consisting of its leaves.
  /// This shows all the keys in the B+ tree in sorted order.
  /// @param[in] aVerbose Determins whether printing should include addresses.
  void printLeaves(bool aVerbose = false);

  /// Print the value associated with a given key, along with the address
  /// at which the tree stores that value.
  /// @param[in] aVerbose Determines whether printing should include addresses.
  void printValue(KeyType aKey, bool aVerbose = false);

  /// Print the path from the root to the leaf bearing key aKey.
  /// @param[in] aVerbose Determines whether printing should include addresses.
  void printPathTo(KeyType aKey, bool aVerbose = false);

  /// Print key, value, and address for each item in the range
  /// from aStart to aEnd, including both.
  void printRange(KeyType aStart, KeyType aEnd);

  /// Remove all elements from the B+ tree. You can then build
  /// it up again by inserting new elements into it.
  void destroyTree();

  /// Read elements to be inserted into the B+ tree from a text file.
  /// Each new element should consist of a single integer on a line by itself.
  /// This B+ tree treats each such input as both a new value and the key
  /// under which to store it.
  void readInputFromFile(std::string aFileName);

private:
  void startNewTree(KeyType aKey, ValueType aValue);
  void insertIntoLeaf(KeyType aKey, ValueType aValue);
  void insertIntoParent(Node *aOldNode, KeyType aKey, Node *aNewNode);
  template <typename T> T *split(T *aNode);
  void removeFromLeaf(KeyType aKey);
  template <typename N> void coalesceOrRedistribute(N *aNode);
  template <typename N>
  void coalesce(N *aNeighborNode, N *aNode, InternalNode *aParent, int aIndex);
  template <typename N>
  void redistribute(N *aNeighborNode, N *aNode, InternalNode *aParent,
                    int aIndex);
  void adjustRoot();
  LeafNode *findLeafNode(KeyType aKey, bool aPrinting = false,
                         bool aVerbose = false);
  void printValue(KeyType aKey, bool aPrintPath, bool aVerbose);
  std::vector<EntryType> range(KeyType aStart, KeyType aEnd);
  const int fOrder;
  Node *fRoot;
  Printer fPrinter;
};

#endif
