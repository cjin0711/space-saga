#include "LevelA.h"
#include "Utility.h"
#include <windows.h>
#include <ctime>
#include <cstdlib>

#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 7

int WAVE_COUNT = 1;

const char SPRITESHEET_FILEPATH[] = "assets/playerCraft.png";
constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

GLuint g_font_texture_id_A;

// DATA FOR MAP
unsigned int LevelA_DATA[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

LevelA::~LevelA()
{
    delete[] g_state.enemies;
    delete    g_state.player;
    delete    g_state.map;
	delete    g_state.player_beams;
    Mix_FreeChunk(g_state.jump_sfx);
    Mix_FreeMusic(g_state.music);
}

void LevelA::initialise()
{
    OutputDebugString(L"LEVEL A IS INITIALISINGGGGGGGGGGGGGGGGGGGGGGGGGGG");
	// ----- MAP ----- //
    GLuint map_texture_id = Utility::load_texture("assets/platform.png");

    g_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LevelA_DATA, map_texture_id, 1.0f, 1, 1);

	// ----- FONT ----- //
    g_font_texture_id_A = Utility::load_texture(FONTSHEET_FILEPATH);

    // ----- WAVE ----- //
    m_wave_timer = 0.0f;
	m_asteroid_timer = 0.0f;

    m_start_delay = 3.0f; // Delay duration in seconds
    m_delay_complete = false; // Flag to indicate if the delay is over

	// ----- PLAYER ----- //
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    g_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );

    // Initial Position
    g_state.player->set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    g_state.player->set_start_position(glm::vec3(1.0f, -3.0f, 0.0f));



    // ----- ENEMY: ASTEROID ----- //
    GLuint asteroid_texture_id = Utility::load_texture("assets/asteroid.png");

    g_state.enemies = new Entity[WAVE_COUNT];

    for (int i = 0; i < WAVE_COUNT; i++)
    {
        g_state.enemies[i] = Entity(
            asteroid_texture_id,    // texture id
            0.0f,               // speed
            1.0f,              // width
            1.0f,               // height
            ENEMY,              // entity type
            ASTEROID,              // AI type
            FLYING                // initial AI state
        );
    }

    g_state.enemies[0].set_position(glm::vec3(11.1f, -4.0, 0.0f));
    //g_state.enemies[0].set_acceleration(glm::vec3(0.0f, -5.0f, 0.0f));

	// ----- BACKGROUND MUSIC ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    g_state.music = Mix_LoadMUS("assets/wave_one_music.mp3");
    Mix_PlayMusic(g_state.music, -1);
    // Volume
    Mix_VolumeMusic(64.0f);

	// ----- JUMP SOUND EFFECT ----- //
    g_state.jump_sfx = Mix_LoadWAV("assets/wave_one_music.wav");
	g_state.lose_life_sfx = Mix_LoadWAV("assets/lose-life.wav");
}

void LevelA::wave_one() {
    GLuint asteroid_texture_id = Utility::load_texture("assets/asteroid.png");

    g_state.enemies = new Entity[WAVE_COUNT];

    for (int i = 0; i < WAVE_COUNT; i++)
    {
        g_state.enemies[i] = Entity(
            asteroid_texture_id,    // texture id
            7.0f,               // speed
            1.0f,              // width
            1.0f,               // height
            ENEMY,              // entity type
            ASTEROID,              // AI type
            FLYING               // initial AI state
        );
    }

    float random_height = rand() % 4 + 1.5f;

    g_state.enemies[0].set_position(glm::vec3(10.1f, -random_height, 0.0f));
}

void LevelA::wave_two() {
    GLuint asteroid_texture_id = Utility::load_texture("assets/asteroid.png");

    g_state.enemies = new Entity[WAVE_COUNT];

    for (int i = 0; i < WAVE_COUNT; i++)
    {
        g_state.enemies[i] = Entity(
            asteroid_texture_id,    // texture id
            7.0f,               // speed
            1.0f,              // width
            1.0f,               // height
            ENEMY,              // entity type
            ASTEROID,              // AI type
            FLYING                // initial AI state
        );
    }

    float random_height_one = rand() % 5 + 1.0f;

    g_state.enemies[0].set_position(glm::vec3(10.1f, -random_height_one, 0.0f));

    float random_height_two = rand() % 5 + 1.0f;
    while (random_height_one == random_height_two) {
        random_height_two = rand() % 5 + 1.0f;
    }
    
    g_state.enemies[1].set_position(glm::vec3(10.1f, -random_height_two, 0.0f));
    g_state.enemies[1].set_speed(8.5f);
}

void LevelA::update(float delta_time)
{

    g_state.player->update(delta_time, g_state.player, g_state.enemies, WAVE_COUNT, g_state.map);

    for (int i = 0; i < WAVE_COUNT; i++) {
        g_state.enemies[i].update(delta_time, g_state.player, g_state.player, 1, g_state.map);
    }

    // 3 Second Initial Delay
    if (!m_delay_complete) {
        m_start_delay -= delta_time; // Reduce the delay timer


        if (m_start_delay <= 0.0f) {
            m_delay_complete = true; // Mark the delay as complete
        }
        return; // Skip further updates until delay is over
    }

    // Wave time counter & time between each asteroid
	m_wave_timer += delta_time;
    m_asteroid_timer += delta_time;

    // Check for Level A Pass
	if (m_wave_timer >= WAVE_DURATION) {
		passed_levelA = true;
	}
    // Wave One
	if (m_asteroid_timer >= 1.75f && m_wave_timer < WAVE_DURATION / 2) {
		wave_one();
        m_asteroid_timer = 0.0f;
	}
    // Wave Two
    else if (m_asteroid_timer >= 1.75f && m_wave_timer > WAVE_DURATION / 2 && m_wave_timer < WAVE_DURATION) {
        WAVE_COUNT = 2;
        wave_two();
        m_asteroid_timer = 0.0f;
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

}


void LevelA::render(ShaderProgram* g_shader_program)
{
    g_state.map->render(g_shader_program);
    for (int i = 0; i < WAVE_COUNT; i++)
        g_state.enemies[i].render(g_shader_program);

    if (g_state.player) {
        g_state.player->render(g_shader_program);

        // Level 1 Intro
        if (!m_delay_complete) {
            Utility::draw_text(g_shader_program, g_font_texture_id_A, "LEVEL 1: ASTEROID PARTY", 0.3f, 0.0001f,
                glm::vec3(2.0, -3.0, 0.0f)); 
        }

        // Level 1 Complete
        if (passed_levelA) {
            Utility::draw_text(g_shader_program, g_font_texture_id_A, "LEVEL 1 COMPLETE", 0.35f, 0.0001f,
                glm::vec3(2.5, -2.5, 0.0f));
            Utility::draw_text(g_shader_program, g_font_texture_id_A, "Press Space to Continue", 0.3f, 0.0001f,
                glm::vec3(2.0, -3.5, 0.0f));
        }

        if (num_lives >= 1) {
            Utility::draw_text(g_shader_program, g_font_texture_id_A, "Lives:" + std::to_string(num_lives), 0.2f, 0.0001f,
                glm::vec3(g_state.player->get_position().x - 0.65f, g_state.player->get_position().y + 1.0f, 0.0f));
        }

        if (kill_count < 3 && g_state.player->get_touched()) {
            Utility::draw_text(g_shader_program, g_font_texture_id_A, "YOU LOSE!", 0.5f, 0.05f,
                glm::vec3(g_state.player->get_position().x + 1.5f, g_state.player->get_position().y, 0.0f));
        }
    }


}
