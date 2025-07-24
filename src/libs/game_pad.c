/**
 * @file game_pad.c
 * @brief PlayStation 1 gamepad input handling implementation
 * 
 * This module provides a high-level interface for PlayStation 1 gamepad input,
 * wrapping the low-level PSX pad library with convenient data structures and
 * functions for button state tracking, analog stick handling, and edge detection.
 * 
 * Features:
 * - Automatic pad system initialization
 * - Support for digital and analog controllers
 * - Button press/release edge detection
 * - Structured access to different button groups (D-pad, face, shoulder, system)
 * - Normalized analog stick values (-1.0 to 1.0)
 * - Connection state monitoring
 * 
 * The module maintains internal buffers for up to 2 controllers and handles
 * the conversion between PSX pad format (inverted button logic) and a more
 * intuitive boolean representation.
 * 
 * Usage:
 * 1. Initialize a gamepad with init_game_pad()
 * 2. Call sync_pad() each frame to update button states
 * 3. Use the various query functions to check button states
 * 4. Clean up with cleanup_game_pad() when done
 * 
 * @author marconvcm
 * @date Current
 */
#include "game_pad.h"
#include <psxpad.h>
#include <string.h>

// Internal pad buffer
static PADTYPE pad_buffer[2][34 / 2];
static bool pad_system_initialized = false;

// Initialize the pad system if not already done
static void ensure_pad_system_init(void)
{
   if (!pad_system_initialized)
   {
      InitPAD((char *)pad_buffer[0], 34, (char *)pad_buffer[1], 34);
      StartPAD();
      ChangeClearPAD(1);
      pad_system_initialized = true;
   }
}

// Convert PSX pad format to our button format
static uint16_t convert_pad_buttons(PADTYPE *pad)
{
   if (pad->stat != 0)
   {
      return 0; // Pad not ready
   }

   // PSX pad buttons are inverted (0 = pressed, 1 = not pressed)
   uint16_t buttons = ~(pad->btn);

   return buttons;
}

// Update button structures from raw button data
static void update_button_states(GamePad *pad)
{
   uint16_t buttons = pad->buttons_raw;

   // D-Pad
   pad->dpad.up = (buttons & PAD_BUTTON_UP) != 0;
   pad->dpad.down = (buttons & PAD_BUTTON_DOWN) != 0;
   pad->dpad.left = (buttons & PAD_BUTTON_LEFT) != 0;
   pad->dpad.right = (buttons & PAD_BUTTON_RIGHT) != 0;

   // Face buttons
   pad->face.triangle = (buttons & PAD_BUTTON_TRIANGLE) != 0;
   pad->face.circle = (buttons & PAD_BUTTON_CIRCLE) != 0;
   pad->face.x = (buttons & PAD_BUTTON_X) != 0;
   pad->face.square = (buttons & PAD_BUTTON_SQUARE) != 0;

   // Shoulder buttons
   pad->shoulder.l1 = (buttons & PAD_BUTTON_L1) != 0;
   pad->shoulder.l2 = (buttons & PAD_BUTTON_L2) != 0;
   pad->shoulder.r1 = (buttons & PAD_BUTTON_R1) != 0;
   pad->shoulder.r2 = (buttons & PAD_BUTTON_R2) != 0;

   // System buttons
   pad->system.select = (buttons & PAD_BUTTON_SELECT) != 0;
   pad->system.start = (buttons & PAD_BUTTON_START) != 0;
   pad->system.l3 = (buttons & PAD_BUTTON_L3) != 0;
   pad->system.r3 = (buttons & PAD_BUTTON_R3) != 0;

   // Analog sticks (L3/R3 states)
   pad->left_stick.pressed = pad->system.l3;
   pad->right_stick.pressed = pad->system.r3;
}

