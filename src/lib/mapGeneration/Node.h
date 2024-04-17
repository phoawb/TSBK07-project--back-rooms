#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "VectorUtils4.h"
#include "components/Renderable.hpp"
#include "components/Transform.hpp"

class Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::weak_ptr<Node> WeakNodePtr;

class Node : public std::enable_shared_from_this<Node>, public Transform, public Renderable {
 public:
  std::vector<NodePtr> children;
  vec2 bottomLeftCorner, topLeftCorner, bottomRightCorner, topRightCorner;
  int treeLayerIndex;
  WeakNodePtr parent;

  Node() : treeLayerIndex(0){};  // default constructor
  Node(const vec2& bottomLeft, const vec2& topRight, int index);

  void addChild(std::shared_ptr<Node> child);
  int getWidth() { return topRightCorner.x - bottomLeftCorner.x; }
  int getHeight() { return topRightCorner.y - bottomLeftCorner.y; }
};
#endif  // NODE_H
