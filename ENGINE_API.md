# Engine Public API Reference

The engine provides button and remote input, buffered graphics, and buzzer output. Game code uses `InputManager`, `DisplayManager`, and `Buzzer`; `IRRemote` is available when direct receiver access is needed.

This reference covers the explicitly declared public functions in `src/engine`, including constructors. It describes the current implementation. Game classes under `src/game` and the vendored ST7735 driver's lower-level functions are outside this API.

## How to use the engine

Create the engine objects once and keep them alive while games reference them. Call each object's `init()` before using its hardware. During the main loop:

1. Call `InputManager::update()` once before reading button and remote events.
2. Update the active game's state using those events.
3. Call `Buzzer::update()` regularly so expired sounds stop.
4. Draw the scene into RAM and present the regions that changed.

Drawing primitives do not transmit pixels. Presentation transmits them and waits for completion. Complete screen methods such as `showTitle()` already present their output.

## Dependency injection by reference

The games use **constructor dependency injection**: code outside the game creates the input, display, and buzzer objects, then supplies them through the game's constructor. This is a design pattern, not a requirement of the C++ language or a separate framework.

For example, [Practice](src/game/practice.h) receives the existing engine objects:

```cpp
Practice practice(input, display, buzzer);
```

Its constructor binds the reference members in its initializer list:

```cpp
Practice::Practice(InputManager& input, DisplayManager& display, Buzzer& buzzer)
    : input_(input), display_(display), buzzer_(buzzer) {}
```

The `&` members refer to the original objects; they do not copy or own them. `update()` can call `input_.wasPressed(...)`, and `render()` can call `display_.drawSprite(...)` using those stored references. The referenced objects must outlive the game, and C++ reference members must be initialized when the game is constructed.

Keep these responsibilities separate:

- The application creates and initializes the engine objects once.
- Each game receives only the services it needs and uses their public APIs.
- The application or game manager selects the active game and calls its lifecycle functions. `start()` resets game state without replacing the engine references.

This keeps dependencies visible in the constructor and allows several games to share the same hardware services without global manager pointers. The current references use concrete classes; replacing them with test doubles would require an interface or another substitution mechanism.

## InputManager

Header: [input_manager.h](src/engine/input/input_manager.h). Implementation: [input_manager.cpp](src/engine/input/input_manager.cpp).

```cpp
#include "engine/input/input_manager.h"
```

### Construction and updates

| Public function | Behavior |
| --- | --- |
| `InputManager()` | Creates the button records for the four configured GPIO pins and allocates an `IRRemote` for `REMOTE_GPIO`. Does not configure GPIO until `init()`. |
| `void init()` | Configures the buttons as inputs with pull-ups, records their initial levels, and initializes the remote receiver. Wiring is active-low: pressing a button connects its input to ground. |
| `void update()` | Moves each button's current state into its previous state, samples the new state, and polls the receiver for an accepted remote command. Call once per input sampling cycle. |

### Button queries

Each query takes one of `Color::Red`, `Color::Green`, `Color::Blue`, or `Color::Yellow`. `Color::Count` is the array-size marker; do not pass it as a button.

| Public function | Returns |
| --- | --- |
| `bool wasPressed(Color color) const` | `true` when the current sample is pressed and the previous sample was released. |
| `bool wasReleased(Color color) const` | `true` when the current sample is released and the previous sample was pressed. |
| `bool isHeld(Color color) const` | Currently returns `true` only when both the current and previous samples are pressed. It is false on the first pressed sample. |

These queries read stored samples. Calling a query does not consume the event, so repeated queries before the next `update()` give the same answer. Physical button debounce is not implemented.

**Initialization detail:** `init()` currently stores raw GPIO levels, while `update()` stores inverted levels. The initial sample therefore uses the opposite meaning from later samples. For example, the first update can report a release for a button that was never pressed. This is existing behavior, not an intended input contract to rely on.

