#include "LevelC.h"
#include "Utility.h"
#include <windows.h>

#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 7
#define ENEMY_COUNT 1

const char SPRITESHEET_FILEPATH[] = "assets/playerCraft.png";
constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

GLuint g_font_texture_id_C;

// DATA FOR MAP
unsigned int LevelC_DATA[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

LevelC::~LevelC()
{
    delete[] g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
    Mix_FreeChunk(g_state.jump_sfx);
    Mix_FreeMusic(g_state.music);
}

void LevelC::initialise()
{

    // ----- MAP ----- //
    GLuint map_texture_id = Utility::load_texture("assets/platform.png");

    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LevelC_DATA, map_texture_id, 1.0f, 1, 1);

    // ----- FONT ----- //
    g_font_texture_id_C = Utility::load_texture(FONTSHEET_FILEPATH);

    // ----- WAVE ----- //
    m_spaceship_one_beam_timer = 0.0f;
    m_spaceship_two_beam_timer = 0.0f;
    m_missile_timer = 0.0f;

    // ----- PLAYER ----- //
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, -18.0f, 0.0f);

    g_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        1.0f,                      // width
        1.0f,                       // height
        PLAYER                     // entity type
    );

    // Initial Position
    g_state.player->set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    g_state.player->set_start_position(glm::vec3(1.0f, -3.0f, 0.0f));


    // ----- PLAYER BEAMS ----- //
    GLuint player_beams_texture_id = Utility::load_texture("assets/playerBeam.png");

    g_state.player_beams = new Entity(
        player_beams_texture_id,        // texture id
        5.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        PLAYERBEAM                   // entity type
    );

    g_state.player_beams->set_position(glm::vec3(-10.0f, 10.0f, 0.0f));

    // ----- ENEMY BEAMS ----- //
    GLuint enemy_beams_texture_id = Utility::load_texture("assets/enemyBeam.png");

    g_state.enemy_one_beams = new Entity(
        enemy_beams_texture_id,        // texture id
        6.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        ENEMYBEAM                    // entity type
    );

    g_state.enemy_two_beams = new Entity(
        enemy_beams_texture_id,        // texture id
        6.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        ENEMYBEAM                    // entity type
    );

    // ----- ENEMY MISSILE ----- //
    GLuint enemy_missile_texture_id = Utility::load_texture("assets/missile.png");

    g_state.enemy_missile = new Entity(
        enemy_missile_texture_id,        // texture id
        3.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        HOMING,                       // entity type
        MISSILE,                    // AI type
        FLYING					   // initial AI state
    );

    // ----- ENEMY: MOTHERSHIP (Count: 1) ----- //
    GLuint boss_texture_id = Utility::load_texture("assets/bossCraft.png");

    g_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        g_state.enemies[i] = Entity(
            boss_texture_id,    // texture id
            1.0f,               // speed
            2.75f,              // width
            2.75f,               // height
            ENEMY,              // entity type
            MOTHERSHIP,           // AI type
            BOSSIDLE               // initial AI state
        );
    }

    g_state.enemies[0].set_position(glm::vec3(8.1f, -3.0f, 0.0f));

    // ----- BACKGROUND MUSIC ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    g_state.music = Mix_LoadMUS("assets/wave_three_music.mp3");
    Mix_PlayMusic(g_state.music, -1);

    // Volume
    Mix_VolumeMusic(64.0f);

    // ----- JUMP SOUND EFFECT ----- //
    g_state.jump_sfx = Mix_LoadWAV("assets/playerBeamShoot.wav");
    g_state.lose_life_sfx = Mix_LoadWAV("assets/lose-life.wav");
    g_state.enemy_death_sfx = Mix_LoadWAV("assets/enemy_explosion.wav");
}

