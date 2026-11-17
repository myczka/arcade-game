#include "practice.h"

#include <cstdio>
#include <algorithm>
#include <cmath>

#include "pico/rand.h"

namespace {
constexpr uint16_t Red = 0xf800;
constexpr uint16_t Green = 0x03e0;
constexpr uint16_t Blue = 0x001f;
constexpr uint16_t Yellow = 0xfc00;
constexpr uint16_t TargetColor = 0xffff;

constexpr uint8_t NoteY = 124;
constexpr uint8_t NoteHeight = 12;
constexpr uint8_t HitZoneX = 8;
constexpr uint8_t HitZoneWidth = 16;

uint16_t colorValue(Color color) {
    switch (color) {
        case Color::Red: return Red;
        case Color::Green: return Green;
        case Color::Blue: return Blue;
        case Color::Yellow: return Yellow;
        case Color::Count: break;
    }
    return 0;
}

uint32_t toneFrequency(Color color) {
    switch (color) {
        case Color::Red: return 262;
        case Color::Green: return 330;
        case Color::Blue: return 392;
        case Color::Yellow: return 494;
        case Color::Count: break;
    }
    return 0;
}
}  // namespace

Practice::Practice(InputManager& input, DisplayManager& display, Buzzer& buzzer, Score& score)
    : input_(input), display_(display), buzzer_(buzzer), score_(score) {}

void Practice::start() {
    notes_ = {};
    elapsed_ = 0.0f;
    spawnTimer_ = SpawnInterval;
    hits_ = 0;
    misses_ = 0;
    running_ = true;
    fullPresentNeeded_ = true;
    score_.init(true);
}

void Practice::update(float dt) {
    if (!running_ || dt <= 0.0f) {
        return;
    }

    dt = std::min(dt, GameDuration - elapsed_);
    elapsed_ += dt;

    for (Note& note : notes_) {
        if (note.active) {
            note.x -= NoteSpeed * dt;
        }
    }

    spawnTimer_ -= dt;
    while (spawnTimer_ <= 0.0f) {
        const float spawnTime = elapsed_ + spawnTimer_;
        if (spawnTime <= LastSpawnTime) {
            // Account for the part of this frame since the scheduled spawn.
            spawnNote(-spawnTimer_);
        }
        spawnTimer_ += SpawnInterval;
    }

    resolveInput();
    removeMissedNotes();

    if (elapsed_ >= GameDuration) {
        for (Note& note : notes_) {
            if (note.active) {
                note.active = false;
                ++misses_;
            }
        }
        running_ = false;
    }
}

/* Optimized rendering, disabled while comparing the earlier version.
void Practice::render() {
    if (fullPresentNeeded_) {
        display_.clear();
        display_.drawText(4, 4, "PRACTICE");
    }

    const unsigned accuracy = accuracyPercent();
    char buffer[24];
    if (fullPresentNeeded_ || accuracy != displayedAccuracy_) {
        display_.drawRectangle(76, 4, DisplayManager::Width - 76, 7,
                               DisplayManager::Black);
        std::snprintf(buffer, sizeof(buffer), "ACC %u%%", accuracy);
        display_.drawText(76, 4, buffer);
    }

    // Restore only the moving lane; unchanged text stays in the framebuffer.
    display_.drawRectangle(0, NoteY - 2, DisplayManager::Width, NoteHeight + 4,
                           DisplayManager::Black);
    display_.drawRectangle(HitZoneX, NoteY - 2, HitZoneWidth, NoteHeight + 4,
                           TargetColor);

    for (const Note& note : notes_) {
        if (!note.active || note.x + NoteWidth <= 0.0f ||
            note.x >= DisplayManager::Width) {
            continue;
        }

        display_.drawRectangle(static_cast<int16_t>(std::floor(note.x)), NoteY,
                               static_cast<uint8_t>(NoteWidth),
                               NoteHeight, colorValue(note.color));
    }

    unsigned remaining = 0;
    if (elapsed_ < GameDuration) {
        remaining = static_cast<unsigned>(std::ceil(GameDuration - elapsed_));
    }
    if (fullPresentNeeded_ || remaining != displayedRemaining_) {
        display_.drawRectangle(4, 145, DisplayManager::Width - 4, 7,
                               DisplayManager::Black);
        std::snprintf(buffer, sizeof(buffer), "TIME %u", remaining);
        display_.drawText(4, 145, buffer);
    }
    if (fullPresentNeeded_) {
        display_.present();
        fullPresentNeeded_ = false;
    } else {
        // Covers old/new note positions and restores the hit-zone background.
        display_.present(0, NoteY - 2, DisplayManager::Width, NoteHeight + 4);
        if (accuracy != displayedAccuracy_) {
            display_.present(76, 4, DisplayManager::Width - 76, 7);
        }
        if (remaining != displayedRemaining_) {
            display_.present(4, 145, DisplayManager::Width - 4, 7);
        }
    }
    displayedAccuracy_ = accuracy;
    displayedRemaining_ = remaining;
}

*/