```cpp
input.update();
if (input.wasPressed(Color::Blue)) {
    // Apply the game's swim action once for this press.
}
```

### Remote queries

| Public function | Returns |
| --- | --- |
| `int getRemoteCommand() const` | The command accepted during the most recent `update()`, or `-1` if that update accepted no command. |
| `int getLastRemoteCommand() const` | The most recently accepted command, retained across updates without a new command. Initially `-1`. |

Neither getter polls hardware or consumes a command. Both depend on `update()` polling the owned receiver. `getRemoteCommand()` is suitable for menu actions; `getLastRemoteCommand()` can keep returning an old action and should not be treated as a new press every frame.

The receiver stores commands as `int8_t`, so these getters do not currently provide a reliable nonnegative representation of NEC command bytes 128 through 255. Widening the manager's return type to `int` does not fix the receiver's signed storage.

### Button definitions

Definitions: [button.h](src/engine/input/button.h).

| Identifier | Value or purpose |
| --- | --- |
| `Color::Red` / `RED_BTN_GPIO` | Red button on GPIO 2. |
| `Color::Green` / `GREEN_BTN_GPIO` | Green button on GPIO 3. |
| `Color::Blue` / `BLUE_BTN_GPIO` | Blue button on GPIO 4. |
| `Color::Yellow` / `YELLOW_BTN_GPIO` | Yellow button on GPIO 5. |
| `Color::Count` | Number of buttons: 4. |
| `Button::pin` | GPIO assigned to that button. |
| `Button::current`, `Button::previous` | Stored Boolean input samples. Both default to `false` before initialization. |

`Button` is a data structure with no explicitly declared functions. There is no white physical button. White remains available as a graphics color.

## IRRemote

Header: [ir_remote.h](src/engine/input/ir_remote.h). Implementation: [ir_remote.cpp](src/engine/input/ir_remote.cpp).

```cpp
#include "engine/input/ir_remote.h"
```

`InputManager` normally owns this receiver. Its GPIO interrupt callback routes events through one active receiver pointer. Creating another `IRRemote` replaces that pointer, so do not create a separate receiver alongside an active `InputManager`.

| Public function | Parameters and behavior |
| --- | --- |
| `explicit IRRemote(uint8_t pin)` | Stores the receiver's GPIO, initializes capture state, sets the last command to `-1`, and registers this object as the active receiver. `REMOTE_GPIO` is currently 7. |
| `void init()` | Configures the receiver pin as input without internal pulls and enables interrupts on rising and falling edges. |
| `bool receivedCommand()` | Processes a completed NEC frame if available. Returns `true` when a valid command is accepted; returns `false` for no completed frame, an invalid frame, or a suppressed repeat. Processing a completed frame clears its capture state. |
| `int8_t getLastReceivedCommand() const` | Returns the stored command without polling or consuming anything. Initially `-1`. The signed-byte range limitation described above also applies here. |

Repeated identical commands are suppressed if fewer than 150,000 microseconds have elapsed since the last accepted command. A different valid command can be accepted during that interval. NEC repeat frames reuse the previous command and pass through the same cooldown.

The `receivedCommand()` declaration's existing comment mentions a command value or `-1`, but its actual return type and implementation are Boolean. Retrieve the value separately with `getLastReceivedCommand()`.

## DisplayManager

Header: [display_manager.h](src/engine/output/display_manager.h). Implementation: [display_manager.cpp](src/engine/output/display_manager.cpp).

```cpp
#include "engine/output/display_manager.h"
```

### Coordinates and colors

| Public constant | Meaning |
| --- | --- |
| `DisplayManager::Width` | 128 pixels. |
| `DisplayManager::Height` | 160 pixels. |
| `DisplayManager::Black` | RGB565 value `0x0000`. |
| `DisplayManager::White` | RGB565 value `0xffff`. |
| `DisplayManager::Transparent` | Reserved sprite transparency key `0xF81F`. |

