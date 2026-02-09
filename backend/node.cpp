#include "node.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

Node::Node(std::string name)
    : parent(0)
{
    this->name = name;
}

Node::~Node()
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        // delete children[i];
        //  todo fix
    }

    for (size_t i = 0; i < components.size(); ++i)
    {
        // delete components[i];
        //  todo fix
    }
}

void Node::addChild(std::shared_ptr<Node> child)
{
    if (!child)
        return;

    child->parent = this;
    children.push_back(child);
}

void Node::removeChild(std::shared_ptr<Node> child)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i] == child)
        {
            // todo fix

            // delete children[i];
            // children.erase(children.begin() + i);
            return;
        }
    }
}

void Node::addComponent(std::shared_ptr<Component> component)
{
    if (!component)
        return;
    component->setNode(this);
    components.push_back(component);

    if (Script *script = dynamic_cast<Script *>(component.get()))
    {
        // i need to update ts
        activeScripts.push_back(script);
    }

    if (Sprite *sprite = dynamic_cast<Sprite *>(component.get()))
    {
        if (currentSprite)
        {
            // todo throw an error here...
        }
        currentSprite = sprite;
    }
}

void Node::removeComponent(std::shared_ptr<Component> component)
{
    for (size_t i = 0; i < components.size(); ++i)
    {
        if (components[i] == component)
        {
            if (Script *script = dynamic_cast<Script *>(components[i].get()))
            {
                // activeScripts.erase(std::find(activeScripts.begin(), activeScripts.end(), script));
                //  todo: cuz im too lazy
            }

            if (Sprite *sprite = dynamic_cast<Sprite *>(components[i].get()))
            {
                // todo: lazy
            }

            // delete components[i];
            // components.erase(components.begin() + i);
            // todo
            return;
        }
    }
}

template <typename T>
T *Node::getComponent()
{
    // todo make this return a shared pointer
    for (size_t i = 0; i < components.size(); ++i)
    {
        T *c = dynamic_cast<T *>(components[i].get());
        if (c)
            return c;
    }
    return 0;
}

void Node::render(Graphics &graphics, Engine *engine)
{
    for (size_t i = 0; i < this->components.size(); i++)
    {
        Sprite *sprite = dynamic_cast<Sprite *>(components[i].get());
        if (sprite)
        {
            if (sprite && sprite->visible)
            {
                graphics.renderSprite(*this, engine);
            }
        }
    }

    for (size_t i = 0; i < this->children.size(); i++)
    {
        children[i]->render(graphics, engine);
    }
}
void Node::update(float deltaTime)
{
    if (parent)
    {
        globalTransform = parent->globalTransform.applyTo(transform);
    }
    else
    {
        globalTransform.transformation = transform.transformation;
        globalTransform.position = transform.position;
    }

    for (size_t i = 0; i < components.size(); i++)
    {
        components[i]->update();
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->update(deltaTime);
    }

    for (Script *script : activeScripts)
    {

        script->update(deltaTime); // todo change
    }
}

Node *Node::getChild(std::string name)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->name == name)
        {
            return children[i].get();
        }
    }
    return nullptr;
}

Node *Node::getChildByIndex(int index)
{
    if (index < 0 || index >= children.size())
    {
        return nullptr;
    }
    return children[index].get();
}

Component *Node::getComponentByIndex(int index)
{
    if (index < 0 || index >= components.size())
    {
        return nullptr;
    }
    return components[index].get();
}