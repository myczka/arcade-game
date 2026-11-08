#pragma once

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver,
    GameComplete
};

class GameManager {
   public:
    void init();
    void update(float dt);
    void render();
    void stateTransition(GameState newState);
   private:
    GameState* currentState;

};