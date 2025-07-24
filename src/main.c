#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <psxgpu.h>
#include "libs/game_pad.h"

// Length of the ordering table, i.e. the range Z coordinates can have, 0-15 in
// this case. Larger values will allow for more granularity with depth (useful
// when drawing a complex 3D scene) at the expense of RAM usage and performance.
#define OT_LENGTH 16

// Size of the buffer GPU commands and primitives are written to. If the program
// crashes due to too many primitives being drawn, increase this value.
#define BUFFER_LENGTH 8192

/* Framebuffer/display list class */

typedef struct
{
   DISPENV disp_env;
   DRAWENV draw_env;

   uint32_t ot[OT_LENGTH];
   uint8_t buffer[BUFFER_LENGTH];
} RenderBuffer;

typedef struct
{
   RenderBuffer buffers[2];
   uint8_t *next_packet;
   int active_buffer;
} RenderContext;

void setup_context(RenderContext *ctx, int w, int h, int r, int g, int b)
{
   // Place the two framebuffers vertically in VRAM.
   SetDefDrawEnv(&(ctx->buffers[0].draw_env), 0, 0, w, h);
   SetDefDispEnv(&(ctx->buffers[0].disp_env), 0, 0, w, h);
   SetDefDrawEnv(&(ctx->buffers[1].draw_env), 0, h, w, h);
   SetDefDispEnv(&(ctx->buffers[1].disp_env), 0, h, w, h);

   // Set the default background color and enable auto-clearing.
   setRGB0(&(ctx->buffers[0].draw_env), r, g, b);
   setRGB0(&(ctx->buffers[1].draw_env), r, g, b);
   ctx->buffers[0].draw_env.isbg = 1;
   ctx->buffers[1].draw_env.isbg = 1;

   // Initialize the first buffer and clear its OT so that it can be used for
   // drawing.
   ctx->active_buffer = 0;
   ctx->next_packet = ctx->buffers[0].buffer;
   ClearOTagR(ctx->buffers[0].ot, OT_LENGTH);

   // Turn on the video output.
   SetDispMask(1);
}

void flip_buffers(RenderContext *ctx)
{
   // Wait for the GPU to finish drawing, then wait for vblank in order to
   // prevent screen tearing.
   DrawSync(0);
   VSync(0);

   RenderBuffer *draw_buffer = &(ctx->buffers[ctx->active_buffer]);
   RenderBuffer *disp_buffer = &(ctx->buffers[ctx->active_buffer ^ 1]);

   // Display the framebuffer the GPU has just finished drawing and start
   // rendering the display list that was filled up in the main loop.
   PutDispEnv(&(disp_buffer->disp_env));
   DrawOTagEnv(&(draw_buffer->ot[OT_LENGTH - 1]), &(draw_buffer->draw_env));

   // Switch over to the next buffer, clear it and reset the packet allocation
   // pointer.
   ctx->active_buffer ^= 1;
   ctx->next_packet = disp_buffer->buffer;
   ClearOTagR(disp_buffer->ot, OT_LENGTH);
}

void *new_primitive(RenderContext *ctx, int z, size_t size)
{
   // Place the primitive after all previously allocated primitives, then
   // insert it into the OT and bump the allocation pointer.
   RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);
   uint8_t *prim = ctx->next_packet;

   addPrim(&(buffer->ot[z]), prim);
   ctx->next_packet += size;

   // Make sure we haven't yet run out of space for future primitives.
   assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));

   return (void *)prim;
}

// A simple helper for drawing text using PSn00bSDK's debug font API. Note that
// FntSort() requires the debug font texture to be uploaded to VRAM beforehand
// by calling FntLoad().
void draw_text(RenderContext *ctx, int x, int y, int z, const char *text)
{
   RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);

   ctx->next_packet = (uint8_t *)
       FntSort(&(buffer->ot[z]), ctx->next_packet, x, y, text);

   assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));
}

/* Main */

#define SCREEN_XRES 320
#define SCREEN_YRES 240

