#ifndef NODE_HPP
#define NODE_HPP

#include "definitions.hpp"
#include "graphics.hpp"
#include "utils.hpp"

class Node
{
public:
	std::string name;
	Transform transform;
	Transform globalTransform;

	Node *parent;
	std::vector<Node *> children;
	std::vector<Component *> components;

	Node();
	~Node();

	// children
	void addChild(Node *child);
	void removeChild(Node *child);
	Node *getChild(std::string name);
	Node *getChildByIndex(int index);

	// components
	void addComponent(Component *component);
	void removeComponent(Component *component);
	template <typename T>
	T *getComponent();
	Component *getComponentByIndex(int index);

	void render(Graphics &graphics);
	void update();
};

#endif