The origin is the top-left corner. Increasing x moves right; increasing y moves down. Colors are native `uint16_t` RGB565 words: five red bits, six green bits, and five blue bits. Sprite arrays should not be byte-swapped.

Rectangle, sprite, and presentation positions are signed so objects can move partly outside the display. Their dimensions are unsigned eight-bit values, limited to 0 through 255. Text positions use unsigned eight-bit values and do not support negative placement.

### Initialization

```cpp
void init();
```

Requests 10 MHz SPI in mode 0, sets up the transmit and clock pins, configures chip-select/data-command/reset GPIOs, initializes the ST7735 black-tab display, and disables text wrapping. Clears the framebuffer to black and presents it. Call once before drawing.

The current project configuration uses SPI1, TX GPIO 11, clock GPIO 10, CS GPIO 9, reset GPIO 12, and DC GPIO 13. These assignments come from [CMakeLists.txt](CMakeLists.txt).

### Buffered drawing functions

All functions in this section return `void` and change RAM only. Call `present()` to make their results visible.

```cpp
void clear(std::uint16_t color = Black);
```

Fills the entire framebuffer with `color`. This does not erase the physical display until its pixels are presented. It does not reset game state or stored sprite data.

```cpp
void drawRectangle(std::int16_t x, std::int16_t y,
                   std::uint8_t width, std::uint8_t height,
                   std::uint16_t color);
```

Draws a filled rectangle with top-left position `(x, y)`. Width and height are pixel counts, not bottom-right coordinates. The driver clips all four edges. Empty rectangles and rectangles entirely outside the display do nothing. For example, `(10, 20, 3, 2)` covers columns 10 through 12 and rows 20 through 21.

```cpp
void drawSprite(std::int16_t x, std::int16_t y,
                const std::uint16_t* pixels,
                std::uint8_t width, std::uint8_t height);
```

Draws a sprite from at least `width * height` RGB565 values, ordered left to right within each row, then top to bottom. `(x, y)` is the top-left of the complete sprite canvas, including transparent padding.

Pixels matching `DisplayManager::Transparent` (`0xF81F`) are skipped, preserving the background already drawn into RAM. This fixed key is shared by all current fish sprites and the converter; callers do not pass it. Every other color is copied exactly, without scaling, blending, or anti-aliasing. The transparent key must not also be an intended visible sprite color. A null pointer or zero dimension does nothing; off-screen pixels are clipped while retaining the source row stride.

Draw the background and obstacles before the fish. To erase an old fish position, redraw the background in RAM and present the old and new regions. Transparency does not erase previously drawn pixels on its own.

```cpp
void drawText(std::uint8_t x, std::uint8_t y, const char* text,
              std::uint16_t color = White,
              std::uint8_t scale = 1,
              std::uint16_t background = Black);
```

Draws one null-terminated line using the driver's five-column, seven-row font. Each character occupies a cell `6 * scale` pixels wide and `7 * scale` pixels tall, including a spacing column. Each cell is filled with `background` before its character is drawn. Scale 2 doubles each glyph pixel in both directions.

The function stops before a character whose full cell would exceed the right edge; it does not wrap. If the full character height will not fit, the call draws nothing. Null text, scale zero, and an origin outside the display also draw nothing. Use printable ASCII; unsupported character codes are drawn as `?` by the driver.

Argument order is **color, scale, background**. The underlying driver's `drawText()` uses a different order. Replacing a long string with a shorter one does not clear the old characters beyond the new string; clear that region or redraw the scene first.

### Presentation

```cpp
void present(std::int16_t x = 0, std::int16_t y = 0,
             std::uint8_t width = Width,
             std::uint8_t height = Height);
```

Sends the selected framebuffer rectangle to the physical display. No arguments means the whole screen. Negative or overflowing regions are clipped; empty and fully off-screen regions do nothing. The framebuffer is unchanged by presentation.

