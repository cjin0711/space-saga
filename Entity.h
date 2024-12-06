#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Map.h"
#include <windows.h>

enum EntityType { PLATFORM, PLAYER, ENEMY, PLAYERBEAM, ENEMYBEAM, MISSILE };
enum AIType     { ASTEROID, SPACECRAFT, MOTHERSHIP  };
enum AIState    { FLYING, STATIONARY, DODGING, BOSS };


enum AnimationDirection { LEFT, RIGHT, UP, DOWN };

class Entity
{
private:
    bool m_is_active = true;

    // NEWLY ADDED
    bool m_lose_life = false;
    bool m_killed_enemy = false;
    bool m_hit_enemy = false;
    bool m_enemy_shooting = false;
    int m_enemy_health = 100;

    bool m_touched_enemy = false;
    int kill_count = 0;
    int num_lives = 3;
  
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    // ����� TRANSFORMATIONS ����� //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed,
              m_jumping_power;
    
    bool m_is_jumping;

	glm::vec3 m_start_position;

    // ����� TEXTURES ����� //
    GLuint    m_texture_id;

    // ����� ANIMATION ����� //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;

    int m_walking[2][3]; // 4x4 array for walking animations
    int m_idle[2]; // array for idle left and right animations
    int m_jump[2]; // array for jump left and right animations
    int m_game_result[2]; // array for win / loss animations

    // ����� COLLISIONS ����� //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;

    // for fireball
    bool m_shooting;

public:
    // ����� STATIC VARIABLES ����� //
    static constexpr int SECONDS_PER_FRAME = 12;

    // ����� METHODS ����� //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[2][4], float animation_time,
        int animation_frames, int animation_index, int animation_cols,
           int animation_rows, float width, float height, EntityType EntityType);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count, Entity* player);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count, Entity* player);

    //OVERLOADED
    void const check_collision_y(Map* map);
    void const check_collision_x(Map* map);


    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_fly();
    void ai_guard(Entity *player);
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }


    void fireball() { 
        OutputDebugString(L"FIREBALL FUNCTION CALLEDDDDDDDDDD!\n");
        set_shooting(true);
    }

    void move_left() { m_movement.x = -1.0f; }
    void move_right() { m_movement.x = 1.0f; }
    void move_up() { m_movement.y = 1.0f; }
    void move_down() { m_movement.y = -1.0f; }

    void idle_left() { m_animation_indices = m_idle; set_animation_index(0); }
    void idle_right() { m_animation_indices = m_idle; set_animation_index(1); }
  
    void const jump_left() { /*m_is_jumping = true;*/ m_animation_indices = m_jump; set_animation_index(0);  }
    void const jump_right() { /*m_is_jumping = true;*/ m_animation_indices = m_jump; set_animation_index(1); }
    void const jump() { m_is_jumping = true; }

    // ����� GETTERS ����� //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    bool      const get_is_active() const { return m_is_active; }

    float get_width() const { return m_width; }
    float get_height() const { return m_height; }

    int       const get_kill_count() const { return kill_count; }
	int       const get_num_lives() const { return num_lives; }

    bool      const get_touched() const { return m_touched_enemy; }
    bool      const get_shooting() const { return m_shooting; }
    bool      const get_lose_life() const { return m_lose_life; }
    bool      const get_killed_enemy() const { return m_killed_enemy; }

    // newly added
    bool      const get_hit_enemy() const { return m_hit_enemy; }
    int       const get_enemy_health() const { return m_enemy_health; }
    glm::vec3 const get_start_position()     const { return m_start_position; }
	bool 	  const get_enemy_shooting() const { return m_enemy_shooting; }


    
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };
    // ����� SETTERS ����� //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }

    void const set_kill_count(int new_kill_count) { kill_count = new_kill_count; }
	void const set_num_lives(int new_num_lives) { num_lives = new_num_lives; }
    void const set_touched() { m_touched_enemy = true; }
    void const set_shooting(bool new_val) { m_shooting = new_val;  }
    void const set_lose_life() { m_lose_life = true; }
    void const set_killed_enemy(bool val) { m_killed_enemy = val; }

    // newly added
    void const set_hit_enemy() { m_hit_enemy = true; }
    void const set_enemy_health(int num) { m_enemy_health = num; }
	void const set_start_position(glm::vec3 new_start_position) { m_start_position = new_start_position; }
	void const set_enemy_shooting() { m_enemy_shooting = true; }

    // Setter for m_walking
    void set_walking(int walking[3][4])
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 1; j < 4; ++j)
            {
                m_walking[i][j - 1] = walking[i][j];
            }
        }
    }
    void set_idle(int walking[3][4]) {
        m_idle[0] = walking[0][0];
        m_idle[1] = walking[1][0];
    }

    void set_jumping(int walking[3][4]) {
        m_jump[0] = walking[2][1];
        m_jump[1] = walking[2][0];
    }

    void set_game_result(int walking[3][4]) {
        m_game_result[0] = walking[2][2];
        m_game_result[1] = walking[2][3];
    }
};

#endif // ENTITY_H