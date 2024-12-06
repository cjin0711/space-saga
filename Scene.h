// Scene.h
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    Map* map;
    Entity* player;
    Entity* enemies;
	Entity* player_beams;
    Entity* enemy_one_beams;
    Entity* enemy_two_beams;
    Entity* enemy_missile;


    Mix_Music* music;
    Mix_Chunk* jump_sfx;
    Mix_Chunk* lose_life_sfx;
    Mix_Chunk* enemy_death_sfx;
	Mix_Chunk* missile_launch_sfx;

    int next_scene_id;
};

extern int num_lives;
//extern float enemy_one_health;
//extern float enemy_two_health;
extern float boss_health;
extern int kill_count;
extern bool lost_game;
extern bool passed_levelA;
extern bool passed_levelB;
extern bool passed_levelC;


class Scene {
public:
    int m_number_of_enemies = 1;

    GameState g_state;

    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;

    GameState const get_state()             const { return g_state; }
    int       const get_number_of_enemies() const { return m_number_of_enemies; }

}; 
