#ifndef SCENE_ANIMATION_ANIMATORi_HH
#define SCENE_ANIMATION_ANIMATORi_HH

#include "esppch.hh"

#include "Animation.hh"

#include "Core/Renderer/Model/Model.hh"

namespace esp
{
  enum class AnimationPeriodType
  {
    ONLY_ONCE,
    MANY_TIMES,
    NON_STOP
  };

  class Animator
  {
   public:
    static const int32_t MAX_BONES = 100;

   private:
    Animation* m_current_animation;

    glm::mat4 m_final_bone_matrices[Animator::MAX_BONES];
    glm::mat4* m_current_bone_matrices;

    float m_current_time;

    struct
    {
      AnimationPeriodType m_type;
      int32_t m_cycle_to_do;
    } m_period;

   private:
    void calculate_bone_transform(Model* model, ModelNode* node, glm::mat4 parent_transform);
    void caclucate_bote_transform_moment_zero(Model* model, ModelNode* node, glm::mat4 parent_transform);
    bool check_animation_period();

   public:
    PREVENT_COPY(Animator)

    Animator();
    ~Animator();

    void update_animation(float dt, Model* model);

    void play_animation(Model* model, uint32_t animation_index, AnimationPeriodType type, uint32_t cycles_count = 1);

    inline glm::mat4* get_final_bone_matrices() { return m_current_bone_matrices; }

    inline bool is_animating() const { return m_current_animation != nullptr; }
  };
} // namespace esp

#endif // SCENE_ANIMATION_ANIMATORi_HH