void LevelC::shoot_player_beams() {
    GLuint player_beams_texture_id = Utility::load_texture("assets/playerBeam.png");

    g_state.player_beams = new Entity(
        player_beams_texture_id,        // texture id
        9.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        PLAYERBEAM                   // entity type
    );
    g_state.player_beams->set_position(glm::vec3(g_state.player->get_position().x + 1.0f, g_state.player->get_position().y, 0.0f));
    g_state.player_beams->set_movement(glm::vec3(1.5f, 0.0f, 0.0f));
}

void LevelC::shoot_enemy_one_beams() {
    GLuint enemy_beams_texture_id = Utility::load_texture("assets/enemyBeam.png");
    g_state.enemy_one_beams = new Entity(
        enemy_beams_texture_id,        // texture id
        6.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        ENEMYBEAM                       // entity type
    );
    g_state.enemy_one_beams->set_position(glm::vec3(g_state.enemies[0].get_position().x -2.0f, g_state.enemies[0].get_position().y + 0.75f, 0.0f));
    g_state.enemy_one_beams->set_movement(glm::vec3(-1.5f, 0.0f, 0.0f));
}

void LevelC::shoot_enemy_two_beams() {
    GLuint enemy_beams_texture_id = Utility::load_texture("assets/enemyBeam.png");
    g_state.enemy_two_beams = new Entity(
        enemy_beams_texture_id,        // texture id
        6.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        ENEMYBEAM                    // entity type
    );
    g_state.enemy_two_beams->set_position(glm::vec3(g_state.enemies[0].get_position().x - 2.0f, g_state.enemies[0].get_position().y - 0.75f, 0.0f));
    g_state.enemy_two_beams->set_movement(glm::vec3(-1.5f, 0.0f, 0.0f));
}

void LevelC::shoot_enemy_missile() {
    GLuint enemy_missile_texture_id = Utility::load_texture("assets/missile.png");

    g_state.enemy_missile = new Entity(
        enemy_missile_texture_id,        // texture id
        3.0f,                       // speed
        0.75f,                      // width
        0.25f,                      // height
        HOMING,                       // entity type
		MISSILE,                    // AI type
		FLYING					   // initial AI state
    );

    g_state.enemy_missile->set_position(glm::vec3(g_state.enemies[0].get_position().x - 2.0f, g_state.enemies[0].get_position().y, 0.0f));
    g_state.enemy_missile->set_movement(glm::vec3(-0.25f, 0.0f, 0.0f));

}

