#include "Node.h"

Node::Node(const vec2& bottomLeft, const vec2& topRight, int index) : treeLayerIndex(index) {
  bottomLeftCorner = bottomLeft;
  topRightCorner = topRight;
  bottomRightCorner = vec2(topRight.x, bottomLeft.y);
  topLeftCorner = vec2(bottomLeft.x, topRight.y);
};

void Node::addChild(std::shared_ptr<Node> child) {
  children.push_back(child);
  child->parent = shared_from_this();
}