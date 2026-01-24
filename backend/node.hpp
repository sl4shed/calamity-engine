#pragma once
#include "definitions.hpp"
#include "graphics.hpp"
#include "utils.hpp"
#include "components.hpp"

class Node
{
public:
	std::string name;
	Transform transform;
	Transform globalTransform;

	Node *parent;
	Sprite *currentSprite = nullptr;
	std::vector<Node *> children;
	std::vector<Component *> components;

	Node(std::string name = "Node");
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
	void update(float deltaTime);

	std::vector<Script *> activeScripts;
};