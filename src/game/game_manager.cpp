#include "game_manager.h"


namespace {
constexpr int MenuOptionCount = static_cast<int>(MenuOption::Count);

bool isRemoteUp(int command) {
    return command == 0x08;
}

bool isRemoteDown(int command) {
    return command == 0x5a;
}

bool isRemoteEnter(int command) {
    return command == 0x1c;
}

bool isRemoteExit(int command) {
    return command == 0x16 || command == 0x42;
}
}

GameManager::GameManager(InputManager& input, DisplayManager& display, Buzzer& buzzer)
    : input_(input), display_(display), buzzer_(buzzer), score_(display),
      practice_(input, display, buzzer, score_), fishy_time_(input, display, buzzer, score_) {}

void GameManager::init() {
    selected = MenuOption::SwimmyFish;
    stateTransition(GameState::Title);
}

void GameManager::stateTransition(GameState newState) {
    buzzer_.stop();
    currentState = newState;
    screenNeedsUpdate = true;

    if (newState == GameState::PracticeGame) {
        practice_.start();
    } else if (newState == GameState::SwimmyFish) {
        fishy_time_.start();
    }
}

void GameManager::openSelectedOption() {
    if (selected == MenuOption::Practice) {
        stateTransition(GameState::PracticeGame);
    } else if (selected == MenuOption::SwimmyFish) {
        stateTransition(GameState::SwimmyFish);
    } else if (selected == MenuOption::Results) {
        stateTransition(GameState::Results);
    }
}

void GameManager::update(float dt) {
    const int command = input_.getRemoteCommand();

    switch (currentState) {
        case GameState::Title:
            if (isRemoteEnter(command)) {
                stateTransition(GameState::MainMenu);
            }
            break;

        case GameState::MainMenu:
            if (isRemoteUp(command)) {
                selected = static_cast<MenuOption>((static_cast<int>(selected) + MenuOptionCount - 1) % MenuOptionCount);
                screenNeedsUpdate = true;
            } else if (isRemoteDown(command)) {
                selected = static_cast<MenuOption>((static_cast<int>(selected) + 1) % MenuOptionCount);
                screenNeedsUpdate = true;
            } else if (isRemoteEnter(command)) {
                openSelectedOption();
            }
            break;

        case GameState::PracticeGame:
            if (isRemoteExit(command)) {
                stateTransition(GameState::MainMenu);
            } else {
                practice_.update(dt);
            }
            break;

        case GameState::SwimmyFish:
            if (isRemoteExit(command)) {
                stateTransition(GameState::MainMenu);
            } else {
                fishy_time_.update(dt);
            }
            break;

        case GameState::Results:
            if (isRemoteExit(command) || isRemoteEnter(command)) {
                stateTransition(GameState::MainMenu);
            }
            break;
    }
}

void GameManager::render() {
    switch (currentState) {
        case GameState::Title:
            if (screenNeedsUpdate) {
                menu_.renderTitle(display_);
                screenNeedsUpdate = false;
            }
            break;

        case GameState::MainMenu:
            if (screenNeedsUpdate) {
                menu_.render(display_, selected);
                screenNeedsUpdate = false;
            }
            break;

        case GameState::PracticeGame:
            practice_.render();
            if (practice_.isFinished()) {
                score_.finish();
                stateTransition(GameState::MainMenu);
            }
            break;

        case GameState::SwimmyFish:
            fishy_time_.render();
            if (fishy_time_.isFinished()) {
                score_.finish();
                stateTransition(GameState::MainMenu);
            }
            break;

        case GameState::Results:
            if (screenNeedsUpdate) {
                score_.renderResults();
                screenNeedsUpdate = false;
            }
            break;
    }
}
