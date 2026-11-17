#pragma once

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "main_menu.h"
#include "practice.h"
#include "score.h"
#include "swimmy_fish.h"
#include "tutorial.h"

enum class GameState {
    Title,
    MainMenu,
    Instructions,
    PracticeGame,
    SwimmyFish,
    Results
};

class GameManager {
   public:
    GameManager(InputManager& input, DisplayManager& display, Buzzer& buzzer);
    void init();
    void update(float dt);
    void render();
    void stateTransition(GameState newState);

   private:
    void openSelectedOption();

    InputManager& input_;
    DisplayManager& display_;
    Buzzer& buzzer_;
    MainMenu menu_;
    Score score_;
    Practice practice_;
    SwimmyFish fishy_time_;
    Tutorial tutorial_;
    GameState currentState = GameState::Title;
    MenuOption selected = MenuOption::SwimmyFish;
    bool screenNeedsUpdate = true;
};
