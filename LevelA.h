#include "Scene.h"

class LevelA : public Scene {
public:
    float m_wave_timer;
    float m_asteroid_timer;
    const float WAVE_DURATION = 24.0f;
    float m_start_delay;   // Delay duration in seconds
    bool m_delay_complete;    // Flag to indicate if the delay is over

    ~LevelA();

    void wave_one();
    void wave_two();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
}; 
