#include "swimmy_fish.h"
#include "assets/red_fish.h"
#include "assets/blue_fish.h"
#include "assets/green_fish.h"
#include "assets/yellow_fish.h"
#include "assets/sky_fish.h"

#include <algorithm>
#include <cmath>
#include <array>

#include "pico/rand.h"

using FishFrames = const uint16_t (*)[1024];

const std::array<FishFrames, static_cast<size_t>(Color::Count)> fish_sprs = {
    red_fish,
    green_fish,
    blue_fish,
    yellow_fish};

SwimmyFish::SwimmyFish(InputManager& input, DisplayManager& display, Buzzer& buzzer) : input_(input), display_(display), buzzer_(buzzer) {

}

void SwimmyFish::fishLocation() {
    fish_cntr_y = fish_y + 16;
    fish_top = fish_cntr_y - 5;
    fish_bottom = fish_cntr_y + 5;
}

bool SwimmyFish::hasCollision() const {
    if (fish_left < 0.0f || fish_right > DisplayManager::Width ||
        fish_top < 0.0f || fish_bottom > DisplayManager::Height) {
        return true;
    }

    for (const Pipe& pipe : pipes) {
        const bool overlaps_x = fish_right > pipe.x &&
                                fish_left < pipe.x + PIPE_WIDTH;
        if (!overlaps_x) {
            continue;
        }
        const float gap_top = pipe.gap_y - GAP_HEIGHT / 2.0f;
        const float gap_bottom = pipe.gap_y + GAP_HEIGHT / 2.0f;
        if (fish_top < gap_top || fish_bottom > gap_bottom) {
            return true;
        }
    }
    return false;
}

void SwimmyFish::start() {
    //fish_x = fish_start_x;
    fish_y = fish_start_y;

    fish_velocity = 0;
    transition_frames_ = 0;
    running_ = true;
    sound_frames_ = 0;
    sound_volume_ = 255;
    sound_c3_ = true;
    color_state_ = static_cast<Color>(get_rand_32() % 4u);

    display_.drawSprite(fish_x, fish_y, fish_sprs.at(static_cast<int>(color_state_))[1], 32, 32);

    for (unsigned i = 0; i < 2; ++i) {
        resetPipe(pipes[i], -static_cast<float>(PIPE_WIDTH) - i * PIPE_SPACING);
    }
    full_present_needed = true;
    fishLocation();

    buzzer_.stop();
}

void SwimmyFish::update(float dt) {
    if (!running_ || dt <= 0.0f) {
        return;
    }
    //meth
    fish_velocity = fish_velocity + GRAVITY * dt;
    fish_y = fish_y + fish_velocity * dt;

    if (transition_frames_ > 0) {
        --transition_frames_;
    }
    if (input_.wasPressed(color_state_)) {
        fish_velocity = fish_velocity - 500;
        transition_frames_ = 3;
    }
    
    // For the love of the game
    //color_state_ = static_cast<Color>(get_rand_32() % 4u);

    fishLocation();
    updatePipes(dt);

    if (hasCollision()) {
        running_ = false;
        fish_velocity = 0.0f;
        buzzer_.stop();
    }
    else if (fish_cntr_x < pipes[0].x) {
        pipes[0] = pipes[1];
        pipes[1] = {-PIPE_SPEED, (7 + get_rand_32() % (GAP_HEIGHT - 7 + 1))};
        color_state_ = static_cast<Color>(get_rand_32() % 4u);
    }
    if (running_) {
        updateSwimSound();
    }
}

void SwimmyFish::updateSwimSound() {
    constexpr uint8_t FramesPerNote = 15;
    constexpr uint32_t NoteDurationMs = 100;
    constexpr uint8_t FadeStep = 16;

    if (sound_volume_ == 0 || ++sound_frames_ < FramesPerNote) {
        return;
    }
    sound_frames_ = 0;

    // Rounded C3 (130.81 Hz) and G3 (196.00 Hz).
    uint32_t frequency = 196;
    if (sound_c3_) {
        frequency = 131;
    }
    buzzer_.play(frequency, NoteDurationMs, sound_volume_);
    sound_c3_ = !sound_c3_;
    if (sound_volume_ > FadeStep) {
        sound_volume_ -= FadeStep;
    } else {
        sound_volume_ = 0;
    }
}

bool SwimmyFish::isFinished() const {
    return !running_;
}

