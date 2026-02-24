#pragma once
#include "definitions.hpp"
#include "graphics.hpp"
#include "utils.hpp"
#include "components.hpp"
#include <iostream>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

/**
 * # Base node class
 * Everything in Calamity Engine is composed of nodes. An analogy would be that a node is like a GameObject in Unity.
 * It can have components, children, and a transform. It also has a name for easy identification.
 * The node class is responsible for updating and rendering itself and its children, as well as managing its components.
 *
 * It has methods available for adding and removing children and components, as well as getting them by name/index.
 * Here is an example of a node with a sprite component and a script component:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>("My Node");
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
 * std::shared_ptr<MyScript> script = std::make_shared<MyScript>();
 * node->addComponent(sprite);
 * node->addComponent(script);
 *
 * node->initialize(); // Make sure to call initialize if this isn't a child of the root node!
 * ```
 */
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