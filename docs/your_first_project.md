# Your first project {#your_first_project}

First off, if you're new to C++ maybe don't start out with my engine since I'm assuming that anyone who's reading this [has some basic C++ knowledge](https://learncpp.com/). You should also probably be familiar with how a game engine like Godot or Unity works.

Generally, any project that's built with this framework should have this general structure:
```
main.cpp
CMakeLists.txt
scripts/
    # scripts go here
assets/
    # assets go here
```
Obviously this isnt required but I heavily recommend it.
Also, while I did write a lot of documentation for every class (documentation which I think is decent), make sure to also look at the examples folder for different features of the engine for actual real code examples which compile!

## `main.cpp`

There is some boilerplate code that goes into starting a project:

```cpp
// includes go here //

int main() {
    Physics physics = Physics(); // always initialize physics before engine
    Logger::init();
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics();
    InputRegistry inputRegistry = InputRegistry();
    Services::init(&graphics, &engine, &input, &inputRegistry, &physics);

    // here initialize any nodes that you want

    engine.initialize();
    while(!input.shouldQuit) {
        engine.update();
        engine.render(graphics);
    }
    
    engine.shutdown();
}
```

## Initializing nodes and components

### Preface
I use [a library called cereal](https://uscilab.github.io/cereal/) to serialize & deserialize nodes into/from json files or strings. You should probably [read it's documentation](https://uscilab.github.io/cereal/) to familiarize yourself with how it works. However, it should be simple enough.

Cereal does NOT like raw pointers, so, every node and component has to be a shared pointer (`std::shared_ptr`). 
### Code
Let's take, for example, initializing the camera:
```cpp
std::shared_ptr<Node> cameraNode = std::make_shared<Node>(); // make the node
std::shared_ptr<Camera> camera = std::make_shared<Camera>(); // make the component
cameraNode->addComponent(camera); // adding the camera component to the node
engine.root.addChild(cameraNode); // adding the node as a child to the root node of the engine
```

Any nodes that you want to be visible on screen have to be a child of the engine's root node.

## Scripts and other components

Scripts are genuinely just classes in header files that extend components. Since scripts extend components you get convenient virtual functions such as: initialize, update, physics update (planned). (that's it)

Since [cereal](https://uscilab.github.io/cereal/) doesn't like it when components don't have save and load functions, scripts have to include them. I decided that's probably the best way to go about it since developers (YOU) get more control over what's saved and what isn't.

Here is an example script for moving the camera using the input system:

```cpp
//////////////////////////////
// scripts/cameraScript.hpp //
//////////////////////////////
#pragma once

// you HAVE to include these two
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>
// other includes go here

class CameraScript : public Script
{
    Node *node;
    Input *pe;

public:
    template <class Archive>
    void save(Archive &ar) const {}

    template <class Archive>
    void load(Archive &ar) {}

    void initialize()
    {
        // usually you initialize variables in the initialize function
        node = this->getNode();
        pe = Services::input();
    }

    void update(float deltaTime) {
        // PS, you have to add these actions yourself to the input registry, look at the input example's main.cpp file.
        auto vec = Services::input()->getVector("left", "right", "up", "down");
        node->transform.position = node->transform.position + (vec * deltaTime);
    }
};

// you also have to do this at the bottom of your script file
// otherwise your game won't compile
CEREAL_REGISTER_TYPE(CameraScript);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Script, CameraScript);
```

And for this script to actually work, you have to add it to the camera node (in our case). So let's go back to the code we had earlier and update it to add the example script:

```cpp
// don't forget to include the script :)
#include "scripts/cameraScript.hpp"

std::shared_ptr<Node> cameraNode = std::make_shared<Node>(); // make the node
cameraNode->addComponent(std::make_shared<Camera>()); // add the camera component to the node
cameraNode->addComponent(std::make_shared<CameraScript>()); // add the script to the camera node
engine.root.addChild(cameraNode); // add the node as a child to the root node of the engine
```