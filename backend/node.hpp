#pragma once
#include "definitions.hpp"
#include "graphics.hpp"
#include "utils.hpp"
#include "components.hpp"
#include <iostream>

class Node
{
public:
	std::string name;
	Transform transform;
	Transform globalTransform;

	Node *parent;
	Sprite *currentSprite = nullptr;
	std::vector<std::shared_ptr<Node>> children;
	std::vector<std::shared_ptr<Component>> components;

	Node(std::string name = "Node");
	~Node();

	// children
	void addChild(std::shared_ptr<Node> child);
	void removeChild(std::shared_ptr<Node> child);
	Node *getChild(std::string name);
	Node *getChildByIndex(int index);

	// components
	void addComponent(std::shared_ptr<Component> component);
	void removeComponent(std::shared_ptr<Component> component);
	template <typename T>
	T *getComponent();
	Component *getComponentByIndex(int index);

	void render(Graphics &graphics, Engine *engine);
	void update(float deltaTime);

	std::vector<Script *> activeScripts;

	template <class Archive>
	void save(Archive &ar) const
	{
		// ar(name, transform, children, components, activeScripts, parent, currentSprite);
		ar(name, transform, components);
	}

	template <class Archive>
	void load(Archive &ar) {}
};