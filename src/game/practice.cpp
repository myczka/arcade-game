#include "practice.h"

#include <cstdio>

#include "pico/rand.h"

namespace {
constexpr std::uint16_t Red = 0xf800;
constexpr std::uint16_t Green = 0x03e0;
constexpr std::uint16_t Blue = 0x001f;
constexpr std::uint16_t Yellow = 0xfc00;
constexpr std::uint16_t TargetColor = 0xffff;

constexpr std::uint8_t NoteY = 124;
constexpr std::uint8_t NoteHeight = 12;
constexpr std::uint8_t HitZoneX = 8;
constexpr std::uint8_t HitZoneWidth = 16;

std::uint16_t colorValue(Color color) {
    switch (color) {
        case Color::Red: return Red;
        case Color::Green: return Green;
        case Color::Blue: return Blue;
        case Color::Yellow: return Yellow;
        case Color::Count: break;
    }
    return 0;
}

std::uint32_t toneFrequency(Color color) {
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

Practice::Practice(InputManager& input, DisplayManager& display, Buzzer& buzzer)
    : input_(input), display_(display), buzzer_(buzzer) {}

void Practice::start() {
    notes_ = {};
    elapsed_ = 0.0f;
    spawnTimer_ = SpawnInterval;
    hits_ = 0;
    misses_ = 0;
    running_ = true;
}

void Practice::update(float dt) {
    if (!running_ || dt <= 0.0f) {
        return;
    }

    elapsed_ += dt;
    spawnTimer_ -= dt;

    while (spawnTimer_ <= 0.0f) {
        spawnNote();
        spawnTimer_ += SpawnInterval;
    }

    for (Note& note : notes_) {
        if (note.active) {
            note.x -= NoteSpeed * dt;
        }
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

void Practice::render() {
    display_.clear();
    display_.drawText(4, 4, "PRACTICE");

    char buffer[24];
    std::snprintf(buffer, sizeof(buffer), "ACC %u%%", accuracyPercent());
    display_.drawText(76, 4, buffer);

    display_.drawRectangle(HitZoneX, NoteY - 2, HitZoneWidth, NoteHeight + 4,
                           TargetColor);

    for (const Note& note : notes_) {
        if (!note.active || note.x < 0.0f || note.x > 128.0f) {
            continue;
        }

        display_.drawRectangle(static_cast<std::uint8_t>(note.x), NoteY,
                               static_cast<std::uint8_t>(NoteWidth),
                               NoteHeight, colorValue(note.color));
    }

    const unsigned remaining =
        elapsed_ >= GameDuration
            ? 0
            : static_cast<unsigned>(GameDuration - elapsed_);
    std::snprintf(buffer, sizeof(buffer), "TIME %u", remaining);
    display_.drawText(4, 145, buffer);
    display_.present();
}

bool Practice::isFinished() const {
    return !running_;
}

unsigned Practice::accuracyPercent() const {
    const unsigned attempts = hits_ + misses_;
    return attempts == 0 ? 0 : (hits_ * 100u) / attempts;
}

void Practice::spawnNote() {
    for (Note& note : notes_) {
        if (!note.active) {
            note.color = static_cast<Color>(
                get_rand_32() % 4u);
            note.x = 128.0f;
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