void LevelC::update(float delta_time)
{

    m_spaceship_one_beam_timer += delta_time;
    m_spaceship_two_beam_timer += delta_time;

    // ENEMY SHOOTING LOGIC
    if (m_spaceship_one_beam_timer >= 1.5f && g_state.enemies[0].get_is_active()) {
        Mix_PlayChannel(-1, g_state.jump_sfx, 0);
        shoot_enemy_one_beams();
        m_spaceship_one_beam_timer = 0.0f;
    }
    if (m_spaceship_two_beam_timer >= 1.65f && g_state.enemies[0].get_is_active()) {
        Mix_PlayChannel(-1, g_state.jump_sfx, 0);
        shoot_enemy_two_beams();
        m_spaceship_two_beam_timer = 0.0f;
    }

    if (g_state.enemies[0].get_missile_launch()) {
		m_missile_timer += delta_time;
        if (m_missile_timer >= 4.0f && g_state.enemies[0].get_is_active()) {
            shoot_enemy_missile();
			m_missile_timer = 0.0f;
        }
    }


    // COLLISION LOGIC
    if (g_state.player->get_lose_life()) {
        Mix_PlayChannel(-1, g_state.lose_life_sfx, 0);
        num_lives--;
    }
    if (num_lives == 0) {
        g_state.player->set_touched();
    }

    // KILL LOGIC
    if (g_state.player->get_killed_enemy()) {
        kill_count++;
    }

    g_state.player->update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);

    // SHOOTING LOGIC
    if (g_state.player->get_shooting()) {
        shoot_player_beams();
    }

    if (g_state.player_beams->get_is_active()) {
        g_state.player_beams->update(delta_time, g_state.player, g_state.enemies, ENEMY_COUNT, g_state.map);
    }
    if (g_state.enemy_one_beams->get_is_active()) {
        g_state.enemy_one_beams->update(delta_time, g_state.player, g_state.player, 1, g_state.map);
    }
    if (g_state.enemy_two_beams->get_is_active()) {
        g_state.enemy_two_beams->update(delta_time, g_state.player, g_state.player, 1, g_state.map);
    }
    if (g_state.enemy_missile->get_is_active()) {
        // to check with collisions with player or player beam
        g_state.enemy_missile->update(delta_time, g_state.player, g_state.player, 1, g_state.map);
		g_state.enemy_missile->update(delta_time, g_state.player, g_state.player_beams, 1, g_state.map);
    }

    for (int i = 0; i < ENEMY_COUNT; i++)
    {

        if (g_state.enemies[0].get_enemy_health() <= 0.0f && g_state.enemies[0].get_enemy_health() >= -18.0f) {
            g_state.enemies[0].set_enemy_health(-20.0f);
            g_state.player->set_killed_enemy(true);
            g_state.enemies[0].deactivate();
        }


        if (g_state.enemies[i].get_hit_enemy()) {
            g_state.enemies[0].set_enemy_health(g_state.enemies[0].get_enemy_health() - 4);
        }

        g_state.enemies[i].update(delta_time, g_state.player, g_state.player, 1, g_state.map);
    }

    if (g_state.player->get_killed_enemy()) {
        Mix_PlayChannel(-1, g_state.enemy_death_sfx, 0);
        g_state.player->set_killed_enemy(false);
    }

    // Check for Level C Pass
    if (!g_state.enemies[0].get_is_active()) {
        passed_levelC = true;
    }
}


void LevelC::render(ShaderProgram* g_shader_program)
{
    g_state.map->render(g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++)
        g_state.enemies[i].render(g_shader_program);

    if (g_state.player_beams->get_is_active()) {
        g_state.player_beams->render(g_shader_program);
    }
    if (g_state.enemy_one_beams->get_is_active()) {
        g_state.enemy_one_beams->render(g_shader_program);
    }
    if (g_state.enemy_two_beams->get_is_active()) {
        g_state.enemy_two_beams->render(g_shader_program);
    }
    if (g_state.enemy_missile->get_is_active()) {
        g_state.enemy_missile->render(g_shader_program);
    }

    if (g_state.player) {
        g_state.player->render(g_shader_program);

        if (num_lives >= 1) {
            Utility::draw_text(g_shader_program, g_font_texture_id_C, "Lives:" + std::to_string(num_lives), 0.2f, 0.0001f,
                glm::vec3(g_state.player->get_position().x - 0.65f, g_state.player->get_position().y + 1.0f, 0.0f));
        }

        if (g_state.enemies[0].get_is_active()) {
            Utility::draw_text(g_shader_program, g_font_texture_id_C, std::to_string(g_state.enemies[0].get_enemy_health()) + "%", 0.2f, 0.0001f,
                glm::vec3(g_state.enemies[0].get_position().x, g_state.enemies[0].get_position().y + 1.0f, 0.0f));
        }
        
        // Conditional Win / Lose Text
        if (passed_levelC) {
            Utility::draw_text(g_shader_program, g_font_texture_id_C, "YOU WIN!", 0.5f, 0.05f,
                glm::vec3(2.5f, -3.0f, 0.0f));
        }

        if (kill_count < 3 && g_state.player->get_touched()) {
            Utility::draw_text(g_shader_program, g_font_texture_id_C, "YOU LOSE!", 0.5f, 0.05f,
                glm::vec3(g_state.player->get_position().x + 1.5f, g_state.player->get_position().y, 0.0f));
        }
    }
}

