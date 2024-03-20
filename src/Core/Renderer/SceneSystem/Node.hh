#ifndef SCENE_SCENE_NODE_HH
#define SCENE_SCENE_NODE_HH

#include "Action.hh"
#include "Components/Components.hh"

namespace esp
{
  class Scene;
  class Entity;

  class Node : public std::enable_shared_from_this<Node>
  {
   private:
    std::shared_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children;
    std::shared_ptr<Entity> m_entity;

   public:
    /// @brief Creates an empty Node.
    /// @return Shared pointer to empty Node.
    static std::shared_ptr<Node> create();
    /// @brief Creates new Node with an Entity.
    /// @param entity Entity to add to a Node.
    /// @return Shared pointer to created Node.
    static std::shared_ptr<Node> create(std::shared_ptr<Entity> entity);

   private:
    static std::shared_ptr<Node> create_root(Scene* scene);

   public:
    PREVENT_COPY(Node)

    ~Node() = default;

    /// @brief Attaches Entity to a Node.
    /// @param entity Entity to add to a Node.
    void attach_entity(std::shared_ptr<Entity> entity);

    /// @brief Adds child to Node in scene greph.
    /// @param child Child Node to be added to current Node.
    void add_child(std::shared_ptr<Node> node);

    void remove_child(Node* node);

    /// @brief Returns pointer to parent Node.
    /// @return Pointer to parent Node.
    Node* get_parent();
    /// @brief Returns pointer to child.
    /// @param index Child's index inside children vector
    /// @return Pointer to child.
    Node* get_child(uint32_t index);
    /// @brief Returns pointer to Entity.
    /// @return Pointer to Entity.
    Entity* get_entity();

    /// @brief Runs Action with arguments for current Node and their children.
    /// @tparam ...Args Types of Action arguments.
    /// @param f Action to be run.
    /// @param ...args Action arguments.
    template<typename... Args> void act(action::Action<void(Node*, Args...)> f, Args&&... args)
    {
      f(this, std::forward<Args>(args)...);
      for (auto& node : m_children)
      {
        node->act(f, std::forward<Args>(args)...);
      }
    }

    // ---------- Transform component functions ----------
    /// @brief Translates entity by given argument
    /// @param vec Vector to translate by
    void translate(glm::vec3 vec);
    /// @brief Rotates entity by given arguments
    /// @param angle Angle to rotate by in radians
    /// @param axis Axis to rotate around
    void rotate(float angle, glm::vec3 axis);

    // ---------------------------------------- MG1 -------------------------------------------------------
    void rotate_x(float angle);
    void rotate_y(float angle);
    void rotate_z(float angle);
    // ----------------------------------------------------------------------------------------------------

    /// @brief Scales entity by given argument
    /// @param val Value to scale by
    void scale(float val);

    /// @brief Sets entity's translation to given argument
    /// @param vec Translation vector
    void set_translation(glm::vec3 vec);
    /// @brief Sets entity's rotation to given arguments
    /// @param angle Rotation angle in radians
    /// @param axis Rotation axis
    void set_rotation(float angle, glm::vec3 axis);
    /// @brief Sets entity's scale to given argument
    /// @param val Scale value
    void set_scale(float val);

    /// @brief Returns entity's model matrix
    /// @param type Tells whether matrix is relative to parent
    /// or world (relative to world by default)
    glm::mat4 get_model_mat(action::ActionType type = action::ESP_ABSOLUTE);
    /// @brief Returns entity's translation
    /// @param type Tells whether translation is relative to parent
    /// or world (relative to world by default)
    /// @return Entity's translation
    glm::vec3 get_translation(action::ActionType type = action::ESP_ABSOLUTE);
    /// @brief Returns entity's rotation
    /// @param type Tells whether rotation is relative to parent
    /// or world (relative to world by default)
    /// @return Entity's rotation
    glm::quat get_rotation(action::ActionType type = action::ESP_ABSOLUTE);
    /// @brief Returns entity's scale
    /// @param type Tells whether scale is relative to parent
    /// or world (ESP_RELATIVE to world by default)
    /// @return Entity's scale
    float get_scale(action::ActionType type = action::ESP_ABSOLUTE);

    /// @brief Returns entity's TransformComponent
    /// @return TransformComponent's reference
    TransformComponent& get_transform();
    // --------------------------------------------------

   private:
    Node() : m_entity{ nullptr } {}
    Node(std::shared_ptr<Entity> entity) : m_entity{ std::move(entity) } {}

    friend class Scene;
  };
} // namespace esp

#endif // SCENE_SCENE_NODE_HH
