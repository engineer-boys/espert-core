# SCENE SYSTEM

Scene system's architecture is build upon 2 paradigms - entity component 
system ([entt](https://github.com/skypjack/entt) library) and scene graph. Scene consists of 3 main classes. Namely 
1. Entity - responsible for managing object's components
2. Node - scene graph's Node, manages Entity taking into account relations with other Nodes. It can posess multiple 
children and a single parent (just like in a regular tree).
3. Scene - registers each Entity that is to be rendered. It contains 
   - a root Node which is the root of scene's graph. All created Nodes must be attached to the root in order to be
    processed by Renderer.
   - vector of Cameras which user can create and add. Also, there is a static pointer on Camera that is currently in use.  

User can interact with Entities directly through it's class, or by applying an Action on Node. This way changes can be 
propagated down the graph applying the same [Action](#action) on Node's children.  
As mentioned, every object (Entity) can have it's own components, and there are some base ones predefined
(i.e. TransformComponent) but more can be created by the user if needed.

## 1. Entity
```Python 
class Entity:
    #Class members:
    self.handle # handle to entt entity
    self.scene  # pointer to scene on which entity is registered

    def add_component<T, Args...>(args...) -> T:
    # Constructs and adds component T to entity using provided args
    
    def get_transform<T>() -> T:
    # Returns component T of entity (if exists)

    def remove_component<T>() -> None:
    # Removes component T from entity (if exists)

    def has_component<T>() -> Bool:
    # Checks if entity has component T
```

## 2. Node
```Python 
class Node:
    #Class members:
    self.parent   # parent node
    self.children # children nodes
    self.entity   # pointer to entity node is connected with

    def create() -> std::shared_ptr<Node>:
    # Creates node object

    def create(entity: std::shared_ptr<Entity>) -> std::shared_ptr<Node>:
    # Creates node object and attaches provided entity

    def attach_entity(entity: std::shared_ptr<Entity>) -> None:
    # Attaches provided entity to node

    def set_parent(parent: std::shared_ptr<SceneNode>) -> None:
    # Sets parent of node

    def add_child(child: std::shared_ptr<SceneNode>) -> None:
    # Adds child to node 

    def act<Args...>(
        f: Action<void(SceneNode*, Args...)>,
        args...) -> None:
    # Executes provided action 'f' on node, and propagates 
    # it to node's children
```

### Action
Action is a function applicable on Node. it can be predefined or constructed directly in 'act' method. Action can be 
constructed as a functor (lambda):
```cpp 
Action<void(Node*)> foo = [](Node* node) {...}
```
or as a regular function:
```cpp 
void foo(Node* node) {...}
```
It doesn't really matter because 'act' accepts both conventions.

Action can also have an ActionType which is either RELATIVE or ABSOLUTE. The first one suggests that Action is dependant
from parent and second one that it isn't, however user has complete freedom in creating Actions, and it is up to him to
decide what each Action will be responsible for. Action.hh contains static TransformAction class with some base actions for TransformComponent which can be used as a 
pattern. 

## 3. Scene
```Python 
class Scene:
    # Class members:
    self.registry       # entt registry
    self.root_node      # root of the scene graph
    self.current_camera # pointer to camera which is currently in use
    self.cameras        # all cameras created for scene

    def create() -> std::shared_ptr<Scene>:
    # Creates scene object

    def create_entity(name: string()) -> std::shared_ptr<Entity>:
    # Registeres and creates entity

    def destroy_entity(entity: Entity&) -> None:
    # Destroys entity

    def add_camera(camera: std::shared_ptr<Camera>) -> None:
    # Adds camera to scene

    def get_camera(index: uint32_t) -> std::shared_ptr<Camera>:
    # Returns camera at provided index (if it exists)

    def get_root() -> Node&:
    # Returns root of the scene graph

    def view<Args...>() -> entt::basic_view<Args...>:
    # Returns entt view with provided components

    def set_current_camera(camera: Camera*) -> None:
    # Sets current camera

    def get_current_camera() -> Camera*:
    # Returns current camera
```

# USAGE
```Python 
def main() -> None:
    # Create scene
    scene = Scene::create()
    
    # Add and adjust camera
    scene->add_camera(std::make_shared<Camera>())
    camera = scene->get_camera(0)
    camera->set_position(glm::vec3{ 0.f, -1.f, -5.f })
    camera->look_at(glm::vec3{ 0.f, 0.f, 0.f })
    camera->set_move_speed(3.f)
    camera->set_sensitivity(4.f)

    # Set current camera
    scene->set_current_camera(camera.get())

    # Create some entities
    cubes: list[std::shared_ptr<Entity>]
    for i in range(0, 3):
        cubes[i] = scene->create_entity()
        cubes[i]->add_component<TransformComponent>()
    
    # Create nodes
    nodes: list[std::shared_prt<Node>]
    for i in range(0, 3):
        nodes[i] = Node::create()
        nodes[i]->attach_entity(cubes[i])
    
    scene->get_root().add_child(nodes[0])
    TransformAction::set_translation(nodes[0].get(),
                                     glm::vec3{ 0.f, 0.f, 0.f }, 
                                     RELATIVE)
    for i in range(1, 3):
        nodes[i-1]->add_child(nodes[i])
        nodes[i]->set_parent(nodes[i-1])
        TransformAction::set_translation(nodes[i].get(),
                                         glm::vec3{ 1.f, 0.f, 0.f },
                                         RELATIVE)
        
    # Update scene
    nodes[0]->act(
        [dt](Node* node)
        {
            auto& transform = node->get_entity()->get_component<TransformComponent>()
            transform.reset();

            TransformAction::update_rotation(node,
                                             -dt / 2,
                                             glm::vec3{ 0.f, 1.f, 0.f },
                                             ABSOLUTE)
            TransformAction::translate(node, ABSOLUTE)
            TransformAction::scale(node, ABSOLUTE)
        })

    # Alternatively
    nodes[0]->act(TransformAction::reset)
    nodes[0]->act(TransformAction::translate, ABSOLUTE)
    nodes[0]->act(TransformAction::update_rotation,
                 dt / 2,
                 glm::vec3{ 0.f, 1.f, 0.f }, 
                 ABSOLUTE)
    nodes[0]->act(TransformAction::scale, ABSOLUTE)
```