#pragma once

#include <array>
#include <cstdint>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"

class Practice {
public:
    Practice(InputManager& input, DisplayManager& display, Buzzer& buzzer);

    void start();
    void update(float dt);
    void render();

    bool isFinished() const;
    unsigned accuracyPercent() const;

private:
    struct Note {
        Color color;
        float x;
        bool active;
    };

    static constexpr std::size_t MaxNotes = 8;
    static constexpr float NoteSpeed = 70.0f;
    static constexpr float SpawnInterval = 0.75f;
    static constexpr float HitZoneLeft = 8.0f;
    static constexpr float HitZoneRight = 24.0f;
    static constexpr float NoteWidth = 8.0f;
    static constexpr float GameDuration = 30.0f;

    void spawnNote();
    void resolveInput();
    bool tryHit(Color color);
    void removeMissedNotes();

    InputManager& input_;
    DisplayManager& display_;
    Buzzer& buzzer_;
    std::array<Note, MaxNotes> notes_{};
    float elapsed_ = 0.0f;
    float spawnTimer_ = 0.0f;
    unsigned hits_ = 0;
    unsigned misses_ = 0;
    bool running_ = false;
};
