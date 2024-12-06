#define LOG(argument) std::cout << argument << '\n'
#define FIXED_TIMESTEP 0.0166666f
#define PLATFORM_COUNT 21

// FOR MAP GENERATION
#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 11


#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include "cmath"
#include <ctime>
#include <cstdlib>
#include <string>
#include <windows.h>

#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"

// ����� CONSTANTS ����� //
const int WINDOW_WIDTH = 640 * 1.5,
WINDOW_HEIGHT = 480 * 1.5;

const float BG_RED = 0.0f,     // Red component
BG_GREEN = 0.0f,               // Green component
BG_BLUE = 0.25f,                // Blue component
BG_OPACITY = 1.0f;             // Opacity

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

// FONT
constexpr char FONTSHEET_FILEPATH[] = "assets/font1.png";

const float MILLISECONDS_IN_SECOND = 1000.0;


// ����� GLOBAL VARIABLES ����� //  
Scene* g_current_scene;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;
Menu* g_menu;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool left_movement = false;
bool right_movement = false;

// To switch between scenes
void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise()
{
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Physics (again)!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ����� VIDEO ����� //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);

	// ����� MENU SETUP ����� //
	g_menu = new Menu();
    switch_to_scene(g_menu);

    // ����� LEVEL A SETUP ����� //
    g_level_a = new LevelA();
    //switch_to_scene(g_level_a);

    // ----- Level B SETUP ----- //
    g_level_b = new LevelB();
    //switch_to_scene(g_level_b);

	// ����� LEVEL C SETUP ����� //
	g_level_c = new LevelC();
    //switch_to_scene(g_level_c);


    // ����� GENERAL ����� //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void process_input()
{

    g_current_scene->g_state.player->set_movement(glm::vec3(0.0f));

    // NEWLY ADDED LINE FIXES SHOOTING FOR LEVEL C
	g_current_scene->g_state.player->set_shooting(false);

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                // SCENE SWITCHING
                if (passed_levelB && !lost_game) {
                    switch_to_scene(g_level_c);
                }
                else if (passed_levelA && !lost_game) {
                    switch_to_scene(g_level_b);
                }

                break;

            case SDLK_h:
                // Stop music
                Mix_HaltMusic();
                break;

            case SDLK_p:
                Mix_PlayMusic(g_current_scene->g_state.music, -1);

            case SDLK_RETURN:
				switch_to_scene(g_level_c);
                break;

            default:
                break;
            }

        default:
            break;
        }
    }
    if (g_current_scene->g_state.player->get_touched()) {
        return;
    }

    if (key_state[SDL_SCANCODE_C]) {
        g_current_scene->g_state.player->fireball();
        Mix_PlayChannel(-1, g_current_scene->g_state.jump_sfx, 0);

    }
    if (key_state[SDL_SCANCODE_UP]) {
        g_current_scene->g_state.player->move_up();
    }
    else if (key_state[SDL_SCANCODE_DOWN]) {
        g_current_scene->g_state.player->move_down();
    }

    if (key_state[SDL_SCANCODE_LEFT]) {
        g_current_scene->g_state.player->move_left();
    }
    else if (key_state[SDL_SCANCODE_RIGHT]) {
        g_current_scene->g_state.player->move_right();
    }


    if (glm::length(g_current_scene->g_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->g_state.player->normalise_movement();
    }

}

void update()
{

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {

        g_accumulator = delta_time;
        return;
    }



    while (delta_time >= FIXED_TIMESTEP)
    {

        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;

    }


    g_view_matrix = glm::mat4(1.0f);

    // Camera Follows the player
    //g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->g_state.player->get_position().x - 4.0f, -g_current_scene->g_state.player->get_position().y, 0.0f));
    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-4.5f, 3.0f, 0.0f));


    g_accumulator = delta_time;
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    g_program.set_view_matrix(g_view_matrix);

    g_current_scene->render(&g_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    delete g_level_a;

	delete g_level_b;

    delete g_level_c;
    
    //delete g_state.enemies;
    //delete g_state.player;
    //delete g_state.fireball;
    //delete g_state.map;
}

// ����� GAME LOOP ����� //
int main(int argc, char* argv[])
{
    initialise();


    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }


    shutdown();
    return 0;
}