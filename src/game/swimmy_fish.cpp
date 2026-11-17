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

constexpr std::array<uint32_t, 4> color_freqs = {110, 147, 261, 783};
const std::array<FishFrames, static_cast<size_t>(Color::Count)> fish_sprs = {
    red_fish,
    green_fish,
    blue_fish,
    yellow_fish};

SwimmyFish::SwimmyFish(InputManager& input, DisplayManager& display, Buzzer& buzzer, Score& score)
    : input_(input), display_(display), buzzer_(buzzer), score_(score) {

}

void SwimmyFish::fishLocation() {
    fish_cntr_y = fish_y + 16;
    fish_top = fish_cntr_y - 5;
    fish_bottom = fish_cntr_y + 5;
}

bool SwimmyFish::hasCollision() const {
    if (fish_top < 0.0f) {
        return true;
    }

    for (const Pipe& pipe : pipes) {
        const float pipe_left = std::floor(pipe.x);
        const bool overlaps_x = fish_right > pipe_left &&
                                fish_left < pipe_left + PIPE_WIDTH;
        if (!overlaps_x) {
            continue;
        }
        const float gap_top = std::floor(pipe.gap_y - GAP_HEIGHT / 2.0f);
        const float gap_bottom = gap_top + GAP_HEIGHT;
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
    ready_frames_ = 30;
    running_ = true;
    score_.init(false);
    color_state_ = static_cast<Color>(get_rand_32() % 4u);

    display_.drawSprite(fish_x, fish_y, fish_sprs.at(static_cast<int>(color_state_))[1], 32, 32);

    for (unsigned i = 0; i < 2; ++i) {
        resetPipe(pipes[i], -static_cast<float>(PIPE_SPACING) - i * PIPE_SPACING);
    }
    full_present_needed = true;
    fishLocation();

    buzzer_.stop();
}

// Update function that runs in game loop
void SwimmyFish::update(float dt) {
    if (!running_ || dt <= 0.0f) {
        return;
    }
    // Show the starting pose for 30 frames before accepting flaps or moving.
    if (ready_frames_ > 0) {
        --ready_frames_;
        return;
    }
    //meth
    fish_velocity = fish_velocity + GRAVITY * dt;
    fish_y = fish_y + fish_velocity * dt;

    // Evaluate transition frames for up and down
    if (transition_frames_ > 0) {
        --transition_frames_;
    } else if (transition_frames_ < 0) {
        ++transition_frames_;
    }

    // Check button but also sets transition frames for up/down
    if (input_.wasPressed(color_state_)) {
        fish_velocity = -103;
        transition_frames_ = 2;
        buzzer_.sweep(160, 800, 300, 32);
    }
    else if (fish_velocity == 0) {
        transition_frames_ = -3;
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

    // Checks if running?
    if (running_) {
        for (Pipe& pipe : pipes) {
            if (!pipe.scored && std::floor(pipe.x) > fish_cntr_x) {
                pipe.scored = true;
                score_.update(score_.value() + 1);
                color_state_ = static_cast<Color>(get_rand_32() % 4u);
                
                // Ding for color change
                buzzer_.play(color_freqs.at(static_cast<int>(color_state_)), 500, 250, 30);
            }
        }
    }
}

bool SwimmyFish::isFinished() const {
    return !running_;
}

void SwimmyFish::render() {
    // Compose in RAM first. Fish sprites can be drawn after the pipes here.
    display_.clear(BACKGROUND);
    drawPipes();

    // forces the neutral frame for 2 frames after button press
    unsigned frame = 1;
    if (transition_frames_ > 0 || transition_frames_ < 0) {
        frame = 1;
    } else if (fish_velocity < 0) {
        frame = 0;
    } else if (fish_velocity > 0) {
        frame = 2;
    }
    
    const auto draw_y = static_cast<int16_t>(std::floor(fish_y));
    display_.drawSprite(fish_x, draw_y, fish_sprs.at(static_cast<int>(color_state_))[frame], 32, 32);

    score_.render();

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
        display_.present(Score::X, Score::Y, Score::Width, Score::Height);
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
    pipe.gap_y = static_cast<float>(MinimumGapY + get_rand_32() % (MaximumGapY - MinimumGapY + 1));
    pipe.scored = false;
}

void SwimmyFish::updatePipes(float dt) {
    for (Pipe& pipe : pipes) {
        pipe.x += PIPE_SPEED * dt;
    }
    for (Pipe& pipe : pipes) {
        if (pipe.x >= DisplayManager::Width) {
            const float leftmost = std::min(pipes[0].x, pipes[1].x);
            resetPipe(pipe, std::min(-static_cast<float>(PIPE_WIDTH), leftmost - PIPE_SPACING));
        }
    }
}

void SwimmyFish::drawPipes() {
    for (const Pipe& pipe : pipes) {
        if (pipe.x >= DisplayManager::Width || pipe.x + PIPE_WIDTH <= 0.0f) {
            continue;
        }
        const auto x = static_cast<int16_t>(std::floor(pipe.x));
        const auto gap_top = static_cast<uint8_t>(std::floor(pipe.gap_y - GAP_HEIGHT / 2.0f));
        const auto gap_bottom = static_cast<uint8_t>(gap_top + GAP_HEIGHT);
        display_.drawRectangle(x, 0, PIPE_WIDTH, gap_top, PIPE_COLOR);
        display_.drawRectangle(x, gap_bottom, PIPE_WIDTH, DisplayManager::Height - gap_bottom, PIPE_COLOR);
    }
}

void SwimmyFish::presentPipeRegion(const Pipe& previous, const Pipe& current) {
    const float left = std::max(0.0f, std::floor(std::min(previous.x, current.x)));
    const float right = std::min(static_cast<float>(DisplayManager::Width), std::ceil(std::max(previous.x, current.x) + PIPE_WIDTH));
    if (right <= left) {
        return;
    }
    // Cover both the old and new pipe extents, leaving the shared gap untouched.
    const auto top_height = static_cast<uint8_t>(std::floor(std::max(previous.gap_y, current.gap_y) - GAP_HEIGHT / 2.0f));
    const auto bottom_y = static_cast<uint8_t>(std::floor(std::min(previous.gap_y, current.gap_y) - GAP_HEIGHT / 2.0f) + GAP_HEIGHT);
    display_.present(static_cast<int16_t>(left), 0, static_cast<uint8_t>(right - left), top_height);
    display_.present(static_cast<int16_t>(left), bottom_y, static_cast<uint8_t>(right - left), DisplayManager::Height - bottom_y);
}