// Update analog stick data
static void update_analog_sticks(GamePad *pad, PADTYPE *psx_pad)
{
   if (pad->analog_mode && psx_pad->len >= 6)
   {
      // Analog data is available
      pad->left_stick.x = psx_pad->rs_x;
      pad->left_stick.y = psx_pad->rs_y;
      pad->right_stick.x = psx_pad->ls_x;
      pad->right_stick.y = psx_pad->ls_y;
   }
   else
   {
      // No analog data, center the sticks
      pad->left_stick.x = 128;
      pad->left_stick.y = 128;
      pad->right_stick.x = 128;
      pad->right_stick.y = 128;
   }
}

GamePad init_game_pad(uint8_t port)
{
   GamePad pad = {0};

   ensure_pad_system_init();

   pad.port = port;
   pad.connected = false;
   pad.analog_mode = false;
   pad.initialized = true;

   // Initialize analog sticks to center position
   pad.left_stick.x = 128;
   pad.left_stick.y = 128;
   pad.right_stick.x = 128;
   pad.right_stick.y = 128;

   return pad;
}

void sync_pad(GamePad *pad)
{
   if (!pad->initialized)
   {
      return;
   }

   PADTYPE *psx_pad = (PADTYPE *)&pad_buffer[pad->port];

   // Check if pad is connected
   if (psx_pad->stat == 0)
   {
      pad->connected = true;
      pad->pad_type = psx_pad->type;

      // Check if analog mode is available
      pad->analog_mode = (psx_pad->type == 0x73) || (psx_pad->type == 0x79);

      // Store previous button state for edge detection
      pad->previous_buttons = pad->buttons_raw;

      // Update button state
      pad->buttons_raw = convert_pad_buttons(psx_pad);

      // Calculate pressed/released buttons
      pad->buttons_pressed = pad->buttons_raw & (~pad->previous_buttons);
      pad->buttons_released = (~pad->buttons_raw) & pad->previous_buttons;

      // Update all button structures
      update_button_states(pad);

      // Update analog sticks if available
      update_analog_sticks(pad, psx_pad);
   }
   else
   {
      pad->connected = false;
      pad->analog_mode = false;

      // Clear all button states
      memset(&pad->dpad, 0, sizeof(pad->dpad));
      memset(&pad->face, 0, sizeof(pad->face));
      memset(&pad->shoulder, 0, sizeof(pad->shoulder));
      memset(&pad->system, 0, sizeof(pad->system));

      pad->buttons_raw = 0;
      pad->buttons_pressed = 0;
      pad->buttons_released = 0;

      // Reset analog sticks to center
      pad->left_stick.x = 128;
      pad->left_stick.y = 128;
      pad->right_stick.x = 128;
      pad->right_stick.y = 128;
      pad->left_stick.pressed = false;
      pad->right_stick.pressed = false;
   }
}

void cleanup_game_pad(GamePad *pad)
{
   if (pad->initialized)
   {
      pad->initialized = false;
   }
}

bool is_button_pressed(const GamePad *pad, uint16_t button)
{
   return (pad->buttons_raw & button) != 0;
}

bool is_button_just_pressed(const GamePad *pad, uint16_t button)
{
   return (pad->buttons_pressed & button) != 0;
}

bool is_button_just_released(const GamePad *pad, uint16_t button)
{
   return (pad->buttons_released & button) != 0;
}

bool is_analog_available(const GamePad *pad)
{
   return pad->connected && pad->analog_mode;
}

float get_analog_x_normalized(const GamePad *pad, bool left_stick)
{
   if (!is_analog_available(pad))
   {
      return 0.0f;
   }

   uint8_t raw_value = left_stick ? pad->left_stick.x : pad->right_stick.x;

   // Convert from 0-255 range to -1.0 to 1.0 range
   return (raw_value - 128.0f) / 128.0f;
}

float get_analog_y_normalized(const GamePad *pad, bool left_stick)
{
   if (!is_analog_available(pad))
   {
      return 0.0f;
   }

   uint8_t raw_value = left_stick ? pad->left_stick.y : pad->right_stick.y;

   // Convert from 0-255 range to -1.0 to 1.0 range
   // Note: Y axis might need to be inverted depending on preference
   return (raw_value - 128.0f) / 128.0f;
}
