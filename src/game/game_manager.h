#pragma once

enum class GameState {
    MainMenuState,
    ReactionState,
    AccuracyState,
    MemoryState,
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