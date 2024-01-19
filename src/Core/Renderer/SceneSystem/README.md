# SCENE SYSTEM

Scene system's architecture is build upon 2 paradigms - entity component 
system ([entt](https://github.com/skypjack/entt) library) and scene graph. Scene consists of 3 main classes. Namely 
1. Entity - responsible for managing object's components
2. Node - scene graph's Node, manages Entity taking into account relations with other Nodes. It can posess multiple 
children and a single parent (just like in a regular tree).
3. Scene - registers each Entity that is on scene. Every Entity has 2 components (Tag and Transform) after creation.
Scene contains 
   - Entt registry for registering entities.  
   - Root Node which is the root of scene's graph. 
   - Vector of Cameras which user can create and add. Also, there is a static pointer on Camera that is currently in use.
     
Scene class provides method for rendering itself on the screen. In order for Node to be rendered through this method it
must meet 2 conditions. It has to have connection (path) to root Node, plus it has to have ModelComponent registered with 
valid model

User can interact with Entity's components directly through it's class, or by applying an [Action](#action) on Node. 
This way changes can be propagated down the graph applying the same Action on Node's children. As mentioned, every 
object (Entity) can have it's own components, and there are some base ones predefined
(i.e. TransformComponent) but more can be created by the user if needed. Another approach for utilizing components is to
inherit from Node and create a new class with extension methods for desired components. For convenience, Node has
predefined methods for TransformComponent's usage.

## 1. Entity
```Python 
class Entity:
    #Class members:
    self.handle # handle to entt entity
    self.scene  # pointer to scene on which entity is registered

    def add_component<T, Args...>(args...) -> T&:
    # Constructs and adds component T to entity using provided args
    
    def get_component<T>() -> T&:
    # Returns component T of entity (if exists)

    def try_get_component<T>() -> T*:
    # Returns pointer to component T of entity if it exists.
    # Else returns null.

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

    def add_child(node: std::shared_ptr<SceneNode>) -> None:
    # Adds child to node and sets node as child's parent
    
    def get_parent() -> Node*:
    # Returns parent of node
    
    def get_child(index: uint32_t) -> Node*:
    # Returns child of node at given index (if exists)
    
    def get_entity() -> Entity*:
    # Returns entity attached to node

    def act<Args...>(
        f: Action<void(SceneNode*, Args...)>,
        args...) -> None:
    # Executes provided action 'f' on node, and propagates 
    # it to node's children

    # ---------- Transform component functions ----------
    def translate(vec: glm::vec3) -> None:
    # Translates node's by given vector
    def rotate(angle: float, axis: glm::vec3) -> None:
    # Rotates node's by given angle and axis
    def scale(val: float) -> None:
    # Scales node's by given value

    def set_translation(vec: glm::vec3) -> None:
    # Sets node's translation to given vector
    def set_rotation(angle: float, axis: glm::vec3) -> None:
    # Sets node's rotastion to given angle and axis
    def set_scale(val: float) -> None:
    # Sets node's scale to given value

    def get_model_mat(type: ActionType::ESP_ABSOLUTE) -> glm::mat4:
    # Gets node's model matrix. If type is 
    # - ESP_ABSOLUTE then matrix is relative to world
    # - ESP_RELATIVE then matrix is relative to parent
    def get_translation(type: ActionType::ESP_ABSOLUTE) -> glm::vec3:
    # Gets node's translation. If type is 
    # - ESP_ABSOLUTE then translation is relative to world
    # - ESP_RELATIVE then translation is relative to parent
    def get_rotation(type: ActionType::ESP_ABSOLUTE) -> glm::quat:
    # Gets node's rotation. If type is 
    # - ESP_ABSOLUTE then rotation is relative to world
    # - ESP_RELATIVE then rotation is relative to parent
    def get_scale(type: ActionType::ESP_ABSOLUTE) -> float:
    # Gets node's scale. If type is 
    # - ESP_ABSOLUTE then scale is relative to world
    # - ESP_RELATIVE then scale is relative to parent
    
    def get_transform() -> TransformComponent&:
    # Gets node's TransformComponent
    # ---------------------------------------------------

```

### Action
Action is a function applicable on Node. it can be predefined or constructed directly in 'act' method. Action can be 
constructed as a functor (lambda):
```cpp 
Action<void(Node*)> foo = [](Node* node) {...}
```
or as a static function:
```cpp 
static void foo(Node* node) {...}
```
It doesn't really matter because 'act' accepts both conventions.

Action can also have an ActionType which is either ESP_RELATIVE or ESP_ABSOLUTE. The first one suggests that Action is dependant
from parent and second one that it isn't, however user has complete freedom in creating Actions, and it is up to him to
decide what each Action will be responsible for.

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

    def draw() -> None:
    # Draws each Node on scene graph that has ModelComponent
```

# USAGE
```Python 
def main() -> None:
    # Create scene
    scene = Scene::create()
    
    # Add and adjust camera
    scene->add_camera(std::make_shared<Camera>())
    camera = scene->get_camera(0)
    camera->set_position(glm::vec3{ 0.f, 1.f, 5.f })
    camera->look_at(glm::vec3{ 0.f, 0.f, 0.f })
    camera->set_move_speed(3.f)
    camera->set_sensitivity(4.f)

    # Set current camera
    scene->set_current_camera(camera.get())

    # Create some entities
    cubes: list[std::shared_ptr<Entity>]
    for i in range(0, 3):
        cubes[i] = scene->create_entity()
    
    # Create nodes
    nodes: list[std::shared_prt<Node>]
    for i in range(0, 3):
        nodes[i] = Node::create()
        nodes[i]->attach_entity(cubes[i])
    
    scene->get_root().add_child(nodes[0])
    nodes[0]->translate(glm::vec3(0.f, 0.f, -1.f))
    
    for i in range(1, 3):
        nodes[i-1]->add_child(nodes[i])
        nodes[i]->translate(glm::vec3(1.f, 0.f, 0.f))
        nodes[i]->scale(.5f)


    # Update scene
    nodes[0]->rotate(get_dt(), glm::vec3(0,f 1.f, 0.f))

    # Draw scene 
    scene->draw()
    # Alternatively (simplified)
     nodes[0]->act(
        [](Node* node)
        {
            model = node->get_entity()->get_component<ModelComponent>()
            model.m_model->draw()
        })
```