int main(int argc, const char **argv)
{
   // Initialize the GPU and load the default font texture provided by
   // PSn00bSDK at (960, 0) in VRAM.
   ResetGraph(0);
   FntLoad(960, 0);

   // Set up our rendering context.
   RenderContext ctx;
   setup_context(&ctx, SCREEN_XRES, SCREEN_YRES, 63, 0, 127);

   // Initialize game pad for player 1 (port 0)
   GamePad pad = init_game_pad(0);

   int x = 160, y = 120; // Start in center
   int dx = 0, dy = 0;   // No initial movement

   // Player square color
   int r = 255, g = 255, b = 0;

   for (;;)
   {
      // Sync pad state - this updates all button states
      sync_pad(&pad);

      // Reset movement
      dx = 0;
      dy = 0;

      // Handle D-Pad movement (digital)
      if (pad.dpad.up)
         dy = -2;
      if (pad.dpad.down)
         dy = 2;
      if (pad.dpad.left)
         dx = -2;
      if (pad.dpad.right)
         dx = 2;

      // Handle analog movement if available
      if (is_analog_available(&pad))
      {
         float analog_x = get_analog_x_normalized(&pad, true); // left stick
         float analog_y = get_analog_y_normalized(&pad, true);

         // Add analog movement (more precise)
         dx += (int)(analog_x * 3);
         dy += (int)(analog_y * 3);
      }

      // Update position with bounds checking
      x += dx;
      y += dy;

      if (x < 0)
         x = 0;
      if (x > (SCREEN_XRES - 64))
         x = SCREEN_XRES - 64;
      if (y < 0)
         y = 0;
      if (y > (SCREEN_YRES - 64))
         y = SCREEN_YRES - 64;

      // Change color based on face buttons
      r = pad.face.square ? 255 : 128; // Red component
      g = pad.face.circle ? 255 : 128; // Green component
      b = pad.face.x ? 255 : 0;        // Blue component

      // Special effect for triangle button
      if (pad.face.triangle)
      {
         r = g = b = 255; // White when triangle is pressed
      }

      // Draw the controllable square
      TILE *tile = (TILE *)new_primitive(&ctx, 1, sizeof(TILE));
      setTile(tile);
      setXY0(tile, x, y);
      setWH(tile, 64, 64);
      setRGB0(tile, r, g, b);

      // Display pad status and instructions
      char text_buffer[256];

      if (pad.connected)
      {
         sprintf(text_buffer, "PAD CONNECTED - %s MODE", is_analog_available(&pad) ? "ANALOG" : "DIGITAL");
         draw_text(&ctx, 8, 16, 0, text_buffer);

         draw_text(&ctx, 8, 32, 0, "D-PAD/ANALOG: MOVE SQUARE");
         draw_text(&ctx, 8, 48, 0, "SQUARE: RED, CIRCLE: GREEN, X: BLUE");
         draw_text(&ctx, 8, 64, 0, "TRIANGLE: WHITE");

         // Show button states
         sprintf(text_buffer, "BUTTONS: %s%s%s%s%s%s%s%s",
                 pad.dpad.up ? "UP " : "",
                 pad.dpad.down ? "DN " : "",
                 pad.dpad.left ? "LF " : "",
                 pad.dpad.right ? "RT " : "",
                 pad.face.triangle ? "△ " : "",
                 pad.face.circle ? "○ " : "",
                 pad.face.x ? "× " : "",
                 pad.face.square ? "□ " : "");
         draw_text(&ctx, 8, 80, 0, text_buffer);

         if (is_analog_available(&pad))
         {
            sprintf(text_buffer, "LEFT STICK: X=%d Y=%d %s",
                    pad.left_stick.x, pad.left_stick.y,
                    pad.left_stick.pressed ? "(PRESSED)" : "");
            draw_text(&ctx, 8, 96, 0, text_buffer);
         }
      }
      else
      {
         draw_text(&ctx, 8, 16, 0, "NO PAD CONNECTED");
         draw_text(&ctx, 8, 32, 0, "PLEASE CONNECT A CONTROLLER");
      }

      flip_buffers(&ctx);
   }

   // Cleanup (though this won't be reached in this example)
   cleanup_game_pad(&pad);
   return 0;
}