void SwimmyFish::render() {
    // Compose in RAM first. Fish sprites can be drawn after the pipes here.
    display_.clear(BACKGROUND);
    drawPipes();

    // Briefly use the neutral pose after each flap, then follow velocity.
    unsigned frame = 1;
    if (transition_frames_ > 0) {
        frame = 1;
    } else if (fish_velocity < 0) {
        frame = 0;
    } else if (fish_velocity > 0) {
        frame = 2;
    }
    const auto draw_y = static_cast<int16_t>(std::floor(fish_y));
    display_.drawSprite(fish_x, draw_y,
        fish_sprs.at(static_cast<int>(color_state_))[frame], 32, 32);

    if (full_present_needed) {
        display_.present();
        full_present_needed = false;
    } else {
        for (unsigned i = 0; i < 2; ++i) {
            if (pipes[i].x < displayed_pipes[i].x) {
                // Recycling jumps across the screen: erase and draw separately.
                presentPipeRegion(displayed_pipes[i], displayed_pipes[i]);
                presentPipeRegion(pipes[i], pipes[i]);
            } else {
                presentPipeRegion(displayed_pipes[i], pipes[i]);
            }
        }
        // Both regions use the finished scene, erasing the old fish position.
        display_.present(fish_x, displayed_fish_y_, 32, 32);
        display_.present(fish_x, draw_y, 32, 32);
    }
    displayed_fish_y_ = draw_y;
    for (unsigned i = 0; i < 2; ++i) {
        displayed_pipes[i] = pipes[i];
    }

}

void SwimmyFish::resetPipe(Pipe& pipe, float x) {
    constexpr unsigned MinimumGapY = PIPE_MARGIN + GAP_HEIGHT / 2;
    constexpr unsigned MaximumGapY = DisplayManager::Height - MinimumGapY;
    pipe.x = x;
    pipe.gap_y = static_cast<float>(MinimumGapY +
        get_rand_32() % (MaximumGapY - MinimumGapY + 1));
    pipe.scored = false;
}

void SwimmyFish::updatePipes(float dt) {
    for (Pipe& pipe : pipes) {
        pipe.x += PIPE_SPEED * dt;
    }
    for (Pipe& pipe : pipes) {
        if (pipe.x >= DisplayManager::Width) {
            const float leftmost = std::min(pipes[0].x, pipes[1].x);
            resetPipe(pipe, std::min(-static_cast<float>(PIPE_WIDTH),
                                     leftmost - PIPE_SPACING));
        }
    }
}

void SwimmyFish::drawPipes() {
    for (const Pipe& pipe : pipes) {
        if (pipe.x >= DisplayManager::Width || pipe.x + PIPE_WIDTH <= 0.0f) {
            continue;
        }
        const auto x = static_cast<int16_t>(std::floor(pipe.x));
        const auto gap_top = static_cast<uint8_t>(pipe.gap_y - GAP_HEIGHT / 2);
        const auto gap_bottom = static_cast<uint8_t>(pipe.gap_y + GAP_HEIGHT / 2);
        display_.drawRectangle(x, 0, PIPE_WIDTH, gap_top, PIPE_COLOR);
        display_.drawRectangle(x, gap_bottom, PIPE_WIDTH,
                               DisplayManager::Height - gap_bottom, PIPE_COLOR);
    }
}

void SwimmyFish::presentPipeRegion(const Pipe& previous, const Pipe& current) {
    const float left = std::max(0.0f, std::floor(std::min(previous.x, current.x)));
    const float right = std::min(static_cast<float>(DisplayManager::Width),
        std::ceil(std::max(previous.x, current.x) + PIPE_WIDTH));
    if (right <= left) {
        return;
    }
    // Cover both the old and new pipe extents, leaving the shared gap untouched.
    const auto top_height = static_cast<uint8_t>(
        std::max(previous.gap_y, current.gap_y) - GAP_HEIGHT / 2);
    const auto bottom_y = static_cast<uint8_t>(
        std::min(previous.gap_y, current.gap_y) + GAP_HEIGHT / 2);
    display_.present(static_cast<int16_t>(left), 0,
                      static_cast<uint8_t>(right - left), top_height);
    display_.present(static_cast<int16_t>(left), bottom_y,
                      static_cast<uint8_t>(right - left),
                      DisplayManager::Height - bottom_y);
}
