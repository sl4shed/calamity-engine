#pragma once
#include "../definitions.hpp"
#include "../../services/services.hpp"
#include "../../utils/utils.hpp"
#include "components.hpp"
#include "../../services/input/keycode.hpp"
#include <iostream>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

/**
 * # Base node class
 * Everything in Calamity Engine is composed of nodes. An analogy would be that a node is like a GameObject in Unity.
 * It can have components, children, and a transform. It also has a name for easy identification and lookup.
 * The node class is responsible for updating and rendering itself and its children, as well as managing its components.
 *
 * It has methods available for adding and removing children and components, as well as getting them by name/index.
 * Here is an example of a node with a sprite component and a script component:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>("My Node");
 * std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
 * node->addComponent(sprite);
 * node->addComponent(std::make_shared<MyScript>());
 * ```
 */
class Node : public std::enable_shared_from_this<Node>
{
public:
	std::string name;
	Transform transform;
	Transform globalTransform;

	Node *parent;
	std::vector<std::shared_ptr<Node>> children;
	std::vector<std::shared_ptr<Component>> components;

	Node(const std::string& name = "Node");
	~Node();
	void free();

	// children
	void addChild(std::shared_ptr<Node> child);
	void removeChild(std::shared_ptr<Node> child);
	std::shared_ptr<Node> getChild(std::string name);
	std::shared_ptr<Node> getChildByIndex(int index);

	// components
	void addComponent(std::shared_ptr<Component> component);
	void removeComponent(std::shared_ptr<Component> component);
	template <typename T>
	std::shared_ptr<T> getComponent() {
		for (size_t i = 0; i < components.size(); ++i)
		{
			if (T *c = dynamic_cast<T *>(components[i].get()))
				return std::dynamic_pointer_cast<T>(components[i]);
		}
		return nullptr;
	};
	std::shared_ptr<Component> getComponentByIndex(int index);

	// state functions
	void render(Graphics &graphics, Engine *engine) const;
	void update(float deltaTime);
	void physicsUpdate() const; // runs locked at 60fps
	void initialize();
	void input(InputEvent& event) const;
	void exit() const;

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
		}

		// rewire child parent pointers
		for (auto &child : children)
			child->parent = this;
	}
};