The driver attempts to reserve two DMA channels on first use: TX for pixels and RX to discard received words. A full-width region can transfer contiguously; narrower regions transfer one row at a time. If channels are unavailable, the same framebuffer region is sent through blocking SPI writes. Successful channel reservations are reused.

`present()` waits for completion in both paths. It returns after SPI finishes and command mode is restored. It is not an asynchronous call, and no game updates run automatically while it waits.

```cpp
display.clear();
display.drawRectangle(10, 20, 30, 16, 0x07e0);
display.drawText(4, 4, "Score: 1");
display.present();  // One complete screen update.
```

For scrolling gameplay, redraw the scene in RAM and present every region where the physical pixels need to change, including old object positions. Practice currently presents its full note lane, then updates the accuracy and timer regions only when their values change. This selection is game behavior, not automatic change detection inside `DisplayManager`.

### Complete screen functions

These functions draw their layouts and present them before returning. They do not read input, change the active game, calculate scores, or wait for the player. All return `void`.

| Public function | Parameters and behavior |
| --- | --- |
| `showTitle()` | Clears the frame and displays the current welcome title, border, and ENTER prompt. |
| `showMenu(int selected)` | Clears the frame and highlights choice 0 for Practice or 1 for Swimmy Fish. Other values highlight no choice. |
| `showInstructions(int mode)` | Clears the frame and draws Practice instructions for mode 0 or Swimmy Fish instructions for mode 1, plus the start prompt. Other values leave the instruction body empty. |
| `showResultOptions(int selected)` | Replaces the lower options area while retaining framebuffer content above it. Highlights 0 for Play again or 1 for Exit; other values highlight neither. Currently presents the entire framebuffer. |

Menu selections are defined by `MenuOption::Practice`, `MenuOption::SwimmyFish`, and the size marker `MenuOption::Count` in [main_menu.h](src/game/main_menu.h). `MainMenu::render()` delegates to `showMenu()` so there is one menu layout. Menu input and launching the selected game remain application responsibilities; selecting a label does not implement or start that game.

## Buzzer

Header: [buzzer.h](src/engine/output/buzzer.h). Implementation: [buzzer.cpp](src/engine/output/buzzer.cpp).

```cpp
#include "engine/output/buzzer.h"
```

| Public function | Parameters and behavior |
| --- | --- |
| `explicit Buzzer(unsigned int pin)` | Stores the GPIO and determines its PWM slice/channel. Initializes playback state without starting a tone. The current application uses GPIO 8. |
| `void init()` | Connects the pin to PWM and initializes the output as silent. Call before `play()`. |
| `void play(std::uint32_t frequency, std::uint32_t duration, std::uint8_t volume = 255)` | Starts a PWM tone. Frequency is in hertz; duration is in milliseconds. Volume ranges from 0 (silent) to 255 (50% duty); lower values narrow the pulses to approximate quieter sound, without changing GPIO voltage. Replaces the current tone and its stop deadline. Any argument being zero calls `stop()`. Returns without waiting for the duration. |
| `void stop()` | Immediately sets the output level to zero, disables the PWM slice, and clears playback state. |
| `void update()` | Stops the tone when its deadline has passed. Call regularly; without this call, a timed tone continues playing. |

The tone's end time is checked when `update()` runs, so stopping can be delayed by other work in the loop. The hardware frequency is determined by the PWM divider and wrap settings and can differ slightly from the requested frequency. Because PWM configuration and disabling affect a whole slice, do not independently drive another output on that slice without coordinating it.

```cpp
buzzer.play(440, 100);  // Request a 440 Hz tone lasting 100 milliseconds.
// Keep calling buzzer.update() during subsequent main-loop iterations.
```

## Audio manager status

[audio_manager.h](src/engine/output/audio_manager.h) is currently empty. It declares no class or public functions. Use `Buzzer` directly for the current audio API.
