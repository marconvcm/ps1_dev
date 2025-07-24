#ifndef GAME_PAD_H
#define GAME_PAD_H

#include <stdint.h>
#include <stdbool.h>

// Pad button definitions
#define PAD_BUTTON_SELECT   0x0001
#define PAD_BUTTON_L3       0x0002
#define PAD_BUTTON_R3       0x0004
#define PAD_BUTTON_START    0x0008
#define PAD_BUTTON_UP       0x0010
#define PAD_BUTTON_RIGHT    0x0020
#define PAD_BUTTON_DOWN     0x0040
#define PAD_BUTTON_LEFT     0x0080
#define PAD_BUTTON_L2       0x0100
#define PAD_BUTTON_R2       0x0200
#define PAD_BUTTON_L1       0x0400
#define PAD_BUTTON_R1       0x0800
#define PAD_BUTTON_TRIANGLE 0x1000
#define PAD_BUTTON_CIRCLE   0x2000
#define PAD_BUTTON_X        0x4000
#define PAD_BUTTON_SQUARE   0x8000

// D-Pad structure for easy access
typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
} DPad;

// Face buttons structure
typedef struct {
    bool triangle;
    bool circle;
    bool x;
    bool square;
} FaceButtons;

// Shoulder buttons structure
typedef struct {
    bool l1;
    bool l2;
    bool r1;
    bool r2;
} ShoulderButtons;

// System buttons structure
typedef struct {
    bool select;
    bool start;
    bool l3;
    bool r3;
} SystemButtons;

// Analog stick structure
typedef struct {
    uint8_t x;
    uint8_t y;
    bool pressed; // L3/R3 button
} AnalogStick;

// Main GamePad structure
typedef struct {
    uint8_t port;
    bool connected;
    bool analog_mode;
    
    // Button states
    DPad dpad;
    FaceButtons face;
    ShoulderButtons shoulder;
    SystemButtons system;
    
    // Analog sticks (if available)
    AnalogStick left_stick;
    AnalogStick right_stick;
    
    // Raw button data for advanced usage
    uint16_t buttons_raw;
    uint16_t buttons_pressed;   // Newly pressed this frame
    uint16_t buttons_released;  // Released this frame
    
    // Internal state
    uint16_t previous_buttons;
    uint8_t pad_type;
    bool initialized;
} GamePad;


GamePad init_game_pad(uint8_t port);
void sync_pad(GamePad* pad);
void cleanup_game_pad(GamePad* pad);

bool is_button_pressed(const GamePad* pad, uint16_t button);
bool is_button_just_pressed(const GamePad* pad, uint16_t button);
bool is_button_just_released(const GamePad* pad, uint16_t button);

bool is_analog_available(const GamePad* pad);
float get_analog_x_normalized(const GamePad* pad, bool left_stick);
float get_analog_y_normalized(const GamePad* pad, bool left_stick);

#endif // GAME_PAD_H
