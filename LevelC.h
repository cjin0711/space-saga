#include "Scene.h"

class LevelC : public Scene {
public:
    float m_spaceship_one_beam_timer;
    float m_spaceship_two_beam_timer;
    float m_missile_timer;
    float m_start_delay;   // Delay duration in seconds
    bool m_delay_complete;    // Flag to indicate if the delay is over

    ~LevelC();

    void shoot_player_beams();
    void shoot_enemy_one_beams();
    void shoot_enemy_two_beams();
	void shoot_enemy_missile();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
