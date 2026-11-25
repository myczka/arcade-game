#pragma once

#include <array>
#include <cstdint>

#include "engine/input/input_manager.h"
#include "engine/output/buzzer.h"
#include "engine/output/display_manager.h"
#include "score.h"

class Practice {
public:
    Practice(InputManager& input, DisplayManager& display, Buzzer& buzzer, Score& score);

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
    static constexpr float NoteStep = 2.0f;
    static constexpr float NoteSpeed = NoteStep * 30.0f;
    static constexpr float SpawnInterval = 0.75f;
    static constexpr float HitZoneLeft = 8.0f;
    static constexpr float HitZoneRight = 24.0f;
    static constexpr float NoteWidth = 8.0f;
    static constexpr float GameDuration = 30.0f;
    // Leave enough time for the final note to travel fully off-screen.
    static constexpr float LastSpawnTime = GameDuration -
        (DisplayManager::Width + NoteWidth) / NoteSpeed;

    void spawnNote();
    void resolveInput();
    bool tryHit(Color color);
    void removeMissedNotes();

    InputManager& input_;
    DisplayManager& display_;
    Buzzer& buzzer_;
    Score& score_;
    std::array<Note, MaxNotes> notes_{};
    float elapsed_ = 0.0f;
    float spawnTimer_ = 0.0f;
    unsigned hits_ = 0;
    unsigned misses_ = 0;
    bool running_ = false;
    bool fullPresentNeeded_ = true;
    unsigned displayedAccuracy_ = 0;
    unsigned displayedRemaining_ = 0;
};
