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
	void initialize();

	std::vector<Script *> activeScripts;

	template <class Archive>
	void save(Archive &ar) const
	{
		// ar(name, transform, children, components, parent, currentSprite);
		ar(CEREAL_NVP(name), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(children));
	}

	template <class Archive>
	void load(Archive &ar)
	{
		ar(CEREAL_NVP(name), CEREAL_NVP(transform), CEREAL_NVP(components), CEREAL_NVP(children));

		// rewire component back-pointers
		for (auto &c : components)
		{
			c->setNode(this);
			if (Script *s = dynamic_cast<Script *>(c.get()))
				activeScripts.push_back(s);
			if (Sprite *s = dynamic_cast<Sprite *>(c.get()))
				currentSprite = s;
		}

		// rewire child parent pointers
		for (auto &child : children)
			child->parent = this;
	}
};