void Practice::render() {
    display_.clear();
    display_.drawText(4, 4, "PRACTICE");

    const unsigned accuracy = accuracyPercent();
    char buffer[24];
    score_.update(accuracy);
    score_.render();

    display_.drawRectangle(HitZoneX, NoteY - 2, HitZoneWidth, NoteHeight + 4, TargetColor);

    for (const Note& note : notes_) {
        if (!note.active || note.x + NoteWidth <= 0.0f ||
            note.x >= DisplayManager::Width) {
            continue;
        }

        display_.drawRectangle(static_cast<int16_t>(std::floor(note.x)), NoteY, static_cast<uint8_t>(NoteWidth), NoteHeight, colorValue(note.color));
    }

    unsigned remaining = 0;
    if (elapsed_ < GameDuration) {
        remaining = static_cast<unsigned>(std::ceil(GameDuration - elapsed_));
    }
    std::snprintf(buffer, sizeof(buffer), "TIME %u", remaining);
    display_.drawText(4, 145, buffer);

    if (fullPresentNeeded_) {
        display_.present();
        fullPresentNeeded_ = false;
    } else {
        // Covers old/new note positions and restores the hit-zone background.
        display_.present(0, NoteY - 2, DisplayManager::Width, NoteHeight + 4);
        if (accuracy != displayedAccuracy_) {
            display_.present(Score::X, Score::Y, Score::Width, Score::Height);
        }
        if (remaining != displayedRemaining_) {
            display_.present(4, 145, DisplayManager::Width - 4, 7);
        }
    }
    displayedAccuracy_ = accuracy;
    displayedRemaining_ = remaining;
}

bool Practice::isFinished() const {
    return !running_;
}

unsigned Practice::accuracyPercent() const {
    const unsigned attempts = hits_ + misses_;
    return attempts == 0 ? 0 : (hits_ * 100u) / attempts;
}

void Practice::spawnNote(float age) {
    for (Note& note : notes_) {
        if (!note.active) {
            note.color = static_cast<Color>(get_rand_32() % 4u);
            note.x = DisplayManager::Width - NoteSpeed * age;
            note.active = true;
            return;
        }
    }
}

void Practice::resolveInput() {
    constexpr Color colors[] = {
        Color::Red, Color::Green, Color::Blue, Color::Yellow};

    for (Color color : colors) {
        if (input_.wasPressed(color)) {
            tryHit(color);
        }
    }
}

bool Practice::tryHit(Color color) {
    for (Note& note : notes_) {
        if (note.active && note.color == color &&
            note.x >= HitZoneLeft - NoteWidth &&
            note.x <= HitZoneRight) {
            note.active = false;
            ++hits_;
            buzzer_.play(toneFrequency(color), 100);
            return true;
        }
    }

    return false;
}

void Practice::removeMissedNotes() {
    for (Note& note : notes_) {
        if (note.active && note.x + NoteWidth < 0.0f) {
            note.active = false;
            ++misses_;
        }
    